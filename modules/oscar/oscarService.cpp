#include <unistd.h>
#include <version_mega.h>
#include "oscarService.h"

#include "event_mt.h"
#include <Events/System/Net/socketEvent.h>
#include <Events/System/Net/oscarEvent.h>
#include <mutexInspector.h>
#include "events_oscar.hpp"

Oscar::Service::Service(const SERVICE_id &svs, const std::string&  nm,IInstance* ifa):
    UnknownBase(nm),
    ListenerSimple(nm,svs),
    Broadcaster(ifa),
    iInstance(ifa)
{
    try
    {
        XTRY;
        XPASS;
    }
    catch (const std::exception &e)
    {
        logErr2("exception: %s %s %d",e.what(),__FILE__,__LINE__);
        throw;
    }

}

Oscar::Service::~Service()
{
}

bool Oscar::Service::on_Connect(const oscarEvent::Connect* e)
{
    MUTEX_INSPECTOR;
    sendEvent(socketListener,new socketEvent::AddToConnectTCP(e->socketId,e->addr,e->socketDescription,e->route));
    return true;
}
bool Oscar::Service::on_SendPacket(const oscarEvent::SendPacket* e)
{
    MUTEX_INSPECTOR;
    sendPacketPlain(Oscar::SB_SINGLEPACKET,e->esi,e->buf);
    return true;
}
bool Oscar::Service::on_AddToListenTCP(const oscarEvent::AddToListenTCP* e)
{
    MUTEX_INSPECTOR;
    sendEvent(socketListener,new socketEvent::AddToListenTCP(e->socketId,e->addr,e->socketDescription,false, e->route));
    return true;
}
bool Oscar::Service::on_Accepted(const socketEvent::Accepted*e)
{
    MUTEX_INSPECTOR;

//    outBuffer o;
//    o<<"HELLO"<<COREVERSION;
//    sendPacketPlain(SB_HELLO,e->esi,o);
    passEvent(new oscarEvent::Accepted(e->esi,e->route));
    return true;
}
bool Oscar::Service::on_StreamRead(const socketEvent::StreamRead* evt)
{

//    logErr2("@@ %s",__PRETTY_FUNCTION__);
    MUTEX_INSPECTOR;
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
                        W_LOCK(evt->esi->inBuffer_.lk);
                        if(evt->esi->inBuffer_._mx_data.size()==0)
                            return true;
                        inBuffer b(evt->esi->inBuffer_._mx_data.data(), evt->esi->inBuffer_._mx_data.size());

                        start_byte=b.get_8_nothrow(success);


                        if (!success) return true;
                        {
                            XTRY;
                            auto len= static_cast<size_t>(b.get_PN_nothrow(success));
                            if (!success) return true;
                            {
                                XTRY;
                                if(b.remains()<len)
                                {
                                    logErr2("if(b.remains()<len)");
                                    return true;
                                }

                                b.unpack_nothrow(req,len,success);

                                if (!success)
                                {
                                    logErr2("---------_ERROR if (!success) %s %d",__FILE__,__LINE__);
                                    return true;
                                }

                                if(b.remains()==0)
                                {
                                    evt->esi->inBuffer_._mx_data.clear();
//                                    return true;
                                }
                                else
                                {

                                    evt->esi->inBuffer_._mx_data.erase(0,evt->esi->inBuffer_._mx_data.size()-b.remains());
                                }



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
                logErr2("oscar buffer broken");
                return true;
                XPASS;
            }
            if(!recvd) return true;


            {
                XTRY;
                switch(start_byte)
                {
                case Oscar::SB_SINGLEPACKET:
                {

                    if(evt->esi->streamType_==epoll_socket_info::STREAMTYPE_CONNECTED)
                    {
                        passEvent(new oscarEvent::PacketOnConnector(evt->esi,toRef(req),evt->route));
                    }
                    else
                    {
                        passEvent(new oscarEvent::PacketOnAcceptor(evt->esi,toRef(req),evt->route));
                    }
                    continue;
                }
                break;
//                case Oscar::SB_HELLO:
//                {
//
//                    inBuffer in(req);
//                    std::string hello=in.get_PSTR();
//                    if(hello!="HELLO")
//                        throw CommonError("!HELLO");
//                    int64_t ver=in.get_PN();
//                    if(ver>>8 != COREVERSION>>8)
//                    {
//                        if(COREVERSION>ver)
//                            sendEvent(ServiceEnum::ErrorDispatcher,new errorDispatcherEvent::SendMessage("ED_VERSION_WRONG","Packet version wrong for peer. You running newest version, please wait"));
//                        else
//                        {
//                            sendEvent(ServiceEnum::ErrorDispatcher,new errorDispatcherEvent::SendMessage("ED_VERSION_WRONG","Packet version wrong for peer in oscar. Your need upgrade software"));
//                        }

//                        evt->esi->close("oscar: invalid peer version");
//                        return true;
//                    }
//                }
//                break;
                default:
                    logErr2("Invalid start byte %d",start_byte);
                    evt->esi->close("oscar: invalid start byte");

                    return true;
                }
                XPASS;
            }
            break;
        }
    }
    catch (const std::exception &e)
    {
        logErr2("exception: %s (%s %d)",e.what(),__FILE__,__LINE__);
        evt->esi->close(e.what());
    }
    catch (...)
    {
        logErr2("exception: unknown (%s %d)",__FILE__,__LINE__);
        evt->esi->close("exeption ...");
    }
    return true;
}
bool Oscar::Service::on_Connected(const socketEvent::Connected* e)
{
    MUTEX_INSPECTOR;
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
    MUTEX_INSPECTOR;
    passEvent(new oscarEvent::NotifyOutBufferEmpty(e->esi,e->route));
    return true;
}
bool Oscar::Service::on_NotifyBindAddress(const socketEvent::NotifyBindAddress*e)
{
    MUTEX_INSPECTOR;
    passEvent(new oscarEvent::NotifyBindAddress(e->addr,e->socketDescription,e->rebind,e->route));
    return true;
}
void Oscar::Service::sendPacketPlain(const Oscar::StartByte& startByte, const REF_getter<epoll_socket_info>& esi, const outBuffer &o)
{
    MUTEX_INSPECTOR;
    XTRY;
    outBuffer O2;
    O2.put_8(startByte);
    O2<<o.asString();
    esi->write_(O2.asString()->asString());
    XPASS;
}
void Oscar::Service::sendPacketPlain(const Oscar::StartByte& startByte, const REF_getter<epoll_socket_info>& esi, const REF_getter<refbuffer> &o)
{
    MUTEX_INSPECTOR;
    XTRY;
    outBuffer O2;
    O2.put_8(startByte);
    O2<<o;
    esi->write_(O2.asString()->asString());
    XPASS;
}
bool Oscar::Service::on_startService(const systemEvent::startService*)
{
    socketListener=dynamic_cast<ListenerBase*>(iInstance->getServiceOrCreate(ServiceEnum::Socket));
    return true;
}

