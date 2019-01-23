#pragma once

#include "Module.h"

namespace WPEFramework {
namespace TestCore {

class TestCommandDescription : public Core::JSON::Container {
private:
    TestCommandDescription(const TestCommandDescription&) = delete;
    TestCommandDescription& operator=(const TestCommandDescription&) = delete;

public:
    TestCommandDescription()
        : Core::JSON::Container()
        , Description()
    {
        Add(_T("description"), &Description);
    }

    ~TestCommandDescription() {}

public:
    Core::JSON::String Description;
};

class TestCommandParameters : public Core::JSON::Container {
private:
    TestCommandParameters(const TestCommandParameters&) = delete;
    TestCommandParameters& operator=(const TestCommandParameters&) = delete;

public:
    class TestCommand : public Core::JSON::Container {
    public:
        TestCommand& operator=(const TestCommand& rhs)
        {
            this->Name = rhs.Name;
            this->Type = rhs.Type;
            this->Comment = rhs.Comment;

            return *this;
        }

        TestCommand()
            : Core::JSON::Container()
            , Name()
            , Type()
            , Comment()

        {
            addFields();
        }

        TestCommand(const string& name, const string& type, const string& comment)
            : Core::JSON::Container()
            , Name(name)
            , Type(type)
            , Comment(comment)
        {
            addFields();
        }

        TestCommand(const TestCommand& copy)
            : Core::JSON::Container()
            , Name(copy.Name)
            , Type(copy.Type)
            , Comment(copy.Comment)
        {
            addFields();
        }

        ~TestCommand() {}

    private:
        void addFields()
        {
            Add(_T("name"), &Name);
            Add(_T("type"), &Type);
            Add(_T("comment"), &Comment);
        }

    public:
        Core::JSON::String Name;
        Core::JSON::String Type;
        Core::JSON::String Comment;
    };

public:
    TestCommandParameters()
        : Core::JSON::Container()
        , Input()
        , Output()
    {
        Add(_T("input"), &Input);
        Add(_T("output"), &Output);
    }
    ~TestCommandParameters() {}

public:
    Core::JSON::ArrayType<TestCommand> Input;
    Core::JSON::ArrayType<TestCommand> Output;
};
} // namespace TestCore
} // namespace WPEFramework
