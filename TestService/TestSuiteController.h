#pragma once

#include "Module.h"

#include "TestSuite.h"

namespace WPEFramework {
namespace TestCore {

class TestSuiteController {
    private:
        class Metadata : public Core::JSON::Container {
            private:
                Metadata(const Metadata&) = delete;
                Metadata& operator=(const Metadata&) = delete;

            public:
                Metadata()
                    : Core::JSON::Container()
                    , TestSuites()
                {
                    Add(_T("testSuites"), &TestSuites);
                }
                ~Metadata() {}

            public:
                Core::JSON::ArrayType<Core::JSON::String> TestSuites;
        };

        TestSuiteController(const TestSuiteController&) = delete;
        TestSuiteController& operator=(const TestSuiteController&) = delete;
        TestSuiteController()
            : _adminLock()
            , _testSuites()
            , _previousTestSuiteName(EMPTY_STRING)
            , _previousTestSuite(nullptr)
        {
        }

    public:
        static TestSuiteController& Instance();

        virtual ~TestSuiteController()
        {
        }

    // TestSuiteController methods
    public:
        typedef Core::IteratorMapType<std::map<string, TestCore::TestSuite*>, TestCore::TestSuite*, string> Iterator;

        inline Iterator TestSuites()
        {
            return (Iterator(_testSuites));
        }

    public:
        string /*JSON*/ Process(const string& path, const uint8_t skipUrl, const string& body /*JSON*/);

        string /*JSON*/ GetTestSuites(void);

        void AnnounceTestSuite(TestCore::TestSuite& testsArea, const string& testSuiteName);
        void RevokeTestSuite(const string& testSuiteName);
        void RevokeAllTestSuites();

    private:
        Core::CriticalSection _adminLock;
        std::map<string, TestCore::TestSuite*> _testSuites;
        string _previousTestSuiteName;
        TestCore::TestSuite* _previousTestSuite;
};

} // namespace TestCore
} // namespace WPEFramework
