#include "../CommandCore/TestCommandController.h"
#include "../CommandCore/TestCommandBase.h"
#include "MemoryAllocation.h"

namespace WPEFramework {

class Malloc : public TestCommandBase {
    private:
        class MallocInputMetadata : public Core::JSON::Container {
            private:
                MallocInputMetadata(const MallocInputMetadata&) = delete;
                MallocInputMetadata& operator=(const MallocInputMetadata&) = delete;

            public:
                MallocInputMetadata()
                    : Core::JSON::Container()
                    , Size(0)
                {
                    Add(_T("size"), &Size);
                }
                ~MallocInputMetadata() {}

            public:
                Core::JSON::DecSInt32 Size;
        };

        Malloc(const Malloc&) = delete;
        Malloc& operator=(const Malloc&) = delete;

    public:
        Malloc()
            : TestCommandBase(TestCommandBase::DescriptionBuilder("Allocates desired kB in memory and holds it"),
                              TestCommandBase::SignatureBuilder(TestCommandBase::Parameter("size", "InType", "memory in kB for allocation"))
                              .AddOutParameter(TestCommandBase::Parameter("allocated", "Int", "allocated memory in kB"))
                              .AddOutParameter(TestCommandBase::Parameter("size", "Int", "[proc/<pid>/statm] size memory in kB"))
                              .AddOutParameter(TestCommandBase::Parameter("resident", "Int", "[proc/<pid>/statm] resident memory in kB")))
            , _memoryAdmin(MemoryAllocation::Instance())
        {
            TestCore::TestCommandController::Instance().Announce(this);
        }

        virtual ~Malloc()
        {
            TestCore::TestCommandController::Instance().Revoke(this);
        }

    public:
        // ICommand methods
        string Execute(const string& params) final
        {
            MallocInputMetadata input;
            uint32_t size;

            if (input.FromString(params))
            {
                size = input.Size;
                _memoryAdmin.Malloc(size);
            }
            return _memoryAdmin.CreateResponse();
        }

        string Name() const final
        {
            return _name;
        }

    private:
        BEGIN_INTERFACE_MAP(Malloc)
            INTERFACE_ENTRY(Exchange::ITestUtility::ICommand)
        END_INTERFACE_MAP

    private:
        MemoryAllocation& _memoryAdmin;
        const string _name = _T("Malloc");
};

static Malloc* _singleton(Core::Service<Malloc>::Create<Malloc>());

} // namespace WPEFramework
