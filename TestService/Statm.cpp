#include "Module.h"

#include "TestCommandController.h"
#include "MemoryAllocation.h"
#include "TestCommandMetadata.h"

namespace WPEFramework {

class Statm : public Exchange::ITestUtility::ICommand {
    private:
        Statm(const Statm&) = delete;
        Statm& operator=(const Statm&) = delete;

    public:
        Statm()
            : _memoryAdmin(MemoryAllocation::Instance())
        {
            TestCore::TestCommandController::Instance().Announce(this);
        }

        virtual ~Statm()
        {
            TestCore::TestCommandController::Instance().Revoke(this);
        }

    public:
        // ICommand methods
        string Execute(const string& params) override
        {
            return _memoryAdmin.CreateResponse();
        }

        string Description() const override
        {
            return _description;
        }

        string Signature() const override
        {
            return _signature;
        }

        string Name() const final
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
        string _description = CreateDescription(_T("Provides information about system memory"));
        string _name = _T("Statm");
        string _signature = EMPTY_STRING;//ToDo: Not supported at the moment
};

static Statm* _singleton(Core::Service<Statm>::Create<Statm>());

} // namespace WPEFramework
