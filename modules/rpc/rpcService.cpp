#define STATICLIB 1

#include "rpcService.h"

#include "version_mega.h"
//#include "event.h"
#include <Events/System/Net/rpc/Connected.h>
#include "Events/System/Net/rpc/IncomingOnAcceptor.h"
#include "Events/System/Net/rpc/IncomingOnConnector.h"
#include "Events/System/Net/rpc/ConnectFailed.h"
#include "Events/System/Net/rpc/Disconnected.h"
#include "Events/System/Net/rpc/Disaccepted.h"
#include "Events/System/Net/oscar/AddToListenTCP.h"
#include "Events/System/Net/oscar/Connect.h"
#include "Events/System/timer/TickAlarm.h"
#include "Events/System/timer/TickTimer.h"

#include "tools_mt.h"
#include "logging.h"
#include "pconfig.h"
#include "colorOutput.h"
#include "events_rpc.hpp"
RPC::Service::Service(const SERVICE_id &svs, const std::string&  nm, IInstance* ifa):
    UnknownBase(nm),
    ListenerSimple(nm,ifa->getConfig(),svs),
    Broadcaster(ifa),
    myOscar(ServiceEnum::Oscar),
    m_iterateTimeout(ifa->getConfig()->get_real("IterateTimeoutSec",60,"")),
    m_connectionActivityTimeout(ifa->getConfig()->get_real("ConnectionActivity",600.0,"")),
    sessions(new __sessions(ifa)),
    iInstance(ifa),
    m_isTerminating(false)
{

    try
    {

        XTRY;
        {
            std::string _main,_reserve;
#if defined(__ANDROID__) || defined(__IOS__) || defined(QT_CORE_LIB)
            _main="NONE";
#else
            _main="INADDR_ANY:0,INADDR6_ANY:0";
#endif
            _reserve="NONE";
            {
                M_LOCK(sharedAddr);
                m_bindAddr_main=ifa->getConfig()->get_tcpaddr("BindAddr_MAIN",_main,"Address used to work with dfs network. NONE - no bind");
                m_bindAddr_reserve=ifa->getConfig()->get_tcpaddr("BindAddr_RESERVE",_reserve,"Address used to communicate with local apps, must be fixed. NONE - no bind");
                if(m_bindAddr_main==m_bindAddr_reserve && m_bindAddr_main.size())
                {
                    throw CommonError("if(m_bindAddr==m_bindAddr2)");
                }
            }
        }

        XPASS;
    }
    catch (std::exception &e)
    {
        logErr2("exception: %s %s %d",e.what(),__FILE__,__LINE__);
        throw;
    }
}
RPC::Service::~Service()
{
    m_isTerminating=true;
    if(sharedAddr.m_internalAddr.size())
    {
        try {
        }
        catch(...)
        {

        }
    }
    //delete sessions;
    sessions->clear();

}

