#include "TestService.h"

namespace WPEFramework {
namespace TestService {

Exchange::IMemory* MemoryObserver(const uint32_t pid)
{
    class MemoryObserverImpl : public Exchange::IMemory {
    private:
        MemoryObserverImpl();
        MemoryObserverImpl(const MemoryObserverImpl&);
        MemoryObserverImpl& operator=(const MemoryObserverImpl&);

    public:
        MemoryObserverImpl(const uint32_t id)
            : _main(id == 0 ? Core::ProcessInfo().Id() : id)
            , _observable(false)
        {
        }
        ~MemoryObserverImpl() {}

    public:
        virtual void Observe(const uint32_t pid)
        {
            if (pid == 0)
            {
                _observable = false;
            }
            else
            {
                _observable = true;
                _main = Core::ProcessInfo(pid);
            }
        }
        virtual uint64_t Resident() const { return (_observable == false ? 0 : _main.Resident()); }

        virtual uint64_t Allocated() const { return (_observable == false ? 0 : _main.Allocated()); }

        virtual uint64_t Shared() const { return (_observable == false ? 0 : _main.Shared()); }

        virtual uint8_t Processes() const { return (IsOperational() ? 1 : 0); }

        virtual const bool IsOperational() const { return (_observable == false) || (_main.IsActive()); }

        BEGIN_INTERFACE_MAP(MemoryObserverImpl)
        INTERFACE_ENTRY(Exchange::IMemory)
        END_INTERFACE_MAP

    private:
        Core::ProcessInfo _main;
        bool _observable;
    };

    return (Core::Service<MemoryObserverImpl>::Create<Exchange::IMemory>(pid));
}
} // namespace TestService

namespace Plugin {
SERVICE_REGISTRATION(TestService, 1, 0);

/* virtual */ const string TestService::Initialize(PluginHost::IShell* service)
{
    /*Assume that everything is OK*/
    string message = EMPTY_STRING;
    Config config;

    ASSERT(service != nullptr);
    ASSERT(_service == nullptr);
    ASSERT(_testUtilityImp == nullptr);
    ASSERT(_memory == nullptr);

    _service = service;
    _skipURL = static_cast<uint8_t>(_service->WebPrefix().length());
    _service->Register(&_notification);

    _testUtilityImp = _service->Root<Exchange::ITestUtility>(_pid, ImplWaitTime, _T("TestUtilityImp"));

    if ((_testUtilityImp != nullptr) && (_service != nullptr))
    {
        _memory = WPEFramework::TestService::MemoryObserver(_pid);
        ASSERT(_memory != nullptr);
        _memory->Observe(_pid);
    }
    else
    {
        ProcessTermination(_pid);
        _service = nullptr;
        _testUtilityImp = nullptr;
        _service->Unregister(&_notification);

        TRACE(Trace::Fatal, (_T("*** TestService could not be instantiated ***")))
        message = _T("TestService could not be instantiated.");
    }

    return message;
}

/* virtual */ void TestService::Deinitialize(PluginHost::IShell* service)
{
    ASSERT(_service == service);
    ASSERT(_testUtilityImp != nullptr);
    ASSERT(_memory != nullptr);
    ASSERT(_pid);

    TRACE(Trace::Information, (_T("*** OutOfProcess Plugin is properly destructed. PID: %d ***"), _pid))

    ProcessTermination(_pid);
    _testUtilityImp = nullptr;
    _memory->Release();
    _memory = nullptr;
    _service->Unregister(&_notification);
    _service = nullptr;
}

/* virtual */ string TestService::Information() const
{
    // No additional info to report.
    return ((_T("The purpose of this plugin is provide ability to execute functional tests.")));
}

static Core::ProxyPoolType<Web::TextBody> _testServiceMetadata(2);

/* virtual */ void TestService::Inbound(Web::Request& request)
{
    if (request.Verb == Web::Request::HTTP_POST)
    {
        request.Body(_testServiceMetadata.Element());
    }
}

/* virtual */ Core::ProxyType<Web::Response> TestService::Process(const Web::Request& request)
{
    ASSERT(_skipURL <= request.Path.length());
    Core::ProxyType<Web::Response> result(PluginHost::Factories::Instance().Response());

    if (_testUtilityImp != nullptr)
    {
        Core::ProxyType<Web::TextBody> body(_testServiceMetadata.Element());
        string requestBody = EMPTY_STRING;

        if ((request.Verb == Web::Request::HTTP_POST) && (request.HasBody()))
        {
            requestBody = (*request.Body<Web::TextBody>());
        }

        (*body) = HandleRequest(request.Path, _skipURL, requestBody);
        if((*body) != EMPTY_STRING)
        {
            result->Body<Web::TextBody>(body);
            result->ErrorCode = Web::STATUS_OK;
            result->Message = (_T("OK"));
            result->ContentType = Web::MIMETypes::MIME_JSON;
        }
        else
        {
            result->ErrorCode = Web::STATUS_BAD_REQUEST;
            result->Message = (_T("Method is not supported"));
        }
    }
    else
    {
        result->ErrorCode = Web::STATUS_METHOD_NOT_ALLOWED;
        result->Message = (_T("Test controller does not exist"));
    }
    return result;
}

void TestService::ProcessTermination(uint32_t pid)
{
    RPC::IRemoteProcess* process(_service->RemoteProcess(pid));
    if (process != nullptr)
    {
        process->Terminate();
        process->Release();
    }
}

void TestService::Activated(RPC::IRemoteProcess* /*process*/)
{
    return;
}

void TestService::Deactivated(RPC::IRemoteProcess* process)
{
    if (_pid == process->Id())
    {
        ASSERT(_service != nullptr);
        PluginHost::WorkerPool::Instance().Submit(PluginHost::IShell::Job::Create(_service, PluginHost::IShell::DEACTIVATED, PluginHost::IShell::FAILURE));
    }
}

string /*JSON*/ TestService::TestCommands(void)
{
    string response;
    Metadata testCommands;

    Exchange::ITestUtility::ICommand::IIterator* supportedCommands = _testUtilityImp->Commands();

    while (supportedCommands->Next())
    {
        Core::JSON::String name;
        name = supportedCommands->Command()->Name();
        testCommands.TestCommands.Add(name);
    }
    testCommands.ToString(response);

    return response;
}

string /*JSON*/ TestService::HandleRequest(const string& path, const uint8_t skipUrl, const string& body /*JSON*/)
{
    bool executed = false;
    // Return empty result in case of issue
    string /*JSON*/ response = EMPTY_STRING;

    Core::TextSegmentIterator index(Core::TextFragment(path, skipUrl, path.length() - skipUrl), false, '/');

    index.Next();
    index.Next();
    // Here process request other than:
    // /Service/<CALLSIGN>/TestCommands
    // /Service/<CALLSIGN>/<TEST_COMMAND_NAME>/...

    if ((index.Current().Text() == _T("TestCommands")) && (!index.Next()))
    {
        response = TestCommands();
        executed = true;
    }
    else
    {
        Exchange::ITestUtility::ICommand::IIterator* supportedCommands = _testUtilityImp->Commands();
        string testCommand = index.Current().Text();

        while (supportedCommands->Next())
        {
            if (supportedCommands->Command()->Name() == testCommand)
            {
                // Found test command
                if (!index.Next())
                {
                    // Execute test command
                    response = supportedCommands->Command()->Execute(body);
                    executed = true;
                }
                else
                {
                    //index.Next();
                    if ((index.Current().Text() == _T("Description")) && (!index.Next()))
                    {
                        response = supportedCommands->Command()->Description();
                        executed = true;
                    }
                    else if ((index.Current().Text() == _T("Parameters")) && (!index.Next()))
                    {
                        response = supportedCommands->Command()->Signature();
                        executed = true;
                    }
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
} // namespace Plugin
} // namespace WPEFramework
