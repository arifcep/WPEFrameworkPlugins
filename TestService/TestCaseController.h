#pragma once

#include "Module.h"

#include "TestCaseMetadata.h"
#include <functional>

namespace WPEFramework {
namespace TestCore {

class TestCase {
private:
    TestCase& operator=(const TestCase&) = delete;

public:
    typedef std::function<string(void)> TestCaseMethod;

public:
    TestCase(const string& name, const string& description, const TestCaseMethod& executableMethod,
        const TestCaseMethod& parametersMethod)
        : _name(name)
        , _description(description)
        , _executable(executableMethod)
        , _parameters(parametersMethod)
    {
    }

    TestCase(const TestCase& copy)
        : _name(copy._name)
        , _description(copy._description)
        , _executable(copy._executable)
        , _parameters(copy._parameters)
    {
    }

    ~TestCase() {}

public:
    inline const string& Name() const { return _name; }
    inline string Description() const { return _description; };
    inline string Execute() const { return _executable(); };
    inline string Parameters() const { return _parameters(); };

private:
    string _name;
    string _description;
    std::function<string(void)> _executable;
    std::function<string(void)> _parameters;
};

class TestCaseController {
private:
    TestCaseController(const TestCaseController&) = delete;
    TestCaseController& operator=(const TestCaseController&) = delete;

public:
    TestCaseController()
        : _testCases()
    {
    }

    ~TestCaseController() {}

public:
    typedef Core::IteratorType<const std::list<TestCase>, const TestCase&, std::list<TestCase>::const_iterator>
        TestCaseIterator;

public:
    inline TestCaseIterator TestCases() { return (TestCaseIterator(_testCases)); }

    void Register(const TestCase& testCase);
    void Unregister(const string& name);
    void UnregisterAll();

public:
    string /*JSON*/ Process(const string& path, const uint8_t skipUrl, const string& body);

private:
    string /*JSON*/ GetTestCaseList() const
    {
        string response = EMPTY_STRING;

        TestCore::TestCases testCasesListResponse;
        for (auto& testCase : _testCases)
        {
            Core::JSON::String name;
            name = testCase.Name();
            testCasesListResponse.TestCaseNames.Add(name);
        }
        testCasesListResponse.ToString(response);

        return response;
    }

    string /*JSON*/ CreateDescriptionResponse(const string& description)
    {
        string response = EMPTY_STRING;
        TestCore::TestCaseDescription tcDescriptionResponse;

        tcDescriptionResponse.Description = description;
        tcDescriptionResponse.ToString(response);

        return response;
    }

private:
    std::list<TestCase> _testCases;
};

} // namespace TestCore
} // namespace WPEFramework
