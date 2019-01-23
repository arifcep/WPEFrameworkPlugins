#include "Module.h"

#include "TestCommandController.h"
#include <interfaces/ITestController.h>

namespace WPEFramework
{
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
                // Return empty result in case of issue
                string /*JSON*/ response = EMPTY_STRING;

                Core::TextSegmentIterator index(Core::TextFragment(path, skipUrl, path.length() - skipUrl), false, '/');

                index.Next();
                index.Next();
                // Here process request other than:
                // /Service/<CALLSIGN>/TestSuites
                // /Service/<CALLSIGN>/<TEST_SUITE_NAME>/...

#if 0
string /*JSON*/ TestCommandController::Process(const string& path, const uint8_t skipUrl, const string& body /*JSON*/)
{
    bool executed = false;
    // Return empty result in case of issue
    string /*JSON*/ response = EMPTY_STRING;

    Core::TextSegmentIterator index(Core::TextFragment(path, skipUrl, path.length() - skipUrl), false, '/');

    index.Next();

    if ((index.Current().Text() == _T("TestSuites")) && (!index.Next()))
    {
        response = GetTestSuites();
        executed = true;
    }
    else
    {
        TestCore::TestSuiteController::Iterator testSuite(TestSuites());
        string currentTestSuiteName = index.Current().Text();

        while (testSuite.Next())
        {
            if (testSuite.Key() == currentTestSuiteName)
            {
                // Found test suite
                if ((currentTestSuiteName != _previousTestSuiteName) && (_previousTestSuiteName != EMPTY_STRING))
                {
                    // Cleanup before run any kind of test from different Test Suite
                    _previousTestSuite->Cleanup();
                }

                index.Next();

                // Get remaining path
                if (index.Remainder().Length() != 0)
                {
                    // Setup each test before execution, it is up to Test Suite to handle Setup method
                    testSuite.Current()->Setup(body);

                    // Leave processing to TestSuite
                    response = testSuite.Current()->Process(path, path.length() - index.Remainder().Length(), body);

                    executed = true;
                    _previousTestSuiteName = currentTestSuiteName;
                    _previousTestSuite = testSuite.Current();
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
#endif

                return response;
            }

            BEGIN_INTERFACE_MAP(TestControllerImplementation)
                INTERFACE_ENTRY(Exchange::ITestController)
            END_INTERFACE_MAP

        private:
            TestCommandController& _testCommandController;
    };

SERVICE_REGISTRATION(TestCommandControllerImp, 1, 0);
}
