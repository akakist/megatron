#ifndef ___________PP__SERVER__H
#define ___________PP__SERVER__H


#include <unknown.h>
#include <listenerBuffered1Thread.h>
#include <broadcaster.h>
#include "event_mt.h"
#include <Events/System/Net/oscarEvent.h>
#include <Events/System/Net/socketEvent.h>
#include <Events/Tools/webHandlerEvent.h>
#include <Events/System/Run/startServiceEvent.h>
#include "oscarSecureUser.h"
#include "st_rsa.h"
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

struct OscarSecureData:public Refcountable
{
    st_rsa rsa;
    st_AES aes;
};
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
        public UnknownBase,
        public ListenerBuffered1Thread,
        public Broadcaster
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
        bool on_ConnectFailed(const socketEvent::ConnectFailed*e);
        bool on_Disaccepted(const socketEvent::Disaccepted*e);
        bool on_Disconnected(const socketEvent::Disconnected*e);

        bool on_RequestIncoming(const webHandlerEvent::RequestIncoming*);


        bool on_startService(const systemEvent::startService*);
        bool handleEvent(const REF_getter<Event::Base>& e);

    protected:
        void sendPacketPlain(const OscarSecure::StartByte& startByte, const REF_getter<epoll_socket_info>& esi, const outBuffer &o);
        void sendPacketPlain(const OscarSecure::StartByte& startByte, const REF_getter<epoll_socket_info>& esi, const REF_getter<refbuffer> &o);

    public:
        Json::Value jdump()
        {
            Json::Value v;
            v["RSA_keysize"]=(int)RSA_keysize;
            v["max_packet_size"]=std::to_string((int64_t)m_maxPacketSize);
            return v;
        }
    public:
        void deinit()
        {
            ListenerBuffered1Thread::deinit();
        }

        static UnknownBase* construct(const SERVICE_id& id, const std::string&  nm,IInstance* ifa);
        ~Service();
        I_ssl * const ssl;
        IInstance* iInstance;
        OscarSecureData* getData(epoll_socket_info* esi);

    };
}


#endif