bool RPC::Service::on_Connected(const oscarEvent::Connected* E)
{
    MUTEX_INSPECTOR;

    REF_getter<Session> S(NULL);
    msockaddr_in addr;
    std::set<route_t> subscribers;
    {
        RLocker lk(sessions->m_lock);
        auto it=sessions->mx_sa2Session.find(E->esi->request_for_connect->addr);
        if(it==sessions->mx_sa2Session.end())
        {
            throw CommonError("2cannot find RPC session on connected %s",E->esi->request_for_connect->addr.dump().c_str());
            return true;
        }
        S=it->second;
        {
            subscribers=sessions->mx_subscribers;

        }
    }

    for(auto &i:subscribers)
    {
        route_t r=i;
        r.pop_front();
        passEvent(new rpcEvent::Connected(E->esi,addr,r));
    }

    if(!S.valid()) throw CommonError("!!!!! FATAL invalid S %s %d",__FILE__,__LINE__);

    S->esi=E->esi;
    doSend(S);
    return true;
}
bool RPC::Service::on_PacketOnAcceptor(const oscarEvent::PacketOnAcceptor*E)
{
    MUTEX_INSPECTOR;
    try {
        inBuffer buf(E->buf);
        int direction;
        buf>>direction;

        if(direction=='p')
        {
            MUTEX_INSPECTOR;
            REF_getter<Event::Base> e=iUtils->unpackEvent(buf);
            if(!e.valid())
            {
                return true;
            }
            REF_getter<Route> r=e->route.pop_front();
            if(r->type==Route::LOCALSERVICE)
            {
                LocalServiceRoute* l=(LocalServiceRoute*) r.operator ->();
                sendEvent(l->id,new rpcEvent::IncomingOnAcceptor(E->esi,e));
            }
            else
            {
                throw CommonError("!(r->type==Route::LOCALSERVICE) %s",_DMI().c_str());
            }
        }
        else if(direction=='s')
        {
            MUTEX_INSPECTOR;
            SERVICE_id dst;
            buf>>dst;
            REF_getter<Event::Base> e=iUtils->unpackEvent(buf);
            if(!e.valid())
            {
                return true;
            }

            e->route.push_front(new RemoteAddrRoute(E->esi->m_id));
            e->route.push_front(new LocalServiceRoute(dst));
            sendEvent(dst,new rpcEvent::IncomingOnAcceptor(E->esi,e));
        }
    }
    catch(std::exception &e)
    {
        DBG(logErr2("PKT RPC catched %s",e.what()));
        E->esi->close("broken packet in RPC");
    }

    return true;
}


bool RPC::Service::on_PacketOnConnector(const oscarEvent::PacketOnConnector* E)
{


    MUTEX_INSPECTOR;

    try {
        inBuffer buf(E->buf);
        int direction;
        buf>>direction;

        if(direction=='p')
        {
            MUTEX_INSPECTOR;
            REF_getter<Event::Base> e=iUtils->unpackEvent(buf);
            if(!e.valid())
            {
                return true;
            }

            REF_getter<Route> r=e->route.pop_front();
            if(r->type==Route::LOCALSERVICE)
            {
                LocalServiceRoute* l=(LocalServiceRoute*) r.operator ->();
//                msockaddr_in addr;
//                if(!sessions->getAddrOnConnected(E->esi->m_id,addr))
//                {
//                    return true;
//                }
                if(!E->esi->request_for_connect.valid())
                    throw CommonError("if(!E->esi->request_for_connect.valid())");

                sendEvent(l->id,new rpcEvent::IncomingOnConnector(E->esi,E->esi->request_for_connect->addr,e));
            }
            else
            {
                throw CommonError("!(r->type==Route::LOCALSERVICE %d) %s %s %s %s ",
                                  r->type,e->id.dump().c_str(),r->dump().c_str(),e->dump().toStyledString().c_str(),_DMI().c_str());
            }
        }
        else if(direction=='s')
        {
            MUTEX_INSPECTOR;
            SERVICE_id dst;
            buf>>dst;
            REF_getter<Event::Base> e=iUtils->unpackEvent(buf);
            if(!e.valid())
            {
                return true;
            }

            e->route.push_front(new RemoteAddrRoute(E->esi->m_id));
            e->route.push_front(new LocalServiceRoute(dst));
//            msockaddr_in addr;
            if(!E->esi->request_for_connect.valid())
                throw CommonError("if(!E->esi->request_for_connect.valid())");
//            if(sessions->getAddrOnConnected(E->esi->m_id,addr))
            {
                sendEvent(dst,new rpcEvent::IncomingOnConnector(E->esi,E->esi->request_for_connect->addr,e));
            }
        }
    }
    catch(std::exception &e)
    {
        DBG(logErr2("PKT RPC catched %s",e.what()));
        E->esi->close("broken packet int RPC");
    }
    return true;

}

