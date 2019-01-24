#pragma once

#include "Module.h"

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

    void Crash(const uint8_t crashDelay);
    string /*JSON*/ CrashNTimes(void);

private:
    uint8_t PendingCrashCount(void);
    bool SetPendingCrashCount(uint8_t newCrashCount);
    void ExecPendingCrash(void);

private:
    uint8_t _crashDelay;
};

} // namespace WPEFramework
