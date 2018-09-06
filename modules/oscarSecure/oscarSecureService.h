#ifndef ___________PP__SERVER__H
#define ___________PP__SERVER__H
#include "listenerBuffered.h"
#include "listenerBuffered1Thread.h"
#include "broadcaster.h"
#include "REF.h"
#include "SOCKET_id.h"


#include "ioBuffer.h"

#include "st_rsa.h"

#include "Events/System/Run/startService.h"


#include "version_mega.h"
#include "oscarSecureUser.h"
#include "ISSL.h"
#include "logging.h"
#include "Events/System/Net/socket/Accepted.h"
#include "Events/System/Net/socket/StreamRead.h"
#include "Events/System/Net/socket/Connected.h"
#include "Events/System/Net/socket/NotifyBindAddress.h"
#include "Events/System/Net/socket/NotifyOutBufferEmpty.h"
#include "Events/System/Net/socket/ConnectFailed.h"

#include "Events/System/Net/oscar/Accepted.h"
#include "Events/System/Net/oscar/AddToListenTCP.h"
#include "Events/System/Net/oscar/Connect.h"
#include "Events/System/Net/oscar/Connected.h"
#include "Events/System/Net/oscar/ConnectFailed.h"
#include "Events/System/Net/oscar/NotifyBindAddress.h"
#include "Events/System/Net/oscar/NotifyOutBufferEmpty.h"
#include "Events/System/Net/oscar/PacketOnAcceptor.h"
#include "Events/System/Net/oscar/PacketOnConnector.h"
#include "Events/System/Net/oscar/SendPacket.h"
#include "Events/System/Net/oscar/SocketClosed.h"
#include "Events/System/timer/TickTimer.h"
#include "Events/System/timer/TickTimer.h"
#include "Events/Tools/webHandler/RegisterDirectory.h"
#include "Events/Tools/webHandler/RegisterHandler.h"
#include "Events/Tools/webHandler/RequestIncoming.h"


namespace OscarSecure
{
    enum StartByte
    {
        SB_SINGLEPACKET_AES	=0x11,
        SB_SET_AES_KEY_C2S  =0x14,
        SB_HELLO_SSL_S2C 	=0x15,
        SB_HELLO_SSL2_S2C 	=0x16,
//        SB_AUTH_FAILED_S2C      =0x17,
    };
    namespace Packet
    {
    }
}

namespace OscarSecure
{

    struct TimeEvent
    {
        enum
        {
            ReportStatTimer,
        };

    };
    class Service:
        private UnknownBase,
        private ListenerBuffered1Thread,
        private Broadcaster,
        public Logging
    {

        int64_t RSA_keysize;
        size_t m_maxPacketSize;
        Service(const SERVICE_id &svs, const std::string&  nm,IInstance* ifa);
        bool on_SendPacket(const oscarEvent::SendPacket*);
        bool on_Connect(const oscarEvent::Connect*);
        bool on_AddToListenTCP(const oscarEvent::AddToListenTCP*);

        bool on_Accepted(const socketEvent::Accepted*);
        bool on_StreamRead(const socketEvent::StreamRead*);
        bool on_Connected(const socketEvent::Connected*);
        bool on_NotifyBindAddress(const socketEvent::NotifyBindAddress*);
        bool on_NotifyOutBufferEmpty(const socketEvent::NotifyOutBufferEmpty* e);
        bool on_ConnectFailed(const socketEvent::ConnectFailed*e)
        {
            passEvent(new oscarEvent::ConnectFailed(e->addr,e->route));
            return true;
        }

        bool on_RequestIncoming(const webHandlerEvent::RequestIncoming*);


        bool on_startService(const systemEvent::startService*);
        bool handleEvent(const REF_getter<Event::Base>& e);

        //void setAuth(const std::string &abuf);
    protected:
        void sendPacketPlain(const OscarSecure::StartByte& startByte, const REF_getter<epoll_socket_info>& esi, const outBuffer &o);
        void sendPacketPlain(const OscarSecure::StartByte& startByte, const REF_getter<epoll_socket_info>& esi, const REF_getter<refbuffer> &o);

        //void processRequest(const SOCKET_id&  socketId, const std::string& buf,const route_t & route);
        //bool on_SOCKET_READ(const socketEvent::StreamRead* evt);
    public:
        REF_getter<__users> __m_users;
        Json::Value jdump()
        {
            Json::Value v;
            v["users"]=__m_users->jdump();
            v["RSA_keysize"]=(int)RSA_keysize;
            v["max_packet_size"]=iUtils->toString((int64_t)m_maxPacketSize);
            return v;
        }
    public:
        static UnknownBase* construct(const SERVICE_id& id, const std::string&  nm,IInstance* ifa);
        ~Service();
        I_ssl * const ssl;

    };
}


#endif