void RPC::Service::addSendPacket(const REF_getter<Session>&S, const REF_getter<refbuffer>&P)
{
    MUTEX_INSPECTOR;
    S_LOG("addSendPacket");
    {

        S->m_OutEventCache.push_back(P);
    }
    if(S->esi.valid())
    {
        doSend(S);
    }
    else
    {
    }
}
bool RPC::Service::on_PassPacket(const rpcEvent::PassPacket* E)
{

//    logErr2("@@ %s",__PRETTY_FUNCTION__);
    MUTEX_INSPECTOR;

    XTRY;
    const rpcEvent::PassPacket *e=E;
    if(!e)
        return false;
    outBuffer o;
    o<<'p';
    iUtils->packEvent(o,e->e);

    REF_getter<Session> S(NULL);
//    bool found=false;
    int attempts=0;
    while(!S.valid())
    {
        RLocker lk(sessions->m_lock);

        auto it=sessions->mx_socketId2session.find(E->socketIdTo);
        if(it==sessions->mx_socketId2session.end())
        {

            logErr2(" %s Session not found",__PRETTY_FUNCTION__);
        }
        else {
            S=it->second;
            break;
        }
        attempts++;
        if(!S.valid())
        {
            usleep(10000);
        }
        if(attempts>5)
            throw CommonError("if(attempts>5) %s",__PRETTY_FUNCTION__);
    }


    {
        addSendPacket(S,o.asString());
    }

    XPASS;
    return true;
}

bool RPC::Service::on_NotifyBindAddress(const oscarEvent::NotifyBindAddress*e)
{
    MUTEX_INSPECTOR;

    {
        M_LOCK(sharedAddr);
//        sharedAddr.m_networkInitialized=true;
        sharedAddr.m_bindAddr_mainSH.insert(e->addr);
    }
    doSendAll();
    return true;
}

bool RPC::Service::on_startService(const systemEvent::startService* )
{
    MUTEX_INSPECTOR;
    XTRY;

    myOscarListener=dynamic_cast<ListenerBase*>(iInstance->getServiceOrCreate(Service::myOscar));


    {
        {


            for(auto &item:m_bindAddr_main)
            {
                SOCKET_id newid=iUtils->getSocketId();
                sendEvent(myOscarListener,new oscarEvent::AddToListenTCP(newid,item,"RPC_UL",dynamic_cast<ListenerBase*>(this)));
            }

            for(auto &item:m_bindAddr_reserve)
            {
                SOCKET_id newid=iUtils->getSocketId();
                sendEvent(myOscarListener,new oscarEvent::AddToListenTCP(newid,item,"RPC_DL",dynamic_cast<ListenerBase*>(this)));
            }
        }
    }

    XPASS;
    return true;
}
bool RPC::Service::on_Accepted(const oscarEvent::Accepted* E)
{
//    logErr2("@@ %s",__PRETTY_FUNCTION__);
    MUTEX_INSPECTOR;
    S_LOG("on_Accepted");

    std::set<route_t> subscribers;
    {
        {
            REF_getter<Session> S=new Session(E->esi->m_id,E->esi);
            {
                WLocker lk(sessions->m_lock);

                sessions->mx_socketId2session.emplace(E->esi->m_id,S);
                subscribers=sessions->mx_subscribers;
            }
        }
    }

    for(auto &i:subscribers)
    {
        route_t r=i;
        r.pop_front();
        passEvent(new rpcEvent::Accepted(E->esi,r));

    }


    return true;
}

