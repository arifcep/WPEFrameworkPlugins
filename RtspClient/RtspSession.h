#ifndef RTSPSESSION_H
#define RTSPSESSION_H

#include <sys/socket.h>
#include <sys/un.h>
#include <linux/netlink.h>

#include <core/NodeId.h>
#include <core/SocketPort.h>
#include <core/Queue.h>
#include <core/Timer.h>

#include "RtspCommon.h"
#include "RtspParser.h"


namespace WPEFramework {
namespace Plugin {

typedef Core::QueueType<RtspMessagePtr> RequestQueue;
typedef Core::QueueType<RtspMessagePtr> ResponseQueue;


class RtspSession
{
    public:
        class Socket : public Core::SocketStream
        {
            public:
            Socket(const Core::NodeId &local, const Core::NodeId &remote, RtspSession& rtspSession);
            virtual ~Socket();
            uint16_t SendData(uint8_t* dataFrame, const uint16_t maxSendSize);
            uint16_t ReceiveData(uint8_t* dataFrame, const uint16_t receivedSize);
            void StateChange();

            private:
                RtspSession& _rtspSession;
       };

        class AnnouncementHandler {
            public:
            virtual void announce(const RtspAnnounce& announcement) = 0;
        };

        class HeartbeatTimer {
        public:
            HeartbeatTimer(RtspSession& parent)
                : _parent(&parent)
            {
            }
            HeartbeatTimer(const HeartbeatTimer& copy)
                : _parent(copy._parent)
            {
            }
            ~HeartbeatTimer()
            {
            }

            HeartbeatTimer& operator=(const HeartbeatTimer& RHS)
            {
                _parent = RHS._parent;
                return (*this);
            }

        public:
            uint64_t Timed(const uint64_t scheduledTime)
            {
                ASSERT(_parent != nullptr);
                return (_parent->Timed(scheduledTime));
            }

        private:
            RtspSession* _parent;
        };

    public:
        RtspSession(RtspSession::AnnouncementHandler& handler);
        ~RtspSession();
        RtspReturnCode Initialize(const string& hostname, uint16_t port);
        RtspReturnCode Terminate();
        RtspReturnCode Open(const string assetId, uint32_t position = 0, const string &reqCpeId = "", const string &remoteIp = "");
        RtspReturnCode Close();
        RtspReturnCode Play(float scale, uint32_t position = 0);
        RtspReturnCode Get(const string name, string &value) const;
        RtspReturnCode Set(const string& name, const string& value);

        RtspReturnCode Send(const RtspMessagePtr& request);
        RtspReturnCode SendHeartbeat(bool bSRM);
        RtspReturnCode SendHeartbeats();

        RtspReturnCode ProcessResponse(const string &response, bool bSRM);
        RtspReturnCode ProcessAnnouncement(const std::string &response, bool bSRM);
        RtspReturnCode SendResponse(int respSeq, bool bSRM);
        RtspReturnCode SendAnnouncement(int code, const string &reason);

        uint64_t Timed(const uint64_t scheduledTime);

    private:
        inline  RtspSession::Socket& GetSocket(bool bSRM)    {
            return (bSRM || _sessionInfo.bSrmIsRtspProxy) ? *_srmSocket : *_controlSocket;
        }

        inline bool IsSrmRtspProxy() {
            return _sessionInfo.bSrmIsRtspProxy;
        }

    private:
        static constexpr uint16_t ResponseWaitTime = 3000;
        static constexpr uint16_t NptUpdateInterwal = 1000;

        Core::NodeId _remote;
        Core::NodeId _local;
        RtspSession::Socket *_srmSocket;
        RtspSession::Socket *_controlSocket;
        RtspSession::AnnouncementHandler& _announcementHandler;
        RtspParser _parser;
        RtspSessionInfo _sessionInfo;
        Core::CriticalSection _adminLock;
        RequestQueue _requestQueue;
        ResponseQueue _responseQueue;
        Core::TimerType<HeartbeatTimer> _heartbeatTimer;

        bool _isSessionActive;
        int _nextSRMHeartbeatMS;
        int _nextPumpHeartbeatMS;
        int _playDelay;
};

}} // WPEFramework::Plugin

#endif
