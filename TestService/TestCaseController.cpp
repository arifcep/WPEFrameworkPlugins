#include "TestCaseController.h"

namespace WPEFramework {
namespace TestCore {

string /*JSON*/ TestCaseController::Process(const string& path, const uint8_t skipUrl, const string& /* body */)
{
    bool executed = false;
    // Return empty result in case of issue
    string /*JSON*/ response = EMPTY_STRING;

    Core::TextSegmentIterator index(Core::TextFragment(path, skipUrl, path.length() - skipUrl), false, '/');
    index.Next();
    string testCaseName = index.Current().Text();

    if (testCaseName == _T("TestCases"))
    {
        // API /TestCases
        response = GetTestCaseList();
        executed = true;
    }
    else
    {
        TestCaseIterator testCase(TestCases());
        while (testCase.Next() == true)
        {
            if (testCase.Current().Name() == testCaseName)
            {
                if (index.Next() == false)
                {
                    // API /<TESTCASE_NAME>/
                    response = testCase.Current().Execute();
                    executed = true;
                }
                else
                {
                    string current = index.Current().Text();
                    if (current == _T("Parameters"))
                    {
                        // API /<TESTCASE_NAME/Parameters
                        // ToDo: Create proper response
                        response = testCase.Current().Parameters();
                        executed = true;
                    }
                    else if (current == _T("Description"))
                    {
                        // API /<TESTCASE_NAME>/Description
                        response = CreateDescriptionResponse(testCase.Current().Description());
                        executed = true;
                    }
                    else
                    {
                        // ToDo: handle this case properly
                        TRACE(Trace::Information, (_T("UNSUPPORTED METHOD")));
                        response = EMPTY_STRING;
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

} // namespace TestCore
} // namespace WPEFramework
