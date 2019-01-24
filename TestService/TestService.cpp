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
    //_testUtilityImp = Core::ServiceAdministrator::Instance().Instantiate<Exchange::ITestUtility>(Core::Library(), _T("TestUtilityImp"), static_cast<uint32_t>(~0));

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
    return ((_T("The purpose of [%s] plugin is proivde ability to execute functional tests."), _pluginName.c_str()));
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

        (*body) = Process(request.Path, _skipURL, requestBody);
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

string /*JSON*/ TestService::TestCommandsResponse(void)
{
    string response;
    Metadata testCommands;

    SYSLOG(Trace::Fatal, (_T("*** TestCommandsResponse 1 ***")));
    Exchange::ITestUtility::ICommand::IIterator* supportedCommands = _testUtilityImp->Commands();
    SYSLOG(Trace::Fatal, (_T("*** TestCommandsResponse 2 ***")));

    while (supportedCommands->Next())
    {
        Core::JSON::String name;
        SYSLOG(Trace::Fatal, (_T("*** TestCommandsResponse 3 ***")));
        name = supportedCommands->Command()->Name();
        SYSLOG(Trace::Fatal, (_T("*** TestCommandsResponse 4 ***")));
        testCommands.TestCommands.Add(name);
    }
    testCommands.ToString(response);
    SYSLOG(Trace::Fatal, (_T("*** TestCommandsResponse 5 ***")));

    return response;
}

string /*JSON*/ TestService::Process(const string& path, const uint8_t skipUrl, const string& body /*JSON*/)
{
    SYSLOG(Trace::Fatal, (_T("*** Process, path: %s***"), path.c_str()));
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
        SYSLOG(Trace::Fatal, (_T("*** Process, TestCommands***")));
        response = TestCommandsResponse();
        SYSLOG(Trace::Fatal, (_T("*** TestCommands %s ***"), response.c_str()));
        executed = true;
    }
    else
    {
        Exchange::ITestUtility::ICommand::IIterator* supportedCommands = _testUtilityImp->Commands();
         string testCommand = index.Current().Text();

        while (supportedCommands->Next())
        {
            SYSLOG(Trace::Fatal, (_T("*** supportedCommands->Command()->Name() %s***"), supportedCommands->Command()->Name().c_str()));
            SYSLOG(Trace::Fatal, (_T("*** testCommand %s***"), testCommand.c_str()));
            if (supportedCommands->Command()->Name() == testCommand)
            {
                // Found test command
                if (!index.Next())
                {
                    SYSLOG(Trace::Fatal, (_T("*** Test execution ***")));
                    // Execute test command
                    response = supportedCommands->Command()->Execute(body);
                    executed = true;
                }
                else
                {
                    //index.Next();
                    if ((index.Current().Text() == _T("Description")) && (!index.Next()))
                    {
                        SYSLOG(Trace::Fatal, (_T("*** Description execution ***")));
                        response = supportedCommands->Command()->Description();
                        executed = true;
                    }
                    else if ((index.Current().Text() == _T("Parameters")) && (!index.Next()))
                    {
                        SYSLOG(Trace::Fatal, (_T("*** Parameters execution ***")));
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
