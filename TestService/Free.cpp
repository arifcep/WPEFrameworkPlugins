#include "Module.h"

#include "TestCommandController.h"
#include "MemoryAllocation.h"
#include "TestCommandMetadata.h"

namespace WPEFramework {

class Free : public Exchange::ITestUtility::ICommand {
    private:
        Free(const Free&) = delete;
        Free& operator=(const Free&) = delete;

    public:
        Free()
            : _memoryAdmin(MemoryAllocation::Instance())
        {
            TestCore::TestCommandController::Instance().Announce(this);
        }

        virtual ~Free()
        {
            TestCore::TestCommandController::Instance().Revoke(this);
        }

    public:
        // ICommand methods
        string Execute(const string& params) override
        {
            bool status = _memoryAdmin.Free();
            return (status == true ? _memoryAdmin.CreateResponse() : EMPTY_STRING);
        }

        // ToDo: Consider to move it to ICommand Base class
        const string& Description() const override
        {
            return _description;
        }

        // ToDo: Consider to move it to ICommand Base class
        virtual const string& Signature() const override
        {
            return _signature;
        }

        // ToDo: Consider to move it to ICommand Base class
        virtual const string& Name() const override
        {
            return _name;
        }

    private:
        BEGIN_INTERFACE_MAP(MemoryAllocationTS)
            INTERFACE_ENTRY(Exchange::ITestUtility::ICommand)
        END_INTERFACE_MAP

        // ToDo: Move it to ICommand Base Class
        string CreateDescription(const string& description)
        {
            TestCore::TestCommandDescription jsonDescription;
            string outString;

            jsonDescription.Description = description;
            jsonDescription.ToString(outString);

            return outString;
        }

    private:
        MemoryAllocation& _memoryAdmin;
        const string _description = CreateDescription(_T("Releases previously allocated memory"));
        const string _name = _T("Free");
        const string _signature = EMPTY_STRING;//ToDo: Not supported at the moment
};

static Free* _singleton(Core::Service<Free>::Create<Free>());

} // namespace WPEFramework
