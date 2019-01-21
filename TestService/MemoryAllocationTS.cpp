#include "MemoryAllocationTS.h"

#include "TraceCategories.h"

namespace WPEFramework {

static MemoryAllocationTS* _singleton(Core::Service<MemoryAllocationTS>::Create<MemoryAllocationTS>());

//
// TestSuite Methods
//
void MemoryAllocationTS::Setup(const string& body)
{
    // Store body locally
    _body = body;
}

void MemoryAllocationTS::Cleanup(void)
{
    Free();
}

//
// MemoryAllocationTS Methods
///
string /*JSON*/ MemoryAllocationTS::GetBody(void)
{
    return _body;
}

string /*JSON*/ MemoryAllocationTS::CreateResultResponse()
{
    string response = EMPTY_STRING;

    MemoryOutputMetadata exeResponse;
    uint32_t allocated, size, resident;

    _lock.Lock();
    allocated = _currentMemoryAllocation;
    size = static_cast<uint32_t>(_process.Allocated() >> 10);
    resident = static_cast<uint32_t>(_process.Resident() >> 10);
    _lock.Unlock();

    exeResponse.Allocated = allocated;
    exeResponse.Resident = resident;
    exeResponse.Size = size;
    exeResponse.ToString(response);

    return response;
}

Core::JSON::ArrayType<TestCore::Parameters::Parameter> MemoryAllocationTS::CreateOutputParamsResponse(void)
{
    TestCore::Parameters outputParameters;
    TestCore::Parameters::Parameter allocated;
    TestCore::Parameters::Parameter size;
    TestCore::Parameters::Parameter resident;
    string allocatedName = "allocated";
    string allocatedType = "Int";
    string allocatedComment = "allocated size in kB";

    string sizeName = "size";
    string sizeType = "Int";
    string sizeComment = "proc size memory in kB";

    string residentName = "resident";
    string residentType = "Int";
    string residentComment = "proc resident memory in kB";

    allocated.Name = allocatedName;
    allocated.Type = allocatedType;
    allocated.Comment = allocatedComment;

    size.Name = sizeName;
    size.Type = sizeType;
    size.Comment = sizeComment;

    resident.Name = residentName;
    resident.Type = residentType;
    resident.Comment = residentComment;

    outputParameters.Output.Add(allocated);
    outputParameters.Output.Add(size);
    outputParameters.Output.Add(resident);

    return outputParameters.Output;
}

// Memory Allocation methods
string /*JSON*/ MemoryAllocationTS::Malloc(void) // size in Kb
{
    uint32_t size;
    // Get body metadata
    string body = GetBody();
    MallocInputMetadata input;

    TRACE(TestCore::TestLifeCycle, (_T("Enter Malloc test case")))

    if (input.FromString(body))
    {
        size = input.Size;
        TRACE(TestCore::TestOutput, (_T("Allocate %lu kB"), size))

        uint32_t noOfBlocks = 0;
        uint32_t blockSize = (32 * (getpagesize() >> 10)); // 128kB block size
        uint32_t runs = (uint32_t)size / blockSize;

        _lock.Lock();

        for (noOfBlocks = 0; noOfBlocks < runs; ++noOfBlocks)
        {
            _memory.push_back(malloc(static_cast<size_t>(blockSize << 10)));
            if (!_memory.back())
            {
                TRACE(TestCore::TestOutput, (_T("*** Failed allocation !!! ***")))
                break;
            }

            for (uint32_t index = 0; index < (blockSize << 10); index++)
            {
                static_cast<unsigned char*>(_memory.back())[index] = static_cast<unsigned char>(rand() & 0xFF);
            }
        }

        _currentMemoryAllocation += (noOfBlocks * blockSize);
        _lock.Unlock();
    }
    else
    {
        TRACE(TestCore::TestOutput, (_T("*** Invalid body, Memory is not allocated !!! ***")))
    }

    TRACE(TestCore::TestLifeCycle, (_T("Exit Malloc test case")));
    return CreateResultResponse();
}

string MemoryAllocationTS::MallocParameters(void)
{
    string response = EMPTY_STRING;
    TestCore::Parameters mallocParameters;

    mallocParameters.Output = CreateOutputParamsResponse();

    TestCore::Parameters::Parameter size;
    string sizeName = "size";
    string sizeType = "Int";
    string sizeComment = "allocate memory size in kB";

    size.Name = sizeName;
    size.Type = sizeType;
    size.Comment = sizeComment;

    mallocParameters.Input.Add(size);

    mallocParameters.ToString(response);

    return response;
}

string /*JSON*/ MemoryAllocationTS::Statm(void)
{
    TRACE(TestCore::TestLifeCycle, (_T("Enter Statm test case")))

    uint32_t allocated;
    uint32_t size;
    uint32_t resident;

    _lock.Lock();
    allocated = _currentMemoryAllocation;
    _lock.Unlock();

    size = static_cast<uint32_t>(_process.Allocated() >> 10);
    resident = static_cast<uint32_t>(_process.Resident() >> 10);

    TRACE(TestCore::TestOutput, (_T("Log Memory statistics")))
    LogMemoryUsage();

    TRACE(TestCore::TestLifeCycle, (_T("Exit Statm test case")))
    return CreateResultResponse();
}

string MemoryAllocationTS::StatmParameters(void)
{
    string response = EMPTY_STRING;
    TestCore::Parameters statmOutputParameters;

    statmOutputParameters.Output = CreateOutputParamsResponse();

    statmOutputParameters.ToString(response);

    return response;
}

string /*JSON*/ MemoryAllocationTS::Free(void)
{
    TRACE(TestCore::TestLifeCycle, (_T("Enter Free test case")))

    if (!_memory.empty())
    {
        for (auto const& memoryBlock : _memory)
        {
            free(memoryBlock);
        }
        _memory.clear();
    }
    TRACE(TestCore::TestOutput, (_T("Free allocated memory")))

    _lock.Lock();
    _currentMemoryAllocation = 0;
    _lock.Unlock();

    TRACE(TestCore::TestLifeCycle, (_T("Exit Free test case")))
    return CreateResultResponse();
}

string MemoryAllocationTS::FreeParameters(void)
{
    string response = EMPTY_STRING;
    TestCore::Parameters freeOutputParameters;

    freeOutputParameters.Output = CreateOutputParamsResponse();

    freeOutputParameters.ToString(response);

    return response;
}

void MemoryAllocationTS::DisableOOMKill(void)
{
    int8_t oomNo = -17;
    _process.OOMAdjust(oomNo);
}

void MemoryAllocationTS::LogMemoryUsage(void)
{
    TRACE(TestCore::TestOutput, (_T("*** Current allocated: %lu Kb ***"), _currentMemoryAllocation))
    TRACE(TestCore::TestOutput, (_T("*** Initial Size:     %lu Kb ***"), _startSize))
    TRACE(TestCore::TestOutput, (_T("*** Initial Resident: %lu Kb ***"), _startResident))
    TRACE(TestCore::TestOutput, (_T("*** Size:     %lu Kb ***"), static_cast<uint32_t>(_process.Allocated() >> 10)))
    TRACE(TestCore::TestOutput, (_T("*** Resident: %lu Kb ***"), static_cast<uint32_t>(_process.Resident() >> 10)))
}

} // namespace WPEFramework
