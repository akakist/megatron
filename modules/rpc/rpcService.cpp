#define STATICLIB 1

#include "rpcService.h"

#include "version_mega.h"
#include "event.h"
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
    ListenerBuffered1Thread(this,nm,ifa->getConfig(),svs,ifa),
    Broadcaster(ifa),
    myOscar(ServiceEnum::OscarSecure),
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
                sharedAddr.m_bindAddr_main=ifa->getConfig()->get_tcpaddr("BindAddr_MAIN",_main,"Address used to work with dfs network. NONE - no bind");
                sharedAddr.m_bindAddr_reserve=ifa->getConfig()->get_tcpaddr("BindAddr_RESERVE",_reserve,"Address used to communicate with local apps, must be fixed. NONE - no bind");
                if(sharedAddr.m_bindAddr_main==sharedAddr.m_bindAddr_reserve && sharedAddr.m_bindAddr_main.size())
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
#if !defined(WITHOUT_UPNP)

            upnp_disablePortmapping();
#endif
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
    bool need_send_connected=false;
    msockaddr_in sa;
    socklen_t sl=sa.maxAddrLen();
    if(getsockname(CONTAINER(E->esi->get_fd()), sa.addr(),&sl))
    {
        logErr2("getsockname: errno %d %s",errno,strerror(errno));
    }
    else
    {
    }
    std::set<route_t> subscribers;
    {
//        M_LOCK(sessions->m_lock);
        auto it=sessions->all.m_socketId2session.find(E->esi->m_id);
        if(it==sessions->all.m_socketId2session.end())
        {
            DBG(logErr2("1cannot find RPC session on connected"));
            logErr2("2cannot find RPC session on connected");
            return true;
        }
        S=it->second;
        S->m_connectionEstablished=true;
        auto it2=sessions->connector.m_socketId2sa.find(E->esi->m_id);
        if(it2!=sessions->connector.m_socketId2sa.end())
        {
            addr=it2->second;
            need_send_connected=true;
            subscribers=sessions->all.m_subscribers;

        }
        else
        {
            throw CommonError("!!!!!!!!!!!!!!!!!!! not found sessions.m_socketId2sa.find(E->esi->m_id); %s %d",__FILE__,__LINE__);
        }
    }

    if(need_send_connected)
    {
        for(auto &i:subscribers)
        {
            route_t r=i;
            r.pop_front();
            passEvent(new rpcEvent::Connected(E->esi,addr,r));
        }
    }

    if(!S.valid()) throw CommonError("!!!!! FATAL invalid S %s %d",__FILE__,__LINE__);

    S->m_connectionEstablished=true;
//    sessions->add(ServiceEnum::RPC,E->esi);
    S->update_last_time_hit();
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
            SERVICE_id dst;//=buf.get_PN();
            //iInstance->initService(dst);
            buf>>dst;
