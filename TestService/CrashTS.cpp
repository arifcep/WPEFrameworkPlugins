#include "CrashTS.h"

#include "TraceCategories.h"
#include <fstream>

namespace WPEFramework {

/*static*/ constexpr char CrashTS::PendingCrashFilepath[];

static CrashTS* _singleton(Core::Service<CrashTS>::Create<CrashTS>());

//
// TestSuite Methods
//
void CrashTS::Setup(const string& body)
{
    // Store body locally
    _body = body;
}

void CrashTS::Cleanup(void)
{
    // ToDo: Implement
}

//
// CrashTS Methods
///
string /*JSON*/ CrashTS::GetBody(void)
{
    return _body;
}

string CrashTS::Crash(void)
{
    string response = EMPTY_STRING;
    string body = GetBody();
    CrashInputMetadata input;

    if (input.FromString(body))
    {
        _crashDelay = input.CrashDelay;
    }

    TRACE(TestCore::TestOutput, (_T("Preparing for crash in %d seconds"), _crashDelay));
    sleep(_crashDelay);

    TRACE(TestCore::TestOutput, (_T("Executing crash!")));
    uint8_t* tmp = nullptr;
    *tmp = 3; // segmentaion fault

    return response;
}

string CrashTS::CrashParameters(void)
{
    // ToDo: implement
    return _T("");
}

string CrashTS::CrashNTimes(void)
{
    string response = EMPTY_STRING;
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

    return response;
}

string CrashTS::CrashNTimesParameters(void)
{
    // ToDo: implement
    return _T("");
}

uint8_t CrashTS::PendingCrashCount(void)
{
    uint8_t pendingCrashCount = 0;

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

    return pendingCrashCount;
}

bool CrashTS::SetPendingCrashCount(uint8_t newCrashCount)
{
    bool status = false;

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

    return status;
}

void CrashTS::ExecPendingCrash(void)
{
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
}

} // namespace WPEFramework
