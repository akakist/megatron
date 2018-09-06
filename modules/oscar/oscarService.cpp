#include "oscarService.h"


#include "epoll_socket_info.h"
#include "bufferVerify.h"
#include "tools_mt.h"
#include "version_mega.h"
#include "Events/System/Net/socket/AddToConnectTCP.h"
#include "Events/System/Net/socket/AddToListenTCP.h"
#include "events_oscar.hpp"


//using namespace Oscar;
Oscar::Service::Service(const SERVICE_id &svs, const std::string&  nm,IInstance* ifa):
    UnknownBase(nm),
    ListenerBuffered(this,nm,ifa->getConfig(),svs,ifa),
    Broadcaster(ifa),
    Logging(this,ERROR_log,ifa),
    m_maxPacketSize(32*1024*1024),__m_users(new __users)
{
    try
    {
        XTRY;
        m_maxPacketSize=ifa->getConfig()->get_int64_t("maxPacketSize",32*1024*1024,"");
        XPASS;
    }
    catch (std::exception &e)
    {
        logErr2("exception: %s %s %d",e.what(),__FILE__,__LINE__);
        throw;
    }

}

Oscar::Service::~Service()
{
    __m_users->clear();
}

bool Oscar::Service::on_Connect(const oscarEvent::Connect* e)
{
    logErr2("Oscar::Service AddToConnectTCP %s",e->addr.dump().c_str());
    sendEvent(ServiceEnum::Socket,new socketEvent::AddToConnectTCP(e->socketId,e->addr,e->socketDescription,bufferVerify,e->route));
    return true;
}
bool Oscar::Service::on_SendPacket(const oscarEvent::SendPacket* e)
{
    REF_getter<epoll_socket_info> esi=__m_users->getOrNull(e->socketId);


    if(esi.valid())
    {
        sendPacketPlain(Oscar::SB_SINGLEPACKET,esi,e->buf);
    }
    else
    {
        //route_t r=e->route;
        //r.pop_front();
        passEvent(new oscarEvent::SocketClosed(e->socketId,poppedFrontRoute(e->route)));
        logErr2("!esi valid %s %d",__FILE__,__LINE__);
    }
    return true;
}
bool Oscar::Service::on_AddToListenTCP(const oscarEvent::AddToListenTCP* e)
{
    sendEvent(ServiceEnum::Socket,new socketEvent::AddToListenTCP(e->socketId,e->addr,e->socketDescription,false,bufferVerify, e->route));
    return true;
}
bool Oscar::Service::on_Accepted(const socketEvent::Accepted*e)
{
    __m_users->user_insert(e->esi,true);

    outBuffer o;
    o<<"HELLO"<<COREVERSION;
    sendPacketPlain(SB_HELLO,e->esi,o);
    passEvent(new oscarEvent::Accepted(e->esi,e->route));
    return true;
}
bool Oscar::Service::on_StreamRead(const socketEvent::StreamRead* evt)
{
    try
    {
        while (1)
        {
            std::string req;
            bool recvd=false;
            bool success=false;
            unsigned char start_byte;
            bool need_disconnect=false;
            {
                {
                    XTRY;
                    {
                        M_LOCK(evt->esi->m_inBuffer);
                        inBuffer b(evt->esi->m_inBuffer._mx_data.data(), evt->esi->m_inBuffer._mx_data.size());

                        start_byte=b.get_8_nothrow(success);

                        if (!success) return true;
                        {
                            XTRY;
                            size_t len=b.get_PN_nothrow(success);
                            if (!success) return true;
                            if (len>m_maxPacketSize)
                            {
                                need_disconnect=true;
                            }
                            else
                            {
                                XTRY;
                                if(b.remains()<len)
                                {
                                    return true;
                                }
                                else if(b.remains()==len)
                                {
                                    DBG(log(TRACE_log,"b.remains()==len %d %d",b.remains(),len));
                                }
                                else if(b.remains()>len)
                                {
                                    DBG(log(TRACE_log,"b.remains()>len %d %d",b.remains(),len));
                                }
                                b.unpack_nothrow(req,len,success);
                                if (!success)
                                {
                                    log(ERROR_log,"---------_ERROR if (!success) %s %d",__FILE__,__LINE__);
                                    return true;
                                }
                                evt->esi->m_inBuffer._mx_data.erase(0,evt->esi->m_inBuffer._mx_data.size()-b.remains());
                                recvd=true;
                                XPASS;
                            }
                            XPASS;
                        }

                    }
                    XPASS;
                }
            }
            if(need_disconnect)
            {
                XTRY;
                evt->esi->close("oscar buffer broken");
                return true;
                XPASS;
            }
            if(!recvd) return true;
            if (recvd)
            {
                XTRY;
                switch(start_byte)
                {
                case Oscar::SB_SINGLEPACKET:
                {
                    REF_getter<epoll_socket_info> u=__m_users->getOrNull(evt->esi->m_id);
                    if(u.valid())
                    {
                        if(!__m_users->isServer(evt->esi->m_id))
                        {
                            passEvent(new oscarEvent::PacketOnConnector(evt->esi,toRef(req),evt->route));
                        }
                        else
                        {
                            passEvent(new oscarEvent::PacketOnAcceptor(evt->esi,toRef(req),evt->route));
                        }
                    }
                    continue;
                }
                break;
                case Oscar::SB_HELLO:
                {
                    inBuffer in(req);
                    std::string hello=in.get_PSTR();
                    if(hello!="HELLO")
                        throw CommonError("!HELLO");
                    int64_t ver=in.get_PN();
                    if(ver>>8!=COREVERSION>>8)
                    {
                        if(COREVERSION>ver)
                            sendEvent(ServiceEnum::ErrorDispatcher,new errorDispatcherEvent::SendMessage("ED_VERSION_WRONG","Packet version wrong for peer. You running newest version, please wait"));
                        else
                        {
                            sendEvent(ServiceEnum::ErrorDispatcher,new errorDispatcherEvent::SendMessage("ED_VERSION_WRONG","Packet version wrong for peer in oscar. Your need upgrade software"));
                        }

                        evt->esi->close("oscar: invalid peer version");
                        return true;
                    }
                }
                break;
                default:
                    log(ERROR_log,"Invalid start byte %d",start_byte);
                    evt->esi->close("oscar: invalid start byte");

                    return true;
                }
                XPASS;
            }
            break;
        }
    }
    catch (std::exception &e)
    {
        log(ERROR_log,"exception: %s (%s %d)",e.what(),__FILE__,__LINE__);
        evt->esi->close(e.what());
    }
    catch (...)
    {
        log(ERROR_log,"exception: unknown (%s %d)",__FILE__,__LINE__);
        evt->esi->close("exeption ...");
    }
    return true;
}
bool Oscar::Service::on_Connected(const socketEvent::Connected* e)
{
    __m_users->user_insert(e->esi,false);

    passEvent(new oscarEvent::Connected(e->esi,e->route));
    return true;
}
UnknownBase* Oscar::Service::construct(const SERVICE_id& id, const std::string&  nm,IInstance* ifa)
{
    XTRY;
    return new Service(id,nm,ifa);
    XPASS;
}
bool Oscar::Service::on_NotifyOutBufferEmpty(const socketEvent::NotifyOutBufferEmpty* e)
{
    passEvent(new oscarEvent::NotifyOutBufferEmpty(e->esi,e->route));
    return true;
}
bool Oscar::Service::on_NotifyBindAddress(const socketEvent::NotifyBindAddress*e)
{
    passEvent(new oscarEvent::NotifyBindAddress(e->esi,e->socketDescription,e->rebind,e->route));
    return true;
}
void Oscar::Service::sendPacketPlain(const Oscar::StartByte& startByte, const REF_getter<epoll_socket_info>& esi, const outBuffer &o)
{
    XTRY;
    outBuffer O2;
    O2.put_8(startByte);
    O2<<o.asString();
    esi->write_(O2.asString()->asString());
    XPASS;
}
void Oscar::Service::sendPacketPlain(const Oscar::StartByte& startByte, const REF_getter<epoll_socket_info>& esi, const REF_getter<refbuffer> &o)
{
    XTRY;
    outBuffer O2;
    O2.put_8(startByte);
    O2<<o;
    esi->write_(O2.asString()->asString());
    XPASS;
}
bool Oscar::Service::on_startService(const systemEvent::startService*)
{
    return true;
}

