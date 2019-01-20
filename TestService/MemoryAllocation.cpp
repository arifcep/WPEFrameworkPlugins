#include "MemoryAllocation.h"

namespace WPEFramework {

static MemoryAllocation* _singleton(Core::Service<MemoryAllocation>::Create<MemoryAllocation>());

//
// TestSuite Methods
//
void MemoryAllocation::Setup(const string& body)
{
    // Store body locally
    _body = body;
}

void MemoryAllocation::Cleanup(void)
{
    Free();
}

//
// MemoryAllocation Methods
///
string /*JSON*/ MemoryAllocation::GetBody()
{
    return _body;
}

string /*JSON*/ MemoryAllocation::CreateResultResponse()
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

// Memory Allocation methods
string /*JSON*/ MemoryAllocation::Malloc(void) // size in Kb
{
    uint32_t size;
    // Get body metadata
    string body = GetBody();
    MallocInputMetadata input;

    if (input.FromString(body))
    {
        size = input.Size;
        TRACE(Trace::Information, (_T("*** Allocate %lu Kb ***"), size))

        uint32_t noOfBlocks = 0;
        uint32_t blockSize = (32 * (getpagesize() >> 10)); // 128kB block size
        uint32_t runs = (uint32_t)size / blockSize;

        _lock.Lock();

        for (noOfBlocks = 0; noOfBlocks < runs; ++noOfBlocks)
        {
            _memory.push_back(malloc(static_cast<size_t>(blockSize << 10)));
            if (!_memory.back())
            {
                TRACE(Trace::Fatal, (_T("*** Failed allocation !!! ***")))
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
        TRACE(Trace::Fatal, (_T("*** Invalid POST Body, Memory is not allocated !!! ***")))
    }

    return CreateResultResponse();
}

string MemoryAllocation::MallocParameters(void)
{
    // ToDo: Do proper implementation
    return _T("");
}

string /*JSON*/ MemoryAllocation::Statm(void)
{
    TRACE(Trace::Information, (_T("*** TestServiceImplementation::Statm ***")))

    uint32_t allocated;
    uint32_t size;
    uint32_t resident;

    _lock.Lock();
    allocated = _currentMemoryAllocation;
    _lock.Unlock();

    size = static_cast<uint32_t>(_process.Allocated() >> 10);
    resident = static_cast<uint32_t>(_process.Resident() >> 10);

    return CreateResultResponse();
}

string MemoryAllocation::StatmParameters(void)
{
    // ToDo: Do proper implementation
    return _T("");
}

string /*JSON*/ MemoryAllocation::Free(void)
{
    TRACE(Trace::Information, (_T("*** TestServiceImplementation::Free ***")))

    if (!_memory.empty())
    {
        for (auto const& memoryBlock : _memory)
        {
            free(memoryBlock);
        }
        _memory.clear();
    }

    _lock.Lock();
    _currentMemoryAllocation = 0;
    _lock.Unlock();

    return CreateResultResponse();
}

string MemoryAllocation::FreeParameters(void)
{
    // ToDo: Do proper implementation
    return _T("");
}

void MemoryAllocation::DisableOOMKill()
{
    int8_t oomNo = -17;
    _process.OOMAdjust(oomNo);
}

void MemoryAllocation::LogMemoryUsage(void)
{
    TRACE(Trace::Information, (_T("*** Current allocated: %lu Kb ***"), _currentMemoryAllocation))
    TRACE(Trace::Information, (_T("*** Initial Size:     %lu Kb ***"), _startSize))
    TRACE(Trace::Information, (_T("*** Initial Resident: %lu Kb ***"), _startResident))
    TRACE(Trace::Information, (_T("*** Size:     %lu Kb ***"), static_cast<uint32_t>(_process.Allocated() >> 10)))
    TRACE(Trace::Information, (_T("*** Resident: %lu Kb ***"), static_cast<uint32_t>(_process.Resident() >> 10)))
}

} // namespace WPEFramework