void registerOscarModule(const char* pn)
{
    if(pn)
    {
        iUtils->registerPlugingInfo(COREVERSION,pn,IUtils::PLUGIN_TYPE_SERVICE,ServiceEnum::Oscar,"Oscar",getEvents_oscar());
    }
    else
    {
        iUtils->registerService(COREVERSION,ServiceEnum::Oscar,Oscar::Service::construct,"Oscar");
        regEvents_oscar();
    }

}


bool Oscar::Service::handleEvent(const REF_getter<Event::Base>& e)
{
    XTRY;
    auto &ID=e->id;
    if( socketEventEnum::Accepted==ID)
        return on_Accepted((const socketEvent::Accepted*)e.get());
    if( socketEventEnum::StreamRead==ID)
        return on_StreamRead((const socketEvent::StreamRead*)e.get());
    if( socketEventEnum::Connected==ID)
        return on_Connected((const socketEvent::Connected*)e.get());
    if( socketEventEnum::NotifyBindAddress==ID)
        return on_NotifyBindAddress((const socketEvent::NotifyBindAddress*)e.get());
    if( socketEventEnum::NotifyOutBufferEmpty==ID)
        return on_NotifyOutBufferEmpty((const socketEvent::NotifyOutBufferEmpty*)e.get());
    if( socketEventEnum::ConnectFailed==ID)
        return on_ConnectFailed((const socketEvent::ConnectFailed*)e.get());
    if( oscarEventEnum::SendPacket==ID)
        return(this->on_SendPacket((const oscarEvent::SendPacket*)e.get()));

    if( oscarEventEnum::AddToListenTCP==ID)
        return(this->on_AddToListenTCP((const oscarEvent::AddToListenTCP*)e.get()));

    if( oscarEventEnum::Connect==ID)
        return(this->on_Connect((const oscarEvent::Connect*)e.get()));

    if(systemEventEnum::startService==ID)
        return on_startService((const systemEvent::startService*)e.get());


    if( socketEventEnum::Disaccepted==ID)
        return on_Disaccepted((const socketEvent::Disaccepted*)e.get());
    if( socketEventEnum::Disconnected==ID)
        return on_Disconnected((const socketEvent::Disconnected*)e.get());

    XPASS;
    return false;

}
bool Oscar::Service::on_ConnectFailed(const socketEvent::ConnectFailed*e)
{
    passEvent(new oscarEvent::ConnectFailed(e->esi,e->addr,e->route));
    return true;
}
bool Oscar::Service::on_Disaccepted(const socketEvent::Disaccepted*e)
{
    passEvent(new oscarEvent::Disaccepted(e->esi,e->reason,e->route));
    return true;
}
bool Oscar::Service::on_Disconnected(const socketEvent::Disconnected*e)
{
    passEvent(new oscarEvent::Disconnected(e->esi,e->reason,e->route));
    return true;
}
