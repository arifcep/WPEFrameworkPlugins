#include "Module.h"

#include "TestSuiteController.h"
#include <interfaces/ITestController.h>

namespace WPEFramework
{
    class TestControllerImplementation : public Exchange::ITestController
    {
        private:
            TestControllerImplementation(const TestControllerImplementation&) = delete;
            TestControllerImplementation& operator=(const TestControllerImplementation&) = delete;

        public:
            TestControllerImplementation()
                : _testSuiteController(TestCore::TestSuiteController::Instance())
            {}

            virtual ~TestControllerImplementation() {}

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

                // process TesSuites requests
                response = _testSuiteController.Process(path, path.length() - index.Remainder().Length(), body);

                return response;
            }

            BEGIN_INTERFACE_MAP(TestControllerImplementation)
                INTERFACE_ENTRY(Exchange::ITestController)
            END_INTERFACE_MAP

        private:
            TestCore::TestSuiteController& _testSuiteController;
    };

SERVICE_REGISTRATION(TestControllerImplementation, 1, 0);
}
