#include <version_mega.h>
#include "oscarSecureService.h"
#include <logging.h>
#include <colorOutput.h>
#include "mutexInspector.h"
#include <Events/System/Net/oscar/PacketOnAcceptor.h>
#include <Events/System/Net/oscar/PacketOnConnector.h>
#include <Events/System/Net/oscar/Connected.h>
#include <Events/System/Net/oscar/NotifyOutBufferEmpty.h>
#include <Events/System/Net/oscar/NotifyBindAddress.h>
#include <Events/System/Net/socket/AddToConnectTCP.h>
#include <Events/System/Net/socket/AddToListenTCP.h>
#include <Events/Tools/webHandler/RegisterHandler.h>
#include <Events/System/Net/oscar/ConnectFailed.h>
#include <Events/System/Net/oscar/Disaccepted.h>
#include <Events/System/Net/oscar/Disconnected.h>
#include "bufferVerify.h"
#include "events_OscarSecure.hpp"


OscarSecure::Service::~Service()
{
    __m_users->clear();
}
OscarSecure::Service::Service(const SERVICE_id &svs, const std::string&  nm,IInstance* ifa)
    : UnknownBase(nm),
      ListenerBuffered1Thread(this,nm,ifa->getConfig(),svs,ifa),
      Broadcaster(ifa),
      RSA_keysize(256),
      m_maxPacketSize(32*1024*1024),
      __m_users(new __users),
      ssl((I_ssl*)iUtils->queryIface(Ifaces::SSL))
{

    try
    {
        XTRY;
        m_maxPacketSize= static_cast<size_t>(ifa->getConfig()->get_int64_t("maxPacketSize",
                                             32 * 1024 * 1024, ""));
        RSA_keysize=ifa->getConfig()->get_int64_t("RSA_keysize",256,"");
        XPASS;
    }
    catch (std::exception &e)
    {
        logErr2("exception: %s %s %d",e.what(),__FILE__,__LINE__);
        throw;
    }
}