bool RPC::Service::on_SendPacket(const rpcEvent::SendPacket* E)
{

    MUTEX_INSPECTOR;

    S_LOG("on_SendPacket");
    XTRY;
//    REF_getter<Session> S(NULL);
//    SOCKET_id sockId;
//    CONTAINER(sockId)=0L;
    bool found=false;

    REF_getter<Session> S(NULL);
    {
        {
            RLocker lk(sessions->m_lock);
            auto it=sessions->mx_sa2Session.find(E->addressTo);
            if(it!=sessions->mx_sa2Session.end())
                S=it->second;
        }

        if(!S.valid())
        {
            SOCKET_id sockId=iUtils->getSocketId();
            msockaddr_in addressTo=E->addressTo;
            S=new Session(sockId,NULL);
            {
                WLocker lk(sessions->m_lock);

                sessions->mx_sa2Session.emplace(E->addressTo,S);
//                logErr2("sessions->mx_sa2Session.emplace %s",E->addressTo.dump().c_str());
            }
            sendEvent(myOscarListener,new oscarEvent::Connect(sockId,E->addressTo,"RPC",dynamic_cast<ListenerBase*>(this)));

        }
    }

    {

        XTRY;
        outBuffer o;
        o<<'s';
        o<<E->destination;
        iUtils->packEvent(o,E->ev);


        addSendPacket(S,o.asString());
        XPASS;
    }
    XPASS;
    return true;
}
void RPC::Service::doSend(const REF_getter<Session> & S)
{
    MUTEX_INSPECTOR;

    S_LOG("doSend");
    if(S.valid())
    {
        if(S->esi.valid())
        {
            {
                {
                    auto i=S->m_OutEventCache.begin();
                    std::deque<REF_getter<refbuffer> > & d=S->m_OutEventCache;
                    while(d.size())
                    {

                        REF_getter<refbuffer> p=d[0];
                        d.pop_front();
                        sendEvent(myOscarListener,new oscarEvent::SendPacket(S->esi,p,dynamic_cast<ListenerBase*>(this)));

                    }

                }
            }
        }
        else
        {
#ifndef __MOBILE__
            DBG(logErr2("!S->m_connectionEstablished"));
#endif
        }

    }
    else
    {
        logErr2("incorrect: if(S.valid()) %s %d %s",__FILE__,__LINE__,_DMI().c_str());
    }
}

bool RPC::Service::on_NotifyOutBufferEmpty(const oscarEvent::NotifyOutBufferEmpty* e)
{


    MUTEX_INSPECTOR;

    return true;
}



UnknownBase* RPC::Service::construct(const SERVICE_id& id, const std::string&  nm,IInstance* ifa)
{
    XTRY;
    return new Service(id,nm,ifa);
    XPASS;
}

bool RPC::Service::on_UnsubscribeNotifications(const rpcEvent::UnsubscribeNotifications* E)
{
    WLocker lk(sessions->m_lock);

    sessions->mx_subscribers.erase(E->route);
    return true;

}

bool RPC::Service::on_SubscribeNotifications(const rpcEvent::SubscribeNotifications* E)
{
    WLocker lk(sessions->m_lock);
    sessions->mx_subscribers.insert(E->route);
    return true;
}


void registerRPCService(const char* pn)
{
    if(pn)
    {
        iUtils->registerPlugingInfo(COREVERSION,pn,IUtils::PLUGIN_TYPE_SERVICE,ServiceEnum::RPC,"RPC",getEvents_rpc());
    }
    else
    {
        iUtils->registerService(COREVERSION,ServiceEnum::RPC,RPC::Service::construct,"RPC");
        regEvents_rpc();
    }

}



bool RPC::Service::on_TickAlarm(const timerEvent::TickAlarm*e)
{
    MUTEX_INSPECTOR;
    return true;
}
Json::Value RPC::__sessions::jdump()
{
    MUTEX_INSPECTOR;
    Json::Value v;
    std::map<SOCKET_id, REF_getter<Session> >m;
    std::set<route_t> s;
    {
        RLocker lk(m_lock);

        m=mx_socketId2session;
        s=mx_subscribers;
    }
    v["sessions.size"]=std::to_string(m.size());
    v["subscribers.size"]=std::to_string(s.size());
    for(auto &i:s)
    {
        v["subscribers"].append(i.dump());
    }
    for(auto &i:m)
    {
        v["sessions"].append(i.second->jdump());
    }
    return v;
}

