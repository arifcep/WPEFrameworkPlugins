#include "Module.h"

#include "TestCommandController.h"
#include "TestCommandMetadata.h"
#include "interfaces/ITestUtility.h"

#include "CrashAdmin.h"

namespace WPEFramework {

class Crash : public Exchange::ITestUtility::ICommand {
private:
    Crash(const Crash&) = delete;
    Crash& operator=(const Crash&) = delete;

public:
    Crash()
        : _crashAdmin(CrashAdmin::Instance())
    {
        TestCore::TestCommandController::Instance().Announce(this);
    }

    virtual ~Crash() { TestCore::TestCommandController::Instance().Revoke(this); }

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
        ~CrashInputMetadata() = default;

    public:
        Core::JSON::DecUInt8 CrashDelay;
    };

public:
    // ICommand methods
    string Execute(const string& params) override
    {
        CrashInputMetadata input;
        uint8_t crashDelay = CrashAdmin::DefaultCrashDelay;

        if (input.FromString(params))
        {
            crashDelay = input.CrashDelay;
        }

        _crashAdmin.Crash(crashDelay);

        return (_T("Function should never return"));
    }

    // ToDo: Consider to move it to ICommand Base class
    virtual string Description() const override { return _description; }

    // ToDo: Consider to move it to ICommand Base class
    virtual string Signature() const override { return _signature; }

    // ToDo: Consider to move it to ICommand Base class
    virtual string Name() const override { return _name; }

private:
    BEGIN_INTERFACE_MAP(Crash)
    INTERFACE_ENTRY(Exchange::ITestUtility::ICommand)
    END_INTERFACE_MAP

    // ToDo: Move it to ICommand Base Class
    // ToDO: Consider having such generic function also reusable by other commands
    string CreateDescription(const string& description)
    {
        TestCore::TestCommandDescription jsonDescription;
        string outString;

        jsonDescription.Description = description;
        jsonDescription.ToString(outString);

        return outString;
    }

private:
    CrashAdmin& _crashAdmin;
    string _description = CreateDescription(_T("Cause segmenation fault resulting in crash"));
    string _name = _T("Crash");
    string _signature = EMPTY_STRING; // ToDo: Not supported at the moment
};

static Crash* _singleton(Core::Service<Crash>::Create<Crash>());

} // namespace WPEFramework
