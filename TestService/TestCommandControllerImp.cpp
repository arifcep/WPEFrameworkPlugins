#include "Module.h"

#include "TestCommandController.h"
#include <interfaces/ITestController.h>

namespace WPEFramework {
namespace TestCore {

    class TestCommandControllerImp : public Exchange::ITestController
    {
        private:
            TestCommandControllerImp(const TestCommandControllerImp&) = delete;
            TestCommandControllerImp& operator=(const TestCommandControllerImp&) = delete;

        public:
            TestCommandControllerImp()
                : _testCommandController(TestCommandController::Instance())
            {}

            virtual ~TestCommandControllerImp() {}

            // ITestController methods
            string /*JSON*/ Process(const string& path, const uint8_t skipUrl, const string& body /*JSON*/)
            {
                SYSLOG(Trace::Fatal, (_T("*** Process, path: %s***"), path.c_str()));
                bool executed = false;
                // Return empty result in case of issue
                string /*JSON*/ response = EMPTY_STRING;

                Core::TextSegmentIterator index(Core::TextFragment(path, skipUrl, path.length() - skipUrl), false, '/');

                index.Next();
                index.Next();
                // Here process request other than:
                // /Service/<CALLSIGN>/TestCommands
                // /Service/<CALLSIGN>/<TEST_COMMAND_NAME>/...

                if ((index.Current().Text() == _T("TestCommands")) && (!index.Next()))
                {
                    response = _testCommandController.TestCommands();
                    executed = true;
                }
                else
                {
                    Exchange::ITestUtility::ICommand::IIterator* supportedCommands = _testCommandController.Commands();
                     string testCommand = index.Current().Text();

                    while (supportedCommands->Next())
                    {
                        if (supportedCommands->Command()->Name() == testCommand)
                        {
                            // Found test command
                            // Get remaining path to determine type of request
                            if (index.Remainder().Length() == 0)
                            {
                                // Execute test command
                                response = supportedCommands->Command()->Execute(body);
                                executed = true;
                            }
                            else
                            {
                                index.Next();
                                if ((index.Current().Text() == _T("Description")) && (!index.Next()))
                                {
                                    response = supportedCommands->Command()->Description();
                                    executed = true;
                                }
                                else if ((index.Current().Text() == _T("Parameters")) && (!index.Next()))
                                {
                                    response = supportedCommands->Command()->Signature();
                                    executed = true;
                                }
                            }
                            break;
                        }
                    }
                }

                if (!executed)
                {
                    TRACE(Trace::Fatal, (_T("*** Test case method not found !!! ***")))
                }

                return response;
            }

            BEGIN_INTERFACE_MAP(TestControllerImplementation)
                INTERFACE_ENTRY(Exchange::ITestController)
            END_INTERFACE_MAP

        private:
            TestCommandController& _testCommandController;
    };

SERVICE_REGISTRATION(TestCommandControllerImp, 1, 0);
} // namespace TestCore
} // namespace WPEFramewor
