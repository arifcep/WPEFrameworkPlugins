#pragma once

#include "Module.h"

#include "TestCaseController.h"
#include "TestCaseMetadata.h"
#include "TestSuite.h"
#include "TestSuiteController.h"

#include <functional>
using namespace std::placeholders;

namespace WPEFramework {

class CrashTS : public TestCore::TestSuite {
private:
    CrashTS(const CrashTS&) = delete;
    CrashTS& operator=(const CrashTS&) = delete;

public:
    static constexpr uint8_t DefaultCrashDelay = 3;
    static constexpr char PendingCrashFilepath[] = "/tmp/CrashTS.pending";

private:
    class CrashInputMetadata : public Core::JSON::Container {
    private:
        CrashInputMetadata(const CrashInputMetadata&) = delete;
        CrashInputMetadata& operator=(const CrashInputMetadata&) = delete;

    public:
        CrashInputMetadata()
            : Core::JSON::Container()
            , CrashDelay(0)
        {
            Add(_T("crashDelay"), &CrashDelay);
        }
        ~CrashInputMetadata() {}

    public:
        Core::JSON::DecUInt8 CrashDelay;
    };

    class CrashNTimesInputMetadata : public Core::JSON::Container {
    private:
        CrashNTimesInputMetadata(const CrashNTimesInputMetadata&) = delete;
        CrashNTimesInputMetadata& operator=(const CrashNTimesInputMetadata&) = delete;

    public:
        CrashNTimesInputMetadata()
            : Core::JSON::Container()
            , CrashDelay(0)
            , CrashCount(0)
        {
            Add(_T("crashDelay"), &CrashDelay);
            Add(_T("crashCount"), &CrashCount);
        }

        ~CrashNTimesInputMetadata() {}

    public:
        Core::JSON::DecUInt8 CrashDelay;
        Core::JSON::DecUInt8 CrashCount;
    };

public:
    CrashTS()
        : _body()
        , _crashDelay(DefaultCrashDelay)
    {
        // START | Register -- Crash
        TestCore::TestSuite::Register(TestCore::TestCase("Crash", /* Method Name */
            "Crash one time", /* Description */
            std::bind(&CrashTS::Crash, this), /* Executeable */
            std::bind(&CrashTS::CrashParameters, this) /* Parameters */
            ));
        // END | REGISTER -- Crash

        // START | Register -- CrashNTimes
        TestCore::TestSuite::Register(TestCore::TestCase("CrashNTimes", /* Method Name */
            "Crashes N times consequtively", /* Description */
            std::bind(&CrashTS::CrashNTimes, this), /* Executeable */
            std::bind(&CrashTS::CrashNTimesParameters, this) /* Parameters */
            ));
        // END | REGISTER -- CrashNTimes

        TestCore::TestSuiteController::Instance().AnnounceTestSuite(*this, "Crash");

        // execute any pending crashes
        ExecPendingCrash();
    }

    virtual ~CrashTS() {}

public:
    // TestSuite methods
    virtual void Setup(const string& body) override;
    virtual void Cleanup(void) override;

private:
    // TestSuite Methods - TestCases
    string /*JSON*/ Crash(void);
    string CrashParameters(void);

    string /*JSON*/ CrashNTimes(void);
    string CrashNTimesParameters(void);

private:
    string /*JSON*/ GetBody(void);

    uint8_t PendingCrashCount(void);
    bool SetPendingCrashCount(uint8_t newCrashCount);
    void ExecPendingCrash(void);

    BEGIN_INTERFACE_MAP(CrashTS)
    INTERFACE_ENTRY(TestCore::TestSuite)
    END_INTERFACE_MAP

private:
    string _body;
    uint8_t _crashDelay;
};

} // namespace WPEFramework
