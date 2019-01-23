#include "Module.h"

#include "TestCommandController.h"
#include "MemoryAllocation.h"
#include "TestCommandMetadata.h"

namespace WPEFramework {

class Malloc : public Exchange::ITestUtility::ICommand {
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
            : _memoryAdmin(MemoryAllocation::Instance())
        {
            TestCore::TestCommandController::Instance().Announce(this);
        }

        virtual ~Malloc()
        {
            TestCore::TestCommandController::Instance().Revoke(this);
        }

    public:
        // ICommand methods
        string Execute(const string& params) override
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

        // ToDo: Consider to move it to ICommand Base class
        string CreateDescription(const string& description)
        {
            TestCore::TestCommandDescription jsonDescription;
            string outString;

            jsonDescription.Description = description;
            jsonDescription.ToString(outString);

            return outString;
        }

    private:
        BEGIN_INTERFACE_MAP(MemoryAllocationTS)
            INTERFACE_ENTRY(Exchange::ITestUtility::ICommand)
        END_INTERFACE_MAP

    private:
        MemoryAllocation& _memoryAdmin;
        const string _description = CreateDescription(_T("Allocates desired kB in memory and holds it"));
        const string _name = _T("Malloc");
        const string _signature = EMPTY_STRING;//ToDo: Not supported at the moment
};

static Malloc* _singleton(Core::Service<Malloc>::Create<Malloc>());

} // namespace WPEFramework