unsigned short RPC::Service::getExternalListenPortMain()
{
    MUTEX_INSPECTOR;
    bool cond=false;
    {
        M_LOCK(sharedAddr);
        cond=/*!sharedAddr.m_networkInitialized &&*/ sharedAddr.m_bindAddr_mainSH.empty();
    }
    while(cond)
    {
        if(m_isTerminating)
        {
            logErr2("@@ %s %d ret 0",__PRETTY_FUNCTION__,__LINE__);
            return 0;
        }
        usleep(10000);
        {
            M_LOCK(sharedAddr);
            cond=/*!sharedAddr.m_networkInitialized &&*/ sharedAddr.m_bindAddr_mainSH.empty();

        }

    }
    {
        M_LOCK(sharedAddr);
        if(sharedAddr.m_bindAddr_mainSH.size()==0)
        {
            logErr2("@@ %s %d ret 0",__PRETTY_FUNCTION__,__LINE__);
            return 0;
        }
        return sharedAddr.m_bindAddr_mainSH.begin()->port();
    }
}
std::set<msockaddr_in> RPC::Service::getInternalListenAddrs()
{
    MUTEX_INSPECTOR;
    bool cond=false;
    {
        M_LOCK(sharedAddr);
        cond=/*!sharedAddr.m_networkInitialized &&*/ sharedAddr.m_bindAddr_mainSH.empty();
    }
    while(cond)
    {
        usleep(10000);
        {
            M_LOCK(sharedAddr);
            cond=/*!sharedAddr.m_networkInitialized &&*/ sharedAddr.m_bindAddr_mainSH.empty();
        }
    }
    M_LOCK(sharedAddr);
    return sharedAddr.m_internalAddr;

}
void RPC::Service::doSendAll()
{
    std::set<REF_getter<Session> > s;
    {
        RLocker lk(sessions->m_lock);

        for(auto& z: sessions->mx_socketId2session)
            s.insert(z.second);
        for(auto& z: sessions->mx_sa2Session)
            s.insert(z.second);
    }
    for(auto &i:s)
    {

        doSend(i);
    }

}
bool RPC::Service::on_ConnectFailed(const oscarEvent::ConnectFailed*e)
{

    MUTEX_INSPECTOR;
    std::set<route_t> subscribers;
    if(!e->esi->request_for_connect.valid())
        throw CommonError("if(!e->esi->request_for_connect.valid())");

    {
        WLocker lk(sessions->m_lock);
        subscribers=sessions->mx_subscribers;
        sessions->mx_sa2Session.erase(e->esi->request_for_connect->addr);
    }

    for(auto &i:subscribers)
    {
        route_t r=i;
        r.pop_front();

        passEvent(new rpcEvent::ConnectFailed(e->addr,r));
    }
//    cleanSocket(e->esi->m_id);
    return true;
}
bool RPC::Service::on_Disconnected(const oscarEvent::Disconnected* e)
{
    MUTEX_INSPECTOR;
    std::set<route_t> subscribers;
    {
        RLocker lk(sessions->m_lock);
        subscribers=sessions->mx_subscribers;
    }


    for(auto &i:subscribers)
    {
        route_t r=i;
        r.pop_front();

        DBG(logErr2("-------------DISCONNECTED SUCCESSED"));
        if(!e->esi->request_for_connect.valid())
            throw CommonError("if(!e->esi->request_for_connect.valid())");
        passEvent(new rpcEvent::Disconnected(e->esi,e->esi->request_for_connect->addr,e->reason,r));
    }
    {
        WLocker lk(sessions->m_lock);

        sessions->mx_sa2Session.erase(e->esi->request_for_connect->addr);
    }

    return true;
}
bool RPC::Service::on_Disaccepted(const oscarEvent::Disaccepted*e)
{
    MUTEX_INSPECTOR;

    std::set<route_t> subscribers;
    {
        RLocker lk(sessions->m_lock);

        subscribers=sessions->mx_subscribers;
    }

    for(auto &i:subscribers)
    {
        route_t r=i;
        r.pop_front();

        passEvent(new rpcEvent::Disaccepted(e->esi,e->reason,r));
    }
    {
        WLocker lk(sessions->m_lock);
        sessions->mx_socketId2session.erase(e->esi->m_id);
    }


    return true;

}