void registerOscarModule(const char* pn)
{
    if(pn)
    {
        iUtils->registerPlugingInfo(COREVERSION,pn,IUtils::PLUGIN_TYPE_SERVICE,ServiceEnum::Oscar,"Oscar");
    }
    else
    {
        iUtils->registerService(COREVERSION,ServiceEnum::Oscar,Oscar::Service::construct,"Oscar");
        regEvents_oscar();
    }

}
void Oscar::__users::user_insert(const REF_getter<epoll_socket_info>& esi, const bool &isServer)
{
    
    add(ServiceEnum::Oscar,esi);
    {
        M_LOCK(m_lock);
        if(m_isServers.count(esi->m_id)) throw CommonError("if(m_isServers.count(esi->m_id)) "+_DMI());
        m_isServers[esi->m_id]=isServer;
    }
}
Json::Value Oscar::__users::jdump()
{
    Json::Value v;
    v["SocketsContainerBase"]=SocketsContainerBase::jdump();
    std::map<SOCKET_id,bool > m;
    {
        M_LOCK(m_lock);
        m=m_isServers;
    }
    for(std::map<SOCKET_id,bool >::iterator i=m.begin(); i!=m.end(); i++)
    {
        v["isServer"][iUtils->toString(CONTAINER(i->first))]=i->second;
    }
    return v;
}

