#include "TestCommandController.h"

namespace WPEFramework
{

/* static */ TestCommandController& TestCommandController::Instance()
{
    static TestCommandController _singleton;
    return (_singleton);
}

string /*JSON*/ TestCommandController::TestCommands(void)
{
    string response;
    Metadata testCommands;

    for (auto& testCommand : _commands)
    {
        Core::JSON::String name;
        name = testCommand.first;
        testCommands.TestCommands.Add(name);
    }
    testCommands.ToString(response);

    return response;
}

void TestCommandController::Announce(Exchange::ITestUtility::ICommand* command)
{
    ASSERT(command != nullptr);

    _adminLock.Lock();
    _commands[command->Name()] = command;
    _adminLock.Unlock();
}

void TestCommandController::Revoke(Exchange::ITestUtility::ICommand* command)
{
    ASSERT(command != nullptr);

    _adminLock.Lock();
    _commands.erase(command->Name());
    _adminLock.Unlock();
}

Exchange::ITestUtility::ICommand* TestCommandController::Command(const string& name)
{
     Exchange::ITestUtility::ICommand* command = nullptr;
    _adminLock.Lock();
    command = _commands[name];
    _adminLock.Unlock();
    return command;
}

TestCommandController::Iterator TestCommandController::Commands(void)
{
    return (Iterator(_commands));
}
} // namespace WPEFramework