bool RPC::Service::handleEvent(const REF_getter<Event::Base>& e)
{
    MUTEX_INSPECTOR;
    XTRY;
    //auto _this=this;
    auto& ID=e->id;
    if(timerEventEnum::TickAlarm==ID)
        return on_TickAlarm((const timerEvent::TickAlarm*)e.operator->());
    if(timerEventEnum::TickTimer==ID)
        return true;
    if( oscarEventEnum::PacketOnAcceptor==ID)
        return(this->on_PacketOnAcceptor((const oscarEvent::PacketOnAcceptor*)e.operator->()));

    if( oscarEventEnum::PacketOnConnector==ID)
        return(this->on_PacketOnConnector((const oscarEvent::PacketOnConnector*)e.operator->()));

    if( oscarEventEnum::Connected==ID)
        return(this->on_Connected((const oscarEvent::Connected*)e.operator->()));

    if( oscarEventEnum::Accepted==ID)
        return(this->on_Accepted((const oscarEvent::Accepted*)e.operator->()));

    if( oscarEventEnum::NotifyBindAddress==ID)
        return(this->on_NotifyBindAddress((const oscarEvent::NotifyBindAddress*)e.operator->()));

    if( oscarEventEnum::NotifyOutBufferEmpty==ID)
        return(this->on_NotifyOutBufferEmpty((const oscarEvent::NotifyOutBufferEmpty*)e.operator->()));


    if( oscarEventEnum::ConnectFailed==ID)
        return(this->on_ConnectFailed((const oscarEvent::ConnectFailed*)e.operator->()));

    if(systemEventEnum::startService==ID)
        return on_startService((const systemEvent::startService*)e.operator->());

    if( rpcEventEnum::PassPacket==ID)
        return(this->on_PassPacket((const rpcEvent::PassPacket*)e.operator->()));
    if( rpcEventEnum::SendPacket==ID)
        return(this->on_SendPacket((const rpcEvent::SendPacket*)e.operator->()));
    if( rpcEventEnum::SubscribeNotifications==ID)
        return(this->on_SubscribeNotifications((const rpcEvent::SubscribeNotifications*)e.operator->()));
    if( rpcEventEnum::UnsubscribeNotifications==ID)
        return(this->on_UnsubscribeNotifications((const rpcEvent::UnsubscribeNotifications*)e.operator->()));
    if( oscarEventEnum::Disconnected==ID)
        return(this->on_Disconnected((const oscarEvent::Disconnected*)e.operator->()));
    if( oscarEventEnum::Disaccepted==ID)
        return(this->on_Disaccepted((const oscarEvent::Disaccepted*)e.operator->()));
    if( webHandlerEventEnum::RequestIncoming==ID)
        return(this->on_RequestIncoming((const webHandlerEvent::RequestIncoming*)e.operator->()));


    XPASS;
    return false;

}
bool RPC::Service::on_RequestIncoming(const webHandlerEvent::RequestIncoming* e)
{

    MUTEX_INSPECTOR;
    HTTP::Response cc(iInstance);
    cc.content+="<h1>RPC report</h1><p>";

    Json::Value v=jdump();
    Json::StyledWriter w;
    cc.content+="<pre>\n"+w.write(v)+"\n</pre>";

    cc.makeResponse(e->esi);
    return true;
}
Json::Value RPC::Service::jdump()
{
    MUTEX_INSPECTOR;
    Json::Value j;
    j["sessions"].append(sessions->getWebDumpableLink("sessions"));
    return j;

}
