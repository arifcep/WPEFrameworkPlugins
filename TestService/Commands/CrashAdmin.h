#pragma once

#include "../Module.h"

#include "../CommandCore/TraceCategories.h"
#include <fstream>

namespace WPEFramework {

class CrashAdmin {
private:
    CrashAdmin(const CrashAdmin&) = delete;
    CrashAdmin& operator=(const CrashAdmin&) = delete;

public:
    static constexpr uint8_t DefaultCrashDelay = 3;
    static constexpr char PendingCrashFilepath[] = "/tmp/CrashAdmin.pending";

private:
    // ToDo: Move out to CrashNTime command
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
    CrashAdmin()
        : _crashDelay(DefaultCrashDelay)
    {
    }

    virtual ~CrashAdmin() = default;

    static CrashAdmin& Instance()
    {
        static CrashAdmin _singleton;
        return (_singleton);
    }

    void Crash(const uint8_t crashDelay)
    {
        string response = EMPTY_STRING;

        TRACE(TestCore::TestOutput, (_T("Preparing for crash in %d seconds"), _crashDelay));
        sleep(_crashDelay);

        TRACE(TestCore::TestOutput, (_T("Executing crash!")));
        uint8_t* tmp = nullptr;
        *tmp = 3; // segmentaion fault

        return;
    }

    string /*JSON*/ CrashNTimes(void)
    {
        string response = EMPTY_STRING;
#if TODO_MAKE_CRASH_N_TIMES_WORK
        string body = GetBody();
        CrashNTimesInputMetadata input;
        uint8_t crashCount = 0;
        uint8_t pendingCrashCount = PendingCrashCount();

        if (input.FromString(body))
        {
            _crashDelay = input.CrashDelay;
            crashCount = input.CrashCount;
        }

        if (pendingCrashCount != 0)
        {
            TRACE(TestCore::TestOutput, (_T("Pending crash already in progress")));
        }
        else
        {
            if (!SetPendingCrashCount(crashCount))
            {
                TRACE(Trace::Fatal, (_T("Failed to set new pending crash count")));
            }
            else
            {
                ExecPendingCrash();
            }
        }
#endif

        return response;
    }

private:
    uint8_t PendingCrashCount(void)
    {
        uint8_t pendingCrashCount = 0;
#if TODO_MAKE_CRASH_N_TIMES_WORK
        std::ifstream pendingCrashFile;
        pendingCrashFile.open(PendingCrashFilepath, std::fstream::binary);

        if (pendingCrashFile.is_open())
        {
            uint8_t readVal = 0;

            pendingCrashFile >> readVal;
            if (pendingCrashFile.good())
            {
                pendingCrashCount = readVal;
            }
            else
            {
                TRACE(TestCore::TestOutput, (_T("Failed to read value from pendingCrashFile")));
            }
        }
#endif
        return pendingCrashCount;
    }

    bool SetPendingCrashCount(uint8_t newCrashCount)
    {
        bool status = false;
#if TODO_MAKE_CRASH_N_TIMES_WORK
        std::ofstream pendingCrashFile;
        pendingCrashFile.open(PendingCrashFilepath, std::fstream::binary | std::fstream::trunc);

        if (pendingCrashFile.is_open())
        {

            pendingCrashFile << newCrashCount;

            if (pendingCrashFile.good())
            {
                status = true;
            }
            else
            {
                TRACE(TestCore::TestOutput, (_T("Failed to write value to pendingCrashFile ")));
            }
            pendingCrashFile.close();
        }
#endif
        return status;
    }

    void ExecPendingCrash(void)
    {
#if TODO_MAKE_CRASH_N_TIMES_WORK
        uint8_t pendingCrashCount = PendingCrashCount();
        if (pendingCrashCount > 0)
        {
            pendingCrashCount--;
            if (SetPendingCrashCount(pendingCrashCount))
            {
                Crash();
            }
            else
            {
                TRACE(Trace::Fatal, (_T("Failed to set new pending crash count")));
            }
        }
        else
        {
            TRACE(TestCore::TestOutput, (_T("No pending crash")));
        }
#endif
    }

private:
    uint8_t _crashDelay;
};
#if TODO_MAKE_CRASH_N_TIMES_WORK
/*static*/ constexpr char CrashAdmin::PendingCrashFilepath[];
#endif
} // namespace WPEFramework
