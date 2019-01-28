#include "../CommandCore/TestCommandController.h"
#include "MemoryAllocation.h"
#include "../CommandCore/TestCommandMetadata.h"
#include "../CommandCore/TestCommandBase.h"

namespace WPEFramework {

class Free : public TestCommandBase {
    private:
        Free(const Free&) = delete;
        Free& operator=(const Free&) = delete;

    public:
        Free()
            : TestCommandBase(TestCommandBase::DescriptionBuilder("Provides information about system memory"),
                              TestCommandBase::SignatureBuilder(TestCore::TestCommandSignature::Parameter("InName", "InType", "InComments")).
                              AddOutParameter(TestCore::TestCommandSignature::Parameter("OutName", "OutType", "OutComments")))
            , _memoryAdmin(MemoryAllocation::Instance())
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

        string Name() const override
        {
            return _name;
        }

    private:
        BEGIN_INTERFACE_MAP(Free)
            INTERFACE_ENTRY(Exchange::ITestUtility::ICommand)
        END_INTERFACE_MAP

    private:
        MemoryAllocation& _memoryAdmin;
        const string _name = _T("Free");
};

static Free* _singleton(Core::Service<Free>::Create<Free>());

} // namespace WPEFramework
