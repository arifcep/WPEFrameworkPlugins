#include "../CommandCore/TestCommandController.h"
#include "../CommandCore/TestCommandBase.h"
#include "MemoryAllocation.h"

namespace WPEFramework {

class Statm : public TestCommandBase {
    private:
        Statm(const Statm&) = delete;
        Statm& operator=(const Statm&) = delete;

    public:
        Statm()
            : TestCommandBase(TestCommandBase::DescriptionBuilder("Provides information about system memory allocation"),
                              TestCommandBase::SignatureBuilder()
                              .AddOutParameter(TestCommandBase::Parameter("allocated", "Int", "allocated memory in kB"))
                              .AddOutParameter(TestCommandBase::Parameter("size", "Int", "[proc/<pid>/statm] size memory in kB"))
                              .AddOutParameter(TestCommandBase::Parameter("resident", "Int", "[proc/<pid>/statm] resident memory in kB")))
            , _memoryAdmin(MemoryAllocation::Instance())
        {
            TestCore::TestCommandController::Instance().Announce(this);
        }

        virtual ~Statm()
        {
            TestCore::TestCommandController::Instance().Revoke(this);
        }

    public:
        // ICommand methods
        string Execute(const string& params) final
        {
            return _memoryAdmin.CreateResponse();
        }

        string Name() const final
        {
            return _name;
        }

    private:
        BEGIN_INTERFACE_MAP(Statm)
            INTERFACE_ENTRY(Exchange::ITestUtility::ICommand)
        END_INTERFACE_MAP

    private:
        MemoryAllocation& _memoryAdmin;
        const string _name = _T("Statm");
};

static Statm* _singleton(Core::Service<Statm>::Create<Statm>());

} // namespace WPEFramework
