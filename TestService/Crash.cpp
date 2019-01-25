#include "Module.h"

#include "TestCommandController.h"
#include "TestCommandMetadata.h"
#include "interfaces/ITestUtility.h"

#include "CrashAdmin.h"

namespace WPEFramework {

class CommandBase : public Exchange::ITestUtility::ICommand {
private:
    CommandBase(const CommandBase&) = delete;
    CommandBase& operator=(const CommandBase&) = delete;

public:
    class Metadata {
    private:
        Metadata& operator=(const Metadata&) = delete;

    public:
        Metadata()
            : _signature()
        {
        }

        Metadata(const Metadata& copy)
            : _signature(copy._signature)
        {
        }

    public:
        class Parameter : public Core::JSON::Container {
        public:
            Parameter()
                : Core::JSON::Container()
                , Name()
                , Type()
                , Description()
            {
                AddFields();
            }

            Parameter(const string& name, const string& type, const string& description)
                : Core::JSON::Container()
                , Name(name)
                , Type(type)
                , Description(Description)
            {
                AddFields();
            }

            Parameter(const Parameter& copy)
                : Name(copy.Name)
                , Type(copy.Type)
                , Description(copy.Description)
            {
            }

            virtual ~Parameter() = default;

            // ToDo: seems like it won't be necessary
            Parameter& operator=(const Parameter& rhs)
            {
                Name = rhs.Name;
                Type = rhs.Type;
                Description = rhs.Description;
            }

        private:
            inline void AddFields()
            {
                Add(_T("name"), &Name);
                Add(_T("type"), &Type);
                Add(_T("description"), &Description);
            }

        public:
            Core::JSON::String Name;
            // ToDo: make type automagically determined
            Core::JSON::String Type;
            Core::JSON::String Description;
        };

        class Signature : public Core::JSON::Container {
        private:
            Signature& operator=(const Signature&) = delete;

        public:
            Signature()
                : Core::JSON::Container()
                , Input()
                , Output()
            {
                AddFields();
            }

            Signature(const Signature& copy)
                : Core::JSON::Container()
                , Input(copy.Input)
                , Output(copy.Output)
            {
                AddFields();
            }

            virtual ~Signature() = default;

        private:
            inline void AddFields()
            {
                Add(_T("input"), &Input);
                Add(_T("output"), &Output);
            }

        public:
            Core::JSON::ArrayType<Parameter> Input;
            Parameter Output;
        };

    public:
        Metadata& Input(const Parameter& param) { _signature.Output = param; }
        Metadata& Output(const Parameter& param) { _signature.Input.Add(param); }

        void ToString(string& result) const { _signature.ToString(result); }

    private:
        Signature _signature;
    };

public:
    CommandBase(const Metadata& metadata)
        : Exchange::ITestUtility::ICommand()
        , _name()
        , _description()
        , _metadata(metadata)
    {
    }

    virtual ~CommandBase() = default;

public:
    // Exchange::ITestUtility::ICommand
    virtual string Execute(const string& params) override { return (_T("Execute not overloaded/implemented")); }

    virtual string Name() const override { return _name; }
    virtual string Description() const override { return _description; }
    virtual string Signature() const override
    {
        string signature = _T("");
        _metadata.ToString(signature);
        return signature;
    }

private:
    string _name;
    string _description;
    Metadata _metadata;
};

class Crash : public CommandBase {
private:
    Crash(const Crash&) = delete;
    Crash& operator=(const Crash&) = delete;

public:
    Crash()
        : CommandBase(CommandBase::Metadata()
                          .Output(CommandBase::Metadata::Parameter(_T("nameOut"), _T("typeOut"), _T("descriptionOut")))
                          .Input(CommandBase::Metadata::Parameter(_T("nameOutIn"), _T("typeIn"), _T("descriptionIn"))))
        , _crashAdmin(CrashAdmin::Instance())
    {
        // ToDo: try to hide it in base class to do it automagically
        TestCore::TestCommandController::Instance().Announce(this);
    }

    // ToDo: try to hide it in base class to do it automagically
    virtual ~Crash() { TestCore::TestCommandController::Instance().Revoke(this); }

    BEGIN_INTERFACE_MAP(Crash)
        INTERFACE_ENTRY(Exchange::ITestUtility::ICommand)
    END_INTERFACE_MAP

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
    // CommandBase methods final override
    virtual string Execute(const string& params) final
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

    virtual string Name() const final { return _name; }
    virtual string Description() const final { return _description; }

private:
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
    string _name = _T("Crash");
    string _description = CreateDescription(_T("Cause segmenation fault resulting in crash"));
};

static Crash* _singleton(Core::Service<Crash>::Create<Crash>());

} // namespace WPEFramework
