#pragma once

#include "Module.h"

#include <interfaces/ITestUtility.h>

namespace WPEFramework
{

class TestCommandController
{
    private:
        using TestCommandContainer = std::map<string, Exchange::ITestUtility::ICommand*>;

        class Metadata : public Core::JSON::Container {
            private:
                Metadata(const Metadata&) = delete;
                Metadata& operator=(const Metadata&) = delete;

            public:
                Metadata()
                    : Core::JSON::Container()
                    , TestCommands()
                {
                    Add(_T("testCommands"), &TestCommands);
                }
                ~Metadata() {}

            public:
                Core::JSON::ArrayType<Core::JSON::String> TestCommands;
        };

        TestCommandController(const TestCommandController&) = delete;
        TestCommandController& operator=(const TestCommandController&) = delete;
        TestCommandController()
            : _adminLock()
            , _commands()
        {
        }

    public:
        static TestCommandController& Instance();
        ~TestCommandController() = default;

        typedef Core::IteratorMapType<TestCommandContainer, Exchange::ITestUtility::ICommand*, string> Iterator;

        void Announce(Exchange::ITestUtility::ICommand* command);
        void Revoke(Exchange::ITestUtility::ICommand* command);
        Exchange::ITestUtility::ICommand* Command(const string& name);
        Iterator Commands(void);//ToDo: Can I leave implementation of iterator like that
        string /*JSON*/ TestCommands(void);

    private:
        mutable Core::CriticalSection _adminLock;
        TestCommandContainer _commands;
};
} // namespace WPEFramework
