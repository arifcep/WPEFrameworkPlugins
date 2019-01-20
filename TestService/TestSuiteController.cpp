#include "TestSuiteController.h"

namespace WPEFramework {
namespace TestCore {

/* static */ TestSuiteController& TestSuiteController::Instance()
{
    static TestSuiteController _singleton;
    return (_singleton);
}

string /*JSON*/ TestSuiteController::Process(const string& path, const uint8_t skipUrl, const string& body /*JSON*/)
{
    bool executed = false;
    // Return empty result in case of issue
    string /*JSON*/ response = EMPTY_STRING;

    Core::TextSegmentIterator index(Core::TextFragment(path, skipUrl, path.length() - skipUrl), false, '/');

    index.Next();

    if (index.Current().Text() == _T("TestSuites"))
    {
        response = GetTestSuites();
        executed = true;
    }
    else
    {
        TestCore::TestSuiteController::Iterator testSuite(TestSuites());
        string currentTestSuiteName = index.Current().Text();

        while (testSuite.Next() == true)
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

string /*JSON*/ TestSuiteController::GetTestSuites(void)
{
    string response;
    Metadata testSuites;

    for (auto& testSuite : _testSuites)
    {
        Core::JSON::String name;
        name = testSuite.first;
        testSuites.TestSuites.Add(name);
    }
    testSuites.ToString(response);

    return response;
}

void TestSuiteController::AnnounceTestSuite(TestCore::TestSuite& testsArea, const string& testSuiteName)
{
    _adminLock.Lock();

    auto index = _testSuites.find(testSuiteName);

    // Announce a tests area only once
    ASSERT(index == _testSuites.end());

    if (index == _testSuites.end())
    {
        _testSuites.insert(std::pair<string, TestCore::TestSuite*>(testSuiteName, &testsArea));
    }

    _adminLock.Unlock();
}

void TestSuiteController::RevokeTestSuite(const string& testSuiteName)
{
    _adminLock.Lock();

    auto index = _testSuites.find(testSuiteName);

    // Only revoke test areas you subscribed !!!!
    ASSERT(index != _testSuites.end());

    if (index != _testSuites.end())
    {
        _testSuites.erase(index);
    }

    _adminLock.Unlock();
}

void TestSuiteController::RevokeAllTestSuite()
{
    _adminLock.Lock();

    _testSuites.clear();

    _adminLock.Unlock();
}

} // namespace TestCore
} // namespace WPEFramework