void Oscar::__users::on_delete(const REF_getter<epoll_socket_info>&esi, const std::string& )
{
    M_LOCK(m_lock);
    if(!m_isServers.count(esi->m_id)) throw CommonError("if(m_isServers.count(esi->m_id)) "+_DMI());
    m_isServers.erase(esi->m_id);
}
void Oscar::__users::on_mod_write(const REF_getter<epoll_socket_info>&)
{

}

bool Oscar::__users::isServer(const SOCKET_id &sid)
{
    M_LOCK(m_lock);
    if(!m_isServers.count(sid)) throw CommonError("if(m_isServers.count(esi->m_id)) %s %d",__FILE__,__LINE__);
    return m_isServers[sid];

}


bool Oscar::Service::handleEvent(const REF_getter<Event::Base>& e)
{
    XTRY;
    auto &ID=e->id;
    if( socketEventEnum::Accepted==ID)
        return on_Accepted((const socketEvent::Accepted*)e.operator->());
    if( socketEventEnum::StreamRead==ID)
        return on_StreamRead((const socketEvent::StreamRead*)e.operator->());
    if( socketEventEnum::Connected==ID)
        return on_Connected((const socketEvent::Connected*)e.operator->());
    if( socketEventEnum::NotifyBindAddress==ID)
        return on_NotifyBindAddress((const socketEvent::NotifyBindAddress*)e.operator->());
    if( socketEventEnum::NotifyOutBufferEmpty==ID)
        return on_NotifyOutBufferEmpty((const socketEvent::NotifyOutBufferEmpty*)e.operator->());
    if( socketEventEnum::ConnectFailed==ID)
        return on_ConnectFailed((const socketEvent::ConnectFailed*)e.operator->());
    if( oscarEventEnum::SendPacket==ID)
        return(this->on_SendPacket((const oscarEvent::SendPacket*)e.operator->()));

    if( oscarEventEnum::AddToListenTCP==ID)
        return(this->on_AddToListenTCP((const oscarEvent::AddToListenTCP*)e.operator->()));

    if( oscarEventEnum::Connect==ID)
        return(this->on_Connect((const oscarEvent::Connect*)e.operator->()));

    if(systemEventEnum::startService==ID)
        return on_startService((const systemEvent::startService*)e.operator->());

    XPASS;
    return false;

}
