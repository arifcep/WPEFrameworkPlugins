#pragma once

#include "Module.h"

#include "TestCaseMetadata.h"
#include "TestSuite.h"
#include "TestSuiteController.h"
#include "TestCaseController.h"

#include <functional>
using namespace std::placeholders;

namespace WPEFramework {

class MemoryAllocation : public TestCore::TestSuite {
private:
    class MemoryOutputMetadata : public Core::JSON::Container {
    private:
        MemoryOutputMetadata(const MemoryOutputMetadata&) = delete;
        MemoryOutputMetadata& operator=(const MemoryOutputMetadata&) = delete;

    public:
        MemoryOutputMetadata()
            : Core::JSON::Container()
            , Allocated(0)
            , Size(0)
            , Resident(0)
        {
            Add(_T("allocated"), &Allocated);
            Add(_T("size"), &Size);
            Add(_T("resident"), &Resident);
        }
        ~MemoryOutputMetadata() {}

    public:
        Core::JSON::DecSInt32 Allocated;
        Core::JSON::DecSInt32 Size;
        Core::JSON::DecSInt32 Resident;
    };

    class MallocInputMetadata : public Core::JSON::Container {
    private:
        MallocInputMetadata(const MallocInputMetadata&) = delete;
        MallocInputMetadata& operator=(const MallocInputMetadata&) = delete;

    public:
        MallocInputMetadata()
            : Core::JSON::Container()
            , Size(0)
        {
            Add(_T("size"), &Size);
        }
        ~MallocInputMetadata() {}

    public:
        Core::JSON::DecSInt32 Size;
    };

private:
    MemoryAllocation(const MemoryAllocation&) = delete;
    MemoryAllocation& operator=(const MemoryAllocation&) = delete;

public:
    MemoryAllocation()
        : _currentMemoryAllocation(0)
        , _lock()
        , _process()
        , _body()
    {
        DisableOOMKill();
        _startSize = static_cast<uint32_t>(_process.Allocated() >> 10);
        _startResident = static_cast<uint32_t>(_process.Resident() >> 10);

        // START | Register -- Statm
        TestCore::TestSuite::Register(TestCore::TestCase("Statm", /* Method Name */
            "Provides information about system memory", /* Description */
            std::bind(&MemoryAllocation::Statm, this), /* Executeable */
            std::bind(&MemoryAllocation::StatmParameters, this) /* Parameters */
            ));
        // END | REGISTER -- Statm

        // START | Register -- Malloc
        TestCore::TestSuite::Register(TestCore::TestCase("Malloc", /* Method Name */
            "Allocates desired kB in memory and holds it", /* Description */
            std::bind(&MemoryAllocation::Malloc, this), /* Executeable */
            std::bind(&MemoryAllocation::MallocParameters, this) /* Parameters */
            ));
        // END | REGISTER -- Malloc

        // START | Register -- Free
        TestCore::TestSuite::Register(TestCore::TestCase("Free", /* Method Name */
            "Releases previously allocated memory", /* Description */
            std::bind(&MemoryAllocation::Free, this), /* Executeable */
            std::bind(&MemoryAllocation::FreeParameters, this) /* Parameters */
            ));
        // END | REGISTER -- Free

        TestCore::TestSuiteController::Instance().AnnounceTestSuite(*this, "Memory");
    }

    virtual ~MemoryAllocation() { Free(); }

public:
    // ITestSuite methods
    virtual void Setup(const string& body) override;
    virtual void Cleanup(void) override;

private:
    // TestSuite Methods - TestCases
    string /*JSON*/ Statm(void);
    string StatmParameters(void);

    string /*JSON*/ Malloc(void);
    // ToDo: implement
    string MallocParameters(void);

    string /*JSON*/ Free(void);
    string FreeParameters(void);

private:
    void DisableOOMKill(void);
    void LogMemoryUsage(void);

    string /*JSON*/ GetBody(void);
    string /*JSON*/ CreateResultResponse();

    BEGIN_INTERFACE_MAP(MemoryAllocation)
    INTERFACE_ENTRY(TestCore::TestSuite)
    END_INTERFACE_MAP

private:
    uint32_t _startSize;
    uint32_t _startResident;
    Core::CriticalSection _lock;
    Core::ProcessInfo _process;
    std::list<void*> _memory;
    uint32_t _currentMemoryAllocation; // size in Kb

    string _body;
};

} // namespace WPEFramework