//            UnknownBase *zzz=iInstance->getServiceOrCreate(dst);
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
bool RPC::__sessions::getAddrOnConnected(const SOCKET_id& id, msockaddr_in& out)
{
    MUTEX_INSPECTOR;


//    msockaddr_in addr;
    {
//        M_LOCK(m_lock);
        auto it=all.m_socketId2session.find(id);
        if(it==all.m_socketId2session.end())
        {
            return false;
//            throw CommonError("3cannot find RPC session on connected %s",_DMI().c_str());
        }
        auto it2=connector.m_socketId2sa.find(id);
        if(it2!=connector.m_socketId2sa.end())
        {
            out=it2->second;
            return true;
        }
        else
        {
            throw CommonError("!!!!!!!!!!!!!!!!!!! not found sessions.m_socketId2sa.find(E->esi->m_id); %s %d",__FILE__,__LINE__);
        }
    }
    return false;
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
                msockaddr_in addr;
                if(!sessions->getAddrOnConnected(E->esi->m_id,addr))
                {
                    return true;
                }

                sendEvent(l->id,new rpcEvent::IncomingOnConnector(E->esi,addr,e));
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
            msockaddr_in addr;
            if(sessions->getAddrOnConnected(E->esi->m_id,addr))
            {
                sendEvent(dst,new rpcEvent::IncomingOnConnector(E->esi,addr,e));
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

void RPC::Service::addSendPacket(const int& channel, const REF_getter<Session>&S, const REF_getter<oscarEvent::SendPacket>&P)
{
    MUTEX_INSPECTOR;
    S_LOG("addSendPacket");
//    logErr2("addSendPacket");
    S->update_last_time_hit();
    {

        M_LOCK(S.operator ->());
        if(S->bufferSize>10*1000*1000)
            throw CommonError("buffer too large");
        S->m_OutEventCache[channel].push_back(P);
        S->bufferSize+=P->buf->size_;
        {
            M_LOCK (mx);
            mx.totalSendBufferSize+=P->buf->size_;
        }
    }
    if(S->m_connectionEstablished)
    {
        doSend(S);
    }
    else
    {
    }
}
bool RPC::Service::on_PassPacket(const rpcEvent::PassPacket* E)
{

    MUTEX_INSPECTOR;

    XTRY;
    const rpcEvent::PassPacket *e=E;
    if(!e)
        return false;
    outBuffer o;
    o<<'p';
    iUtils->packEvent(o,e->e);

    REF_getter<Session> S=sessions->getSessionOrNull(e->socketIdTo);
    if(!S.valid())
        throw CommonError("if(!S.valid())");
    REF_getter<epoll_socket_info> esi=S->esi;
    if(!esi.valid())
        throw CommonError("if(!S->esi.valid())");
    if(!S.valid())
    {
        throw CommonError("if(!S.valid())");
    }
    {
        S->update_last_time_hit();
        addSendPacket(e->e->rpcChannel,S,new oscarEvent::SendPacket(S->esi->m_id,o.asString(),serviceId));
    }

    XPASS;
    return true;
}

bool RPC::Service::on_NotifyBindAddress(const oscarEvent::NotifyBindAddress*e)
{
    MUTEX_INSPECTOR;

    std::set<msockaddr_in>m_bindAddr_reserve,m_bindAddr_main;
    {
        M_LOCK(sharedAddr);
        m_bindAddr_reserve=sharedAddr.m_bindAddr_reserve;
        m_bindAddr_main=sharedAddr.m_bindAddr_main;

    }
    for(auto &i:m_bindAddr_reserve)
    {
        auto addr=i;
        if(e->rebind)
        {
            sessions->clear();
        }
        socklen_t len=addr.addrLen();

        if(getsockname(CONTAINER(e->esi->get_fd()),addr.addr(),&len))
        {
            logErr2("getsockname: errno %d %s",errno,strerror(errno));
            return true;
        }
        DBG(logErr2("--------GETSOCKNAME m_bindAddr_reserve retursn %s",addr.dump().c_str()));


        DBG(logErr2("---------------e->sockType %s",e->socketDescription.c_str()));
        {
            M_LOCK(sharedAddr);
            sharedAddr.m_networkInitialized=true;
        }
        doSendAll();

    }

    for(auto &i:m_bindAddr_main)
    {
        const msockaddr_in &addr=i;
        if(e->socketDescription=="RPC_UL")
        {
            if(e->rebind)
            {
                sessions->clear();
            }
            socklen_t len=addr.maxAddrLen();

            if(getsockname(CONTAINER(e->esi->get_fd()),addr.addr(),&len))
            {
                logErr2("getsockname: errno %d %s",errno,strerror(errno));
                return true;
            }

            DBG(logErr2("--------GETSOCKNAME m_bindAddr_main retursn %s",addr.dump().c_str()));


            DBG(logErr2("---------------e->sockType %s",e->socketDescription.c_str()));
            {
                M_LOCK(sharedAddr);
                sharedAddr.m_networkInitialized=true;
            }
            doSendAll();
        }
    }
    return true;
}

bool RPC::Service::on_startService(const systemEvent::startService* )
{
    MUTEX_INSPECTOR;
    XTRY;

    if(iUtils->isServiceRegistered(ServiceEnum::WebHandler))
    {
        sendEvent(ServiceEnum::WebHandler, new webHandlerEvent::RegisterHandler("RPC","RPC",ListenerBase::serviceId));
    }


    sendEvent(ServiceEnum::Timer,new timerEvent::SetTimer(timers::CONNECTION_ACTIVITY,NULL,NULL,m_iterateTimeout,ListenerBase::serviceId));

    {
        {
            std::set<msockaddr_in>m_bindAddr_reserve,m_bindAddr_main;
            {
                M_LOCK(sharedAddr);
                m_bindAddr_reserve=sharedAddr.m_bindAddr_reserve;
                m_bindAddr_main=sharedAddr.m_bindAddr_main;

            }


            for(auto &item:m_bindAddr_main)
            {
                SOCKET_id newid=iUtils->getSocketId();
                sendEvent(myOscar,new oscarEvent::AddToListenTCP(newid,item,"RPC_UL",ListenerBase::serviceId));
            }

            for(auto &item:m_bindAddr_reserve)
            {
                SOCKET_id newid=iUtils->getSocketId();
                sendEvent(myOscar,new oscarEvent::AddToListenTCP(newid,item,"RPC_DL",ListenerBase::serviceId));
            }
        }
    }

    XPASS;
    return true;
}
bool RPC::Service::on_Accepted(const oscarEvent::Accepted* E)
{
    MUTEX_INSPECTOR;
    S_LOG("on_Accepted");

    msockaddr_in sa;
    socklen_t sl=sa.maxAddrLen();
    if(getsockname(CONTAINER(E->esi->get_fd()), sa.addr(),&sl))
    {
        logErr2("getsockname: errno %d %s",errno,strerror(errno));
    }

    DBG(logErr2("accepted socket %d",CONTAINER(E->esi->get_fd())));
    std::set<route_t> subscribers;
    {
        const oscarEvent::Accepted *e=E;
        {
            auto it=sessions->all.m_socketId2session.find(e->esi->m_id);
            if(it==sessions->all.m_socketId2session.end())
            {
                REF_getter<Session> S=new Session(e->esi->m_id,E->esi);
                sessions->all.m_socketId2session.insert(std::make_pair(e->esi->m_id,S));
                S->m_connectionEstablished=true;
                S->update_last_time_hit();
                subscribers=sessions->all.m_subscribers;
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
    REF_getter<Session> S(NULL);
    SOCKET_id sockId;
    CONTAINER(sockId)=0L;
    bool found=false;

    {

        auto it=sessions->connector.m_sa2socketId.find(E->addressTo);
        if(it!=sessions->connector.m_sa2socketId.end())
        {
            sockId=it->second;
            found=true;

            if(!sessions->connector.m_socketId2sa.count(sockId))
            {
                found=false;
                logErr2("found=false if(!sessions->connector.m_socketId2sa.count(sockId))");
            }
        }
        if(found)
        {
            auto its=sessions->all.m_socketId2session.find(sockId);
            if(its==sessions->all.m_socketId2session.end())
            {
                found=false;
                logErr2("found=false if(its==sessions->all.m_socketId2session.end())");
            }
            REF_getter<epoll_socket_info> esi=its->second->esi;
            if(esi.valid())
            {
                if(esi->closed())
                    found=false;

            }
            else
            {
                found=false;
            }
        }
        else
        {
//            logErr2("!found %s %d",__FILE__,__LINE__);
        }
    }
    if(!found)
    {
        XTRY;

        sockId=iUtils->getSocketId();
        msockaddr_in addressTo=E->addressTo;

        sessions->connector.m_socketId2sa[sockId]=addressTo;
        sessions->connector.m_sa2socketId[addressTo]=sockId;
        S=new Session(sockId,NULL);

        sessions->all.m_socketId2session.erase(sockId);
        sessions->all.m_socketId2session.insert(std::make_pair(sockId,S));
        {
            sendEvent(myOscar,new oscarEvent::Connect(sockId,addressTo,"RPC",ListenerBase::serviceId));
        }
        XPASS;

    }
    else
    {
//        logErr2("found %s %d",__FILE__,__LINE__);

        XTRY;
        auto its=sessions->all.m_socketId2session.find(sockId);
        if(its==sessions->all.m_socketId2session.end())
        {
            throw CommonError("!!!!!!!!!! cannot find session for socketID");
        }

        S=its->second;
        XPASS;
    }

    {

        XTRY;
        if(CONTAINER(sockId)==0) throw CommonError("CONTAINER(sockId)==0");
        outBuffer o;
        o<<'s';
        o<<E->destination;
        iUtils->packEvent(o,E->ev);


        addSendPacket(E->ev->rpcChannel,S,new oscarEvent::SendPacket(sockId,o.asString(),ListenerBase::serviceId));
        XPASS;
    }
    XPASS;
    return true;
}
void RPC::Service::doSend(const REF_getter<Session> & S)
{
//    logErr2("RPC doSend");
    MUTEX_INSPECTOR;

    S_LOG("doSend");
    S->cstate=Session::EMPTY;
    if(S.valid())
    {
        if(S->m_connectionEstablished)
        {
            S->cstate=Session::FULL;
            size_t sum=0;
            {
                M_LOCK(S.operator ->());
                while(S->m_OutEventCache.size())
                {
                    auto i=S->m_OutEventCache.begin();
                    std::deque<REF_getter<oscarEvent::SendPacket> > & d=i->second;
                    while(d.size())
                    {

                        REF_getter<oscarEvent::SendPacket> p=d[0];
                        d.pop_front();
                        S->bufferSize-=p->buf->size_;
                        {
                            M_LOCK(mx);
                            mx.totalSendBufferSize-=p->buf->size_;
                        }
                        sum+=p->buf->size_;
                        sendEvent(myOscar,p.operator ->());

                    }
                    S->m_OutEventCache.erase(i->first);

                }
                S->cstate=Session::EMPTY;
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

    S_LOG("on_NotifyOutBufferEmpty");
    std::set<route_t> subscribers;
    {
        auto it=sessions->all.m_socketId2session.find(e->esi->m_id);
        if(it==sessions->all.m_socketId2session.end())
        {
            REF_getter<Session> S=new Session(e->esi->m_id,e->esi);
            sessions->all.m_socketId2session.insert(std::make_pair(e->esi->m_id,S));
            S->m_connectionEstablished=true;
            S->update_last_time_hit();
            subscribers=sessions->all.m_subscribers;
        }

    }
    for(auto &i:subscribers)
    {
        passEvent(new rpcEvent::Accepted(e->esi,poppedFrontRoute(i)));

    }
    REF_getter<Session> S=sessions->getSessionOrNull(e->esi->m_id);
    if(S.valid())
    {
        doSend(S);
    }
    else
    {
        DBG(logErr2("!S.valid sock %s in session %s %d",e->esi->__jdump().toStyledString().c_str(),__FILE__,__LINE__));

    }
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
    sessions->all.m_subscribers.erase(E->route);
    return true;

}

bool RPC::Service::on_SubscribeNotifications(const rpcEvent::SubscribeNotifications* E)
{
    sessions->all.m_subscribers.insert(E->route);
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


#if !defined(WITHOUT_UPNP)
void RPC::Service::upnp_disablePortmapping()
{
    bool cond;
    {
        M_LOCK(sharedAddr);
        cond= static_cast<bool>(sharedAddr.m_bindAddr_main.size());
    }
    if(cond)
    {
#ifdef __MOBILE__
        throw CommonError("call upnp_disablePortmapping in mobile");
#else
        M_LOCK(upnp.upnpMX);
        if(!upnp.upnp) upnp.upnp=tr_upnpInit();
        tr_upnpPulse(upnp.upnp,m_bindAddr_main.begin()
                     ->port(),false,true);
#endif
    }
}
#endif

#if !defined(WITHOUT_UPNP)
void RPC::Service::upnp_enablePortmapping()
{
    //if(iInstance->no_bind())
    //  throw CommonError("no bind and void Service::upnp_pulse() %s %d",__FILE__,__LINE__);
    //
    bool cond;
    {
        M_LOCK(sharedAddr);
        cond=sharedAddr.m_bindAddr_main.size();
    }
    if(cond)
    {
//#ifndef __MACH__
#ifdef __MOBILE__
        throw CommonError("call upnp_enablePortmapping in mobile");
#else
        M_LOCK(upnp.upnpMX);
        DBG(logErr2("void Service::upnp_pulse()"));
        if(!upnp.upnp) upnp.upnp=tr_upnpInit();
        int res=tr_upnpPulse(upnp.upnp,m_bindAddr_main.begin()->port(),true,true);
        DBG(logErr2("tr_upnpPulse res %d",res));
        if(/*res!=0 && */strlen(tr_lanaddr))
        {
            M_LOCK(mintAddr);
            msockaddr_in sa;
            DBG(logErr2("tr_lanaddr %s %d",tr_lanaddr,tr_port));
            sa.init(tr_lanaddr,tr_port);
            mintAddr.m_internalAddr.insert(sa);
            DBG(logErr2("------mintAddr.m_internalAddr %s",iUtils->dump(mintAddr.m_internalAddr).c_str()));
        }
#endif
    }

}
#endif
bool RPC::Service::on_TickAlarm(const timerEvent::TickAlarm*e)
{
    MUTEX_INSPECTOR;
    switch(e->tid)
    {
    case timers::CONNECTION_ACTIVITY:
    {
        inBuffer in(e->data);
        SOCKET_id sid;
        in>>sid;
        std::map<SOCKET_id, REF_getter<Session> > ss=sessions->getSessionContainer();
        time_t now=time(NULL);
        for(auto& i:ss)
        {
            REF_getter<Session> S=i.second;
            REF_getter<epoll_socket_info>esi=S->esi;
            if(S.valid() && esi.valid())
            {
                if(now-S->last_time_hit>m_connectionActivityTimeout)
                {
                    DBG(logErr2("close socket S->esi-> %s",esi->remote_name.dump().c_str()));
                    esi->close("rpc close due timer inactivity");
                }
            }
            else logErr2("--------------- invalid behaviour if(S.valid() && esi.valid()) %s %d",__FILE__,__LINE__);
        }
        DBG(logErr2("srabotal timers::CONNECTION_ACTIVITY on socket %d (%s %d) ",CONTAINER(sid),__FILE__,__LINE__));
    }
    break;
    default:
        throw CommonError("invalid timer id %s %d",__FILE__,__LINE__);
    }
    return true;
}
Json::Value RPC::__sessions::jdump()
{
    MUTEX_INSPECTOR;
    Json::Value v;
    std::map<SOCKET_id, REF_getter<Session> >m;
    std::set<route_t> s;
    {
        m=all.m_socketId2session;
        s=all.m_subscribers;
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
        cond=!sharedAddr.m_networkInitialized && sharedAddr.m_bindAddr_main.size();
    }
    while(cond)
    {
        if(m_isTerminating) return 0;
        usleep(10000);
        if(m_isTerminating) return 0;
        {
            M_LOCK(sharedAddr);
            cond=!sharedAddr.m_networkInitialized && sharedAddr.m_bindAddr_main.size();
        }
    }
    {
        M_LOCK(sharedAddr);
        if(sharedAddr.m_bindAddr_main.size()==0)
        {
            return 0;
        }
        return sharedAddr.m_bindAddr_main.begin()->port();
    }
}
std::set<msockaddr_in> RPC::Service::getInternalListenAddrs()
{
    MUTEX_INSPECTOR;
    bool cond=false;
    {
        M_LOCK(sharedAddr);
        cond=!sharedAddr.m_networkInitialized && sharedAddr.m_bindAddr_main.size();
    }
    while(cond)
    {
        usleep(10000);
        {
            M_LOCK(sharedAddr);
            cond=!sharedAddr.m_networkInitialized && sharedAddr.m_bindAddr_main.size();
        }
    }
    M_LOCK(sharedAddr);
    return sharedAddr.m_internalAddr;

}
void RPC::Service::doSendAll()
{
    std::map<SOCKET_id, REF_getter<Session> > m=sessions->getSessionContainer();
    for(auto &i:m)
    {

        doSend(i.second);
    }

}
#if !defined(WITHOUT_UPNP)
bool RPC::Service::on_UpnpPortMap(const rpcEvent::UpnpPortMap* e)
{
    DBG(logErr2("--------------before upnp_pulse();"));
#if !defined(WITHOUT_UPNP)
    upnp_enablePortmapping();
#endif
    DBG(logErr2("--------------upnp_pulse DONE"));
    route_t r=e->route;
    r.pop_front();

    passEvent(new rpcEvent::UpnpResult(e->cookie,r));

    return true;
}
#endif
bool RPC::Service::on_ConnectFailed(const oscarEvent::ConnectFailed*e)
{

    MUTEX_INSPECTOR;
    std::set<route_t> subscribers;
    {
//        M_LOCK(sessions->m_lock);
        subscribers=sessions->all.m_subscribers;
    }
    sessions->all.m_socketId2session.erase(e->esi->m_id);
    sessions->connector.m_sa2socketId.erase(e->esi->remote_name);
    sessions->connector.m_socketId2sa.erase(e->esi->m_id);

    for(auto &i:subscribers)
    {
        route_t r=i;
        r.pop_front();

        passEvent(new rpcEvent::ConnectFailed(e->addr,r));
    }
    cleanSocket(e->esi->m_id);
    return true;
}
void RPC::Service::cleanSocket(const SOCKET_id& sid)
{
    MUTEX_INSPECTOR;
    {
        if(sessions->all.m_socketId2session.count(sid))
        {
            sessions->all.m_socketId2session.erase(sid);
        }
    }
    {
        if(sessions->connector.m_socketId2sa.count(sid))
        {
            logErr2("cleanSocket: sessions->connector.m_socketId2sa.erase %s",std::to_string(CONTAINER(sid)).c_str());
            sessions->connector.m_socketId2sa.erase(sid);
        }
        std::set<msockaddr_in> rm;
        for(auto &z: sessions->connector.m_sa2socketId)
        {
            if(z.second==sid)
            {

                logErr2("cleanSocket: sessions->connector.m_sa2socketId erase %s",z.first.dump().c_str());
                rm.insert(z.first);
            }

        }
        for(auto &z: rm)
            sessions->connector.m_sa2socketId.erase(z);

    }

}
bool RPC::Service::on_Disconnected(const oscarEvent::Disconnected* e)
{
    MUTEX_INSPECTOR;
    std::set<route_t> subscribers;
    {
        subscribers=sessions->all.m_subscribers;
    }


    for(auto &i:subscribers)
    {
        route_t r=i;
        r.pop_front();
        if(e->esi->m_streamType==epoll_socket_info::STREAMTYPE_CONNECTED)
        {
            msockaddr_in destination_addr;
            if(!sessions->getAddrOnConnected(e->esi->m_id,destination_addr))
            {
                logErr2("if(!sessions->getAddrOnConnected(e->esi->m_id,destination_addr))");
//                return true;
            }
            logErr2("destination_addr %s",destination_addr.dump().c_str());
            if(e->esi->inConnection)
            {
                throw CommonError("invalid USECASE with connect failed "+_DMI());

            }
//            else
            {
                DBG(logErr2("-------------DISCONNECTED SUCCESSED"));
                passEvent(new rpcEvent::Disconnected(e->esi,destination_addr,e->reason,r));
            }
        }
        else if(e->esi->m_streamType==epoll_socket_info::STREAMTYPE_ACCEPTED)
        {
            passEvent(new rpcEvent::Disaccepted(e->esi,e->reason,r));
        }
        else throw CommonError("invalid esi type %s %d",__FILE__,__LINE__);
    }
    cleanSocket(e->esi->m_id);

    return true;
}
bool RPC::Service::on_Disaccepted(const oscarEvent::Disaccepted*e)
{
    MUTEX_INSPECTOR;

    std::set<route_t> subscribers;
    {
        subscribers=sessions->all.m_subscribers;
    }

    for(auto &i:subscribers)
    {
        route_t r=i;
        r.pop_front();
        if(e->esi->m_streamType==epoll_socket_info::STREAMTYPE_CONNECTED)
        {
            msockaddr_in destination_addr;
            if(!sessions->getAddrOnConnected(e->esi->m_id,destination_addr))
            {
                logErr2("TODO!! if(!sessions->getAddrOnConnected(e->esi->m_id,destination_addr))");
            }
            if(e->esi->inConnection)
            {
                throw CommonError("invalid USECASE with connect failed "+_DMI());

            }
            else
            {
                DBG(logErr2("-------------DISCONNECTED SUCCESSED"));
                passEvent(new rpcEvent::Disconnected(e->esi,destination_addr,e->reason,r));
            }
        }
        else if(e->esi->m_streamType==epoll_socket_info::STREAMTYPE_ACCEPTED)
        {
            passEvent(new rpcEvent::Disaccepted(e->esi,e->reason,r));
        }
        else throw CommonError("invalid esi type %s %d",__FILE__,__LINE__);
    }
    cleanSocket(e->esi->m_id);


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
#if !defined(WITHOUT_UPNP)
    if( rpcEventEnum::UpnpPortMap==ID)
        return(this->on_UpnpPortMap((const rpcEvent::UpnpPortMap*)e.operator->()));
#endif


    XPASS;
    return false;

}
bool RPC::Service::on_RequestIncoming(const webHandlerEvent::RequestIncoming* e)
{

    MUTEX_INSPECTOR;
    HTTP::Response cc;
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
    {
        M_LOCK(mx);
        j["totalSendBufferSize"]=std::to_string(mx.totalSendBufferSize);
    }
    j["sessions"].append(sessions->getWebDumpableLink("sessions"));
    return j;

}
