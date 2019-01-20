#pragma once

#include "Module.h"

#include "TestCaseController.h"

namespace WPEFramework {
namespace TestCore {

struct TestSuite : virtual public Core::IUnknown {
public:
    enum { ID = 0x13000123 };

    virtual ~TestSuite() {}

    virtual void Setup(const string& body /*JSON*/) = 0;
    virtual void Cleanup(void) = 0;

public:
    inline void Register(const TestCase& testCase) { _testCaseController.Register(testCase); }
    inline void Unregister(const string& name) { _testCaseController.Unregister(name); };

    string /*JSON*/ Process(const string& path, const uint8_t skipUrl, const string& body /*JSON*/)
    {
        return _testCaseController.Process(path, skipUrl, body);
    }

private:
    TestCaseController _testCaseController;
};
} // namespace TestCore
} // namespace WPEFramework
