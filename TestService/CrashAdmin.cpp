#include "CrashAdmin.h"

#include "TraceCategories.h"
#include <fstream>

namespace WPEFramework {

/*static*/ constexpr char CrashAdmin::PendingCrashFilepath[];

void CrashAdmin::Crash(const uint8_t crashDelay)
{
    string response = EMPTY_STRING;

    TRACE(TestCore::TestOutput, (_T("Preparing for crash in %d seconds"), _crashDelay));
    sleep(_crashDelay);

    TRACE(TestCore::TestOutput, (_T("Executing crash!")));
    uint8_t* tmp = nullptr;
    *tmp = 3; // segmentaion fault

    return;
}

// ToDo: Think about way to execute it somehow
string CrashAdmin::CrashNTimes(void)
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

uint8_t CrashAdmin::PendingCrashCount(void)
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

bool CrashAdmin::SetPendingCrashCount(uint8_t newCrashCount)
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

void CrashAdmin::ExecPendingCrash(void)
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

} // namespace WPEFramework