bool OscarSecure::Service::on_StreamRead(const socketEvent::StreamRead* evt)
{
    MUTEX_INSPECTOR;
    try
    {
        while (1)
        {
            MUTEX_INSPECTOR;
            std::string req;
            bool recvd=false;
            bool success=false;
            unsigned char start_byte;
            std::string disconnectReason="unknown";
            bool need_disconnect=false;
            {
                MUTEX_INSPECTOR;
                {
                    XTRY;
                    {
                        M_LOCK(evt->esi->m_inBuffer);
                        if(evt->esi->m_inBuffer._mx_data.size()==0)
                        {
                            return true;
                        }
                        inBuffer b(evt->esi->m_inBuffer._mx_data.data(), evt->esi->m_inBuffer._mx_data.size());

                        start_byte=b.get_8_nothrow(success);

                        if (!success)
                        {
                            return true;
                        }
                        {
                            XTRY;

                            size_t len= static_cast<size_t>(b.get_PN_nothrow(success));

                            if (!success)
                            {
                                return true;
                            }

                            if(b.remains()<len)
                            {
                                return true;
                            }
                            else if(b.remains()==len)
                            {
                            }
                            else if(b.remains()>len)
                            {
                            }

                            if (len>m_maxPacketSize)
                            {
                                need_disconnect=true;
                                disconnectReason="oscar packet len>max_packet_size len="+std::to_string((int64_t)len)+" max_packet_size="+std::to_string((int64_t)m_maxPacketSize);
                                logErr2("disconnectReason %s",disconnectReason.c_str());
                            }
                            else
                            {
                                XTRY;
                                b.unpack_nothrow(req,len,success);
                                if (!success)
                                {
                                    logErr2("---------_ERROR if (!success) %s %d",__FILE__,__LINE__);
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
                evt->esi->close((std::string)"oscar: buffer broken. "+disconnectReason);
                return true;
                XPASS;
            }
            if(!recvd) return true;
            if (recvd)
            {
                XTRY;
                switch(start_byte)
                {
                case OscarSecure::SB_SINGLEPACKET_AES:
                {
                    MUTEX_INSPECTOR;
                    REF_getter<User> u=__m_users->find_throw(evt->esi->m_id);
                    REF_getter<refbuffer> buf=u->aes.decrypt(toRef(req));
                    if(u->esi->m_streamType==epoll_socket_info::STREAMTYPE_ACCEPTED)
                    {
                        passEvent(new oscarEvent::PacketOnAcceptor(evt->esi,buf,evt->route));

                    }
                    else
                    {
                        passEvent(new oscarEvent::PacketOnConnector(evt->esi,buf,evt->route));
                    }
                    continue;
                }
                break;
                case SB_SET_AES_KEY_C2S:
                {
                    MUTEX_INSPECTOR;
                    inBuffer b(req.data(),req.size());
                    REF_getter<User> u=__m_users->findOrCreateAndFind(evt->esi);
                    if(u.valid())
                    {
                        u->aes.init(u->rsa.privateDecrypt(b.get_PSTR()));

                        {
                            if(u->esi->m_streamType!=epoll_socket_info::STREAMTYPE_ACCEPTED)
                                throw CommonError("!isServer");
                            outBuffer o;
                            sendPacketPlain(SB_HELLO_SSL2_S2C,evt->esi,o);
                            u->inited=true;

                        }

                        continue;
                    }
                    else
                    {
                        logErr2("SB_SET_AES_KEY invalid user");
                    }
                }
                break;
                case OscarSecure::SB_HELLO_SSL2_S2C:
                {
                    MUTEX_INSPECTOR;
                    REF_getter<User> u=__m_users->findOrCreateAndFind(evt->esi);
                    if(u.valid())
                    {
                        u->inited=true;
                        passEvent(new oscarEvent::Connected(evt->esi,evt->route));
                    }
                    else
                    {
                        evt->esi->close("SB_HELLO_SSL2: invalied user");
                    }
                }
                break;
                case OscarSecure::SB_HELLO_SSL_S2C:
                {
                    MUTEX_INSPECTOR;
                    XTRY;
                    std::string pubkey;
                    inBuffer b(req.data(),req.size());
                    b>>pubkey;
                    int64_t version=b.get_PN();
                    if(version>>8!=COREVERSION>>8)
                    {
                        logErr2("invalid version");
                        evt->esi->close("oscarSecure: invalid peer version");
                        return true;
                    }
                    REF_getter<User> u=__m_users->findOrCreateAndFind(evt->esi);
                    if(!u.valid())
                    {
                        logErr2(RED("SB_HELLO_SSL invalied user %d"),CONTAINER(evt->esi->m_id));
                    }

                    if(u.valid())
                    {
                        XTRY;
                        XTRY;
                        u->rsa.initFromPublicKey(pubkey);
                        XPASS;
                        std::string aes_key;
                        XTRY;
                        aes_key=u->aes.generateRandomKey();
                        u->aes.init(aes_key);
                        XPASS;
                        outBuffer out;
                        XTRY;
                        out<<u->rsa.publicEncrypt(aes_key);


                        sendPacketPlain(SB_SET_AES_KEY_C2S,evt->esi,out);
                        XPASS;
                        XPASS;
                    }
                    XPASS;
                }
                break;
                default:
                    logErr2("Invalid start byte %d",start_byte);
                    evt->esi->close("oscar: Invalid start byte");

                    return true;
                }
                XPASS;
            }
            break;
        }
    }
    catch (std::exception &e)
    {
        logErr2("exception: %s (%s %d)",e.what(),__FILE__,__LINE__);
        evt->esi->close(e.what());
    }
    catch (...)
    {
        logErr2("exception: unknown (%s %d)",__FILE__,__LINE__);
        evt->esi->close("exception: unknown ");
    }
    return true;

}
bool OscarSecure::Service::on_NotifyOutBufferEmpty(const socketEvent::NotifyOutBufferEmpty* e)
{
    MUTEX_INSPECTOR;
    passEvent(new oscarEvent::NotifyOutBufferEmpty(e->esi,e->route));
    return true;
}


bool OscarSecure::Service::on_NotifyBindAddress(const socketEvent::NotifyBindAddress *e)
{
    MUTEX_INSPECTOR;
    passEvent(new oscarEvent::NotifyBindAddress(e->esi,e->socketDescription,e->rebind,e->route));
    return true;
}


UnknownBase* OscarSecure::Service::construct(const SERVICE_id& id, const std::string&  nm,IInstance* ifa)
{
    XTRY;
    return new Service(id,nm,ifa);
    XPASS;
}

void OscarSecure::Service::sendPacketPlain(const OscarSecure::StartByte& startByte, const REF_getter<epoll_socket_info>& esi, const outBuffer &o)
{
    MUTEX_INSPECTOR;
    XTRY;
    outBuffer O2;
    O2.put_8(startByte);
    O2<<o.asString();
    esi->write_(O2.asString()->asString());
    XPASS;
}
void OscarSecure::Service::sendPacketPlain(const OscarSecure::StartByte& startByte, const REF_getter<epoll_socket_info>& esi, const REF_getter<refbuffer> &o)
{
    MUTEX_INSPECTOR;
    XTRY;
    outBuffer O2;
    O2.put_8(startByte);
    O2<<o;
    esi->write_(O2.asString()->asString());
    XPASS;
}

bool OscarSecure::Service::on_SendPacket(const oscarEvent::SendPacket* e)
{

    MUTEX_INSPECTOR;

    if(CONTAINER(e->socketId)==-1)
    {
        return true;
    }
    REF_getter<User> u=__m_users->find_throw(e->socketId);

    if(u.valid())
    {
        {
            if(u->inited)
            {
                sendPacketPlain(OscarSecure::SB_SINGLEPACKET_AES,u->esi,u->aes.encrypt(e->buf));
            }
            else
            {
                throw CommonError("!if(u->inited) %s %d",__FILE__,__LINE__);

            }
        }
    }
    else
    {
        throw CommonError("!if(u.valid()) %s %d",__FILE__,__LINE__);
    }
    return true;
}
bool OscarSecure::Service::on_Connect(const oscarEvent::Connect*e)
{

    S_LOG("on_Connect");

#ifndef __MOBILE__
#endif
    sendEvent(ServiceEnum::Socket,new socketEvent::AddToConnectTCP(e->socketId,e->addr,e->socketDescription,bufferVerify,e->route));
    return true;
}
bool OscarSecure::Service::on_AddToListenTCP(const oscarEvent::AddToListenTCP*e)
{

    MUTEX_INSPECTOR;

    sendEvent(ServiceEnum::Socket,new socketEvent::AddToListenTCP(e->socketId,e->addr,e->socketDescription,false,bufferVerify, e->route));
    return true;
}
bool OscarSecure::Service::on_Accepted(const socketEvent::Accepted*e)
{
    MUTEX_INSPECTOR;
    XTRY;
    REF_getter<User> u=new User(e->esi);
    u->rsa.generate_key(RSA_keysize);
    outBuffer o;
    o<<u->rsa.getPublicKey();
    o<<COREVERSION;
    __m_users->insert(u);

    sendPacketPlain(SB_HELLO_SSL_S2C,e->esi,o);
    XPASS;
    return true;
}

bool OscarSecure::Service::on_Connected(const socketEvent::Connected*e)
{

#ifndef __MOBILE__
    DBG(logErr2("Service::on_Connected %s %d",__FILE__,__LINE__));
#endif
    __m_users->insert(new User(e->esi));
    return true;
}
bool OscarSecure::Service::on_startService(const systemEvent::startService* )
{
    if(iUtils->isServiceRegistered(ServiceEnum::WebHandler))
    {
        sendEvent(ServiceEnum::WebHandler, new webHandlerEvent::RegisterHandler("oscarSecure","OscarSecure",ListenerBase::serviceId));
    }

    return true;
}

void registerOscarSecureModule(const char* pn)
{
    if(pn)
    {
        iUtils->registerPlugingInfo(COREVERSION,pn,IUtils::PLUGIN_TYPE_SERVICE,ServiceEnum::OscarSecure,"OscarSecure",getEvents_OscarSecure());
    }
    else
    {
        iUtils->registerService(COREVERSION,ServiceEnum::OscarSecure,OscarSecure::Service::construct,"OscarSecure");
        regEvents_OscarSecure();
    }
}
bool OscarSecure::Service::handleEvent(const REF_getter<Event::Base>& e)
{
    MUTEX_INSPECTOR;
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

    if(systemEventEnum::startService==ID)
        return on_startService((const systemEvent::startService*)e.operator->());
    if(webHandlerEventEnum::RequestIncoming==ID)
        return on_RequestIncoming((const webHandlerEvent::RequestIncoming*)e.operator->());
    if( socketEventEnum::Disaccepted==ID)
        return on_Disaccepted((const socketEvent::Disaccepted*)e.operator->());
    if( socketEventEnum::Disconnected==ID)
        return on_Disconnected((const socketEvent::Disconnected*)e.operator->());

    XPASS;
    return false;

}
bool OscarSecure::Service::on_RequestIncoming(const webHandlerEvent::RequestIncoming* e)
{

    MUTEX_INSPECTOR;
    HTTP::Response cc;
    cc.content+="<h1>OscarSecure report</h1><p>";

    Json::Value v=jdump();
    Json::StyledWriter w;
    cc.content+="<pre>\n"+w.write(v)+"\n</pre>";

    cc.makeResponse(e->esi);
    return true;
}
bool OscarSecure::Service::on_ConnectFailed(const socketEvent::ConnectFailed*e)
{
    passEvent(new oscarEvent::ConnectFailed(e->esi,e->addr,e->route));
    return true;
}
bool OscarSecure::Service::on_Disaccepted(const socketEvent::Disaccepted*e)
{
    MUTEX_INSPECTOR;
    passEvent(new oscarEvent::Disaccepted(e->esi,e->reason,e->route));
    return true;
}
bool OscarSecure::Service::on_Disconnected(const socketEvent::Disconnected*e)
{
    MUTEX_INSPECTOR;
    passEvent(new oscarEvent::Disconnected(e->esi,e->reason,e->route));
    return true;
}
