#define STATICLIB 1
#include "rpcService.h"

#include "version_mega.h"
#include "Events/System/Net/rpc/IncomingOnAcceptor.h"
#include "Events/System/Net/rpc/IncomingOnConnector.h"
#include "Events/System/Net/rpc/ConnectFailed.h"
#include "Events/System/Net/rpc/Disconnect.h"
#include "Events/System/Net/rpc/Disaccept.h"
#include "Events/System/Net/rpc/Disconnected.h"
#include "Events/System/Net/rpc/Disaccepted.h"
#include "Events/System/Net/oscar/AddToListenTCP.h"
#include "Events/System/Net/oscar/Connect.h"
#include "events_rpc.hpp"
#include "pconfig.h"
RPC::Service::Service(const SERVICE_id &svs, const std::string&  nm, IInstance* ifa):
    UnknownBase(nm),
    ListenerBuffered1Thread(this,nm,ifa->getConfig(),svs,ifa),
    Broadcaster(ifa),
    IRPC(this),
    Logging(this,
#ifdef DEBUG
            TRACE_log
#else
            INFO_log
#endif
    ,ifa       ),
    myOscar(0L),
    m_iterateTimeout(ifa->getConfig()->get_real("IterateTimeoutSec",60,"")),
    m_networkInitialized(false),
    m_connectionActivityTimeout(ifa->getConfig()->get_real("ConnectionActivity",600.0,"")),
    sessions(new __sessions(ifa)),
    iInstance(ifa),
    m_isTerminating(false)
{

    try
    {
        
        XTRY;
        myOscar=ServiceEnum::OscarSecure;
        //if(!ifa->no_bind())
        {
            std::string _main,_reserve;
//#if defined(QT5) || defined(__ANDROID__)
#if defined(__ANDROID__) || defined(__IOS__) || defined(QT_CORE_LIB)
            _main="NONE";
#else
            _main="INADDR_ANY:0";
#endif
//#else
//            _main="INADDR_ANY:0";
//#endif
            _reserve="NONE";
            m_bindAddr_main=ifa->getConfig()->get_tcpaddr("BindAddr_MAIN",_main,"Address used to work with dfs network. NONE - no bind");
            m_bindAddr_reserve=ifa->getConfig()->get_tcpaddr("BindAddr_RESERVE",_reserve,"Address used to communicate with local apps, must be fixed. NONE - no bind");
            if(m_bindAddr_main==m_bindAddr_reserve && m_bindAddr_main.size())
            {
                throw CommonError("if(m_bindAddr==m_bindAddr2)");
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
    if(mintAddr.m_internalAddr.size())
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

    REF_getter<Session> S(NULL);
    msockaddr_in addr;
    bool need_send_connected=false;
    msockaddr_in sa;
    socklen_t sl=sa.maxAddrLen();
    if(getsockname(CONTAINER(E->esi->get_fd()), sa.addr(),&sl))
    {
        log(ERROR_log,"getsockname: errno %d",errno);
    }
    else
    {
    }
    std::set<route_t> subscribers;
    {
        M_LOCK(sessions->m_lock);
        std::map<SOCKET_id,REF_getter<Session> >::iterator it=sessions->all.m_socketId2session.find(E->esi->m_id);
        if(it==sessions->all.m_socketId2session.end())
        {
            DBG(log(TRACE_log,"cannot find RPC session on connected"));
            logErr2("cannot find RPC session on connected");
            return true;
        }
        S=it->second;
        std::map<SOCKET_id,msockaddr_in>::iterator it2=sessions->connector.m_socketId2sa.find(E->esi->m_id);
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
    sessions->add(ServiceEnum::RPC,E->esi);
    S->update_last_time_hit();
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

        //logErr2("UNPACK %s",iUtils->bin2hex(E->buf).c_str());
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
            UnknownBase *zzz=iInstance->getServiceOrCreate(dst);
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
        DBG(log(TRACE_log,"PKT RPC catched %s",e.what()));
        E->esi->close("broken packet in RPC");
    }

    return true;
}
msockaddr_in RPC::__sessions::getAddrOnConnected(const SOCKET_id& id)
{
    

    msockaddr_in addr;
    {
        M_LOCK(m_lock);
        std::map<SOCKET_id,REF_getter<Session> >::iterator it=all.m_socketId2session.find(id);
        if(it==all.m_socketId2session.end())
        {
            throw CommonError("cannot find RPC session on connected %s",_DMI().c_str());
        }
        std::map<SOCKET_id,msockaddr_in>::iterator it2=connector.m_socketId2sa.find(id);
        if(it2!=connector.m_socketId2sa.end())
        {
            addr=it2->second;
        }
        else
        {
            throw CommonError("!!!!!!!!!!!!!!!!!!! not found sessions.m_socketId2sa.find(E->esi->m_id); %s %d",__FILE__,__LINE__);
        }
    }
    return addr;
}


bool RPC::Service::on_PacketOnConnector(const oscarEvent::PacketOnConnector* E)
{

    

    try {
        inBuffer buf(E->buf);
        int direction;
        buf>>direction;

        //logErr2("UNPACK %s",iUtils->bin2hex(E->buf).c_str());
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
                msockaddr_in addr=sessions->getAddrOnConnected(E->esi->m_id);

                sendEvent(l->id,new rpcEvent::IncomingOnConnector(E->esi,addr,e));
            }
            else
            {
                throw CommonError("!(r->type==Route::LOCALSERVICE %d) %s %s %s %s ",
                                  r->type,e->name,r->dump().c_str(),e->dump().toStyledString().c_str(),_DMI().c_str());
            }
        }
        else if(direction=='s')
        {
            MUTEX_INSPECTOR;
            SERVICE_id dst=buf.get_PN();
            REF_getter<Event::Base> e=iUtils->unpackEvent(buf);
            if(!e.valid())
            {
                return true;
            }

            e->route.push_front(new RemoteAddrRoute(E->esi->m_id));
            e->route.push_front(new LocalServiceRoute(dst));
            sendEvent(dst,new rpcEvent::IncomingOnConnector(E->esi,sessions->getAddrOnConnected(E->esi->m_id),e));
        }
    }
    catch(std::exception &e)
    {
        DBG(log(TRACE_log,"PKT RPC catched %s",e.what()));
        E->esi->close("broken packet int RPC");
    }
    return true;

}

void RPC::Service::addSendPacket(const int& channel, const REF_getter<Session>&S, const REF_getter<oscarEvent::SendPacket>&P)
{
    S_LOG("addSendPacket");
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

    
    XTRY;
    const rpcEvent::PassPacket *e=E;
    outBuffer o;
    o<<'p';
    iUtils->packEvent(o,e->e);

    REF_getter<Session> S=sessions->getSessionOrNull(e->socketIdTo);
    REF_getter<epoll_socket_info> esi=sessions->getOrNull(e->socketIdTo);
    if(!S.valid())
    {
        return true;
    }
    if(!esi.valid())
    {
        return true;
    }
    if(S.valid() && esi.valid())
    {
        S->update_last_time_hit();
        addSendPacket(e->e->rpcChannel,S,new oscarEvent::SendPacket(esi->m_id,o.asString(),serviceId));
    }
    else
    {
    }

    XPASS;
    return true;
}

bool RPC::Service::on_NotifyBindAddress(const oscarEvent::NotifyBindAddress*e)
{
    
    //if(iInstance->no_bind())
    //throw CommonError("Service::on_NotifyBindAddress %s",_DMI().c_str());
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
            log(ERROR_log,"getsockname: errno %d",errno);
            return true;
        }
        DBG(log(TRACE_log,"--------GETSOCKNAME m_bindAddr_reserve retursn %s",addr.dump().c_str()));


        DBG(logErr2("---------------e->sockType %s",e->socketDescription.c_str()));
        m_networkInitialized=true;
        doSendAll();

    }

    for(auto &i:m_bindAddr_main)
    {
        const msockaddr_in &addr=i;
        //logErr2("e->socketDescription %s",e->socketDescription.c_str());
        if(e->socketDescription=="RPC_UL")
        {
            if(e->rebind)
            {
                sessions->clear();
            }
            socklen_t len=addr.maxAddrLen();

            if(getsockname(CONTAINER(e->esi->get_fd()),addr.addr(),&len))
            {
                log(ERROR_log,"getsockname: errno %d",errno);
                return true;
            }
            //addr.setPort();

            DBG(log(TRACE_log,"--------GETSOCKNAME m_bindAddr_main retursn %s",addr.dump().c_str()));


            DBG(logErr2("---------------e->sockType %s",e->socketDescription.c_str()));
            m_networkInitialized=true;
            doSendAll();
        }
    }
    return true;
}

bool RPC::Service::on_startService(const systemEvent::startService* )
{
    XTRY;

    if(iUtils->isServiceRegistered(ServiceEnum::WebHandler))
    {
        sendEvent(ServiceEnum::WebHandler, new webHandlerEvent::RegisterHandler("RPC","RPC",ListenerBase::serviceId));
    }


    sendEvent(ServiceEnum::Timer,new timerEvent::SetTimer(timers::CONNECTION_ACTIVITY,NULL,NULL,m_iterateTimeout,ListenerBase::serviceId));

    //if(!iInstance->no_bind())
    {
        {

            for(auto &i:m_bindAddr_main)
            {
                auto item=i;
                {
                    SOCKET_id newid=iUtils->getSocketId();
                    //item.setSocketId(newid);
                    sendEvent(myOscar,new oscarEvent::AddToListenTCP(newid,item,"RPC_UL",ListenerBase::serviceId));
                }
            }

            for(auto &i:m_bindAddr_reserve)
            {
                auto item=i;
                {
                    SOCKET_id newid=iUtils->getSocketId();
                    //item.setSocketId(newid);
                    sendEvent(myOscar,new oscarEvent::AddToListenTCP(newid,item,"RPC_DL",ListenerBase::serviceId));
                }
            }
        }
    }

    XPASS;
    return true;
}
bool RPC::Service::on_Disaccept(const rpcEvent::Disaccept *e)
{
    sessions->on_delete(e->esi,"rpc:: on_Disaccept");
    return true;
}
bool RPC::Service::on_Disconnect(const rpcEvent::Disconnect *e)
{
    sessions->on_delete(e->esi,"rpc:: on_Disconnect");
    return true;
}
bool RPC::Service::on_Accepted(const oscarEvent::Accepted* E)
{
    S_LOG("on_Accepted");
    
    msockaddr_in sa;
    socklen_t sl=sa.maxAddrLen();
    if(getsockname(CONTAINER(E->esi->get_fd()), sa.addr(),&sl))
    {
        log(ERROR_log,"getsockname: errno %d",errno);
    }

    DBG(log(TRACE_log,"accepted socket %d",CONTAINER(E->esi->get_fd())));
    std::set<route_t> subscribers;
    {
        const oscarEvent::Accepted *e=E;
        {
            M_LOCK(sessions->m_lock);
            std::map<SOCKET_id,REF_getter<Session> >::iterator it=sessions->all.m_socketId2session.find(e->esi->m_id);
            if(it==sessions->all.m_socketId2session.end())
            {
                REF_getter<Session> S=new Session(e->esi->m_id);
                sessions->all.m_socketId2session.insert(std::make_pair(e->esi->m_id,S));
                sessions->add(ServiceEnum::RPC,e->esi);
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
    

    S_LOG("on_SendPacket");
    XTRY;
    //logErr2("on_SendPacket %s",E->ev->dump().c_str());
    REF_getter<Session> S(NULL);
    SOCKET_id sockId;
    CONTAINER(sockId)=0L;
    bool found=false;

    {
        
        M_LOCK(sessions->m_lock);
        std::map<msockaddr_in,SOCKET_id >::iterator it=sessions->connector.m_sa2socketId.find(E->addressTo);
        if(it!=sessions->connector.m_sa2socketId.end())
        {
            sockId=it->second;
            found=true;

            if(!sessions->connector.m_socketId2sa.count(sockId))
            {
                found=false;
            }
        }
        if(found)
        {
            std::map<SOCKET_id, REF_getter<Session> >::iterator its=sessions->all.m_socketId2session.find(sockId);
            if(its==sessions->all.m_socketId2session.end())
            {
                found=false;
            }
        }
    }
    if(found)
    {
        
        XTRY;
        REF_getter<epoll_socket_info> esi=sessions->getOrNull(sockId);
        if(!esi.valid())
        {
            found=false;
        }
        XPASS;
    }

    if(!found)
    {
        
        XTRY;
        M_LOCK(sessions->m_lock);

        sockId=iUtils->getSocketId();
        msockaddr_in addressTo=E->addressTo;

        //addressTo.setSocketId(sockId);
        sessions->connector.m_socketId2sa[sockId]=addressTo;
        sessions->connector.m_sa2socketId[addressTo]=sockId;
        S=new Session(sockId);

        sessions->all.m_socketId2session.erase(sockId);
        sessions->all.m_socketId2session.insert(std::make_pair(sockId,S));
        {
            M_UNLOCK(sessions->m_lock);
            sendEvent(myOscar,new oscarEvent::Connect(sockId,addressTo,"RPC",ListenerBase::serviceId));
        }
        XPASS;

    }
    else
    {
        
        XTRY;
        M_LOCK(sessions->m_lock);
        std::map<SOCKET_id, REF_getter<Session> >::iterator its=sessions->all.m_socketId2session.find(sockId);
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
                    std::map<int,std::deque<REF_getter<oscarEvent::SendPacket> > >::iterator i=S->m_OutEventCache.begin();
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
            DBG(log(TRACE_log,"!S->m_connectionEstablished"));
#endif
        }

    }
    else
    {
        log(ERROR_log,"incorrect: if(S.valid()) %s %d %s",__FILE__,__LINE__,_DMI().c_str());
    }
}

bool RPC::Service::on_NotifyOutBufferEmpty(const oscarEvent::NotifyOutBufferEmpty* e)
{
    

    //logErr2("@%s",__PRETTY_FUNCTION__);
    S_LOG("on_NotifyOutBufferEmpty");
    //if(!S->m_connectionEstablished)
    {
        // on_Connected()
    }
    std::set<route_t> subscribers;
    {
        // MUTEX_INSPECTOR;
        M_LOCK(sessions->m_lock);
        std::map<SOCKET_id,REF_getter<Session> >::iterator it=sessions->all.m_socketId2session.find(e->esi->m_id);
        if(it==sessions->all.m_socketId2session.end())
        {
            REF_getter<Session> S=new Session(e->esi->m_id);
            sessions->all.m_socketId2session.insert(std::make_pair(e->esi->m_id,S));
            sessions->add(ServiceEnum::RPC,e->esi);
            S->m_connectionEstablished=true;
            S->update_last_time_hit();
            subscribers=sessions->all.m_subscribers;
        }

    }
    for(auto &i:subscribers)
    {
        //REF_getter<Event::Base>
        passEvent(new rpcEvent::Accepted(e->esi,poppedFrontRoute(i)));

    }
    REF_getter<Session> S=sessions->getSessionOrNull(e->esi->m_id);
    if(S.valid())
    {
        doSend(S);
    }
    else
    {

        //e->esi->__jdump().toStyledString();
        DBG(log(TRACE_log,"!S.valid sock %s in session %s %d",e->esi->__jdump().toStyledString().c_str(),__FILE__,__LINE__));

    }
    return true;
}


bool RPC::Service::on_SocketClosed(const oscarEvent::SocketClosed*e)
{
    S_LOG("on_SocketClosed");
    REF_getter<Session> S=sessions->getSessionOrNull(e->socketId);
    if(S.valid())
    {
        DBG(log(TRACE_log,"delete orhaned rpc session %d",CONTAINER(e->socketId)));
        sessions->remove(e->socketId,"unknown oscar session");
    }
    else
    {
        DBG(logErr2("fail to remove orphaned rpc session"));
    }

    return true;
}

UnknownBase* RPC::Service::construct(const SERVICE_id& id, const std::string&  nm,IInstance* ifa)
{
    XTRY;
    return new Service(id,nm,ifa);
    XPASS;
}
int64_t RPC::Service::getTotalSendBufferSize()
{
    M_LOCK(mx);
    return mx.totalSendBufferSize;
}

void RPC::Service::directHandleEvent(const REF_getter<Event::Base>&e)
{
    

    processEvent(e);
}

bool RPC::Service::on_UnsubscribeNotifications(const rpcEvent::UnsubscribeNotifications* E)
{
    M_LOCK(sessions->m_lock);
    sessions->all.m_subscribers.erase(E->route);
    return true;

}

bool RPC::Service::on_SubscribeNotifications(const rpcEvent::SubscribeNotifications* E)
{
    M_LOCK(sessions->m_lock);
    sessions->all.m_subscribers.insert(E->route);
    return true;
}


void registerRPCService(const char* pn)
{
    if(pn)
    {
        iUtils->registerPlugingInfo(COREVERSION,pn,IUtils::PLUGIN_TYPE_SERVICE,ServiceEnum::RPC,"RPC");
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
    //logErr2("void Service::upnp_disablePortmapping()");
    //if(iInstance->no_bind())
    //  throw CommonError("no bind and void Service::upnp_pulse() %s %d",__FILE__,__LINE__);

    //
    if(m_bindAddr_main.size())
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
    if(m_bindAddr_main.size())
    {
//#ifndef __MACH__
#ifdef __MOBILE__
        throw CommonError("call upnp_enablePortmapping in mobile");
#else
        M_LOCK(upnp.upnpMX);
        DBG(log(TRACE_log,"void Service::upnp_pulse()"));
        if(!upnp.upnp) upnp.upnp=tr_upnpInit();
        int res=tr_upnpPulse(upnp.upnp,m_bindAddr_main.begin()->port(),true,true);
        DBG(log(TRACE_log,"tr_upnpPulse res %d",res));
        if(/*res!=0 && */strlen(tr_lanaddr))
        {
            M_LOCK(mintAddr);
            msockaddr_in sa;
            DBG(log(TRACE_log,"tr_lanaddr %s %d",tr_lanaddr,tr_port));
            sa.init(tr_lanaddr,tr_port);
            mintAddr.m_internalAddr.insert(sa);
            DBG(log(TRACE_log,"------mintAddr.m_internalAddr %s",iUtils->dump(mintAddr.m_internalAddr).c_str()));
        }
#endif
    }

}
#endif
bool RPC::Service::on_TickAlarm(const timerEvent::TickAlarm*e)
{
    switch(e->tid)
    {
    case timers::CONNECTION_ACTIVITY:
    {
        inBuffer in(e->data);
        SOCKET_id sid;
        in>>sid;
        std::map<SOCKET_id, REF_getter<Session> > ss=sessions->getSessionContainer();
        time_t now=time(NULL);
        for(std::map<SOCKET_id, REF_getter<Session> >::iterator i=ss.begin(); i!=ss.end(); i++)
        {
            REF_getter<Session> S=i->second;
            REF_getter<epoll_socket_info> esi=sessions->getOrNull(i->first);
            if(S.valid() && esi.valid())
            {
                if(now-S->last_time_hit>m_connectionActivityTimeout)
                {
                    DBG(log(TRACE_log,"close socket S->esi-> %s",esi->remote_name.dump().c_str()));
                    esi->close("rpc close due timer inactivity");
                }
            }
            else logErr2("--------------- invalid behaviour if(S.valid() && esi.valid()) %s %d",__FILE__,__LINE__);
        }
        DBG(log(ERROR_log,"srabotal timers::CONNECTION_ACTIVITY on socket %d (%s %d) ",CONTAINER(sid),__FILE__,__LINE__));
    }
    break;
    default:
        throw CommonError("invalid timer id %s %d",__FILE__,__LINE__);
    }
    return true;
}
Json::Value RPC::__sessions::jdump()
{
    Json::Value v;
    std::map<SOCKET_id, REF_getter<Session> >m;
    std::set<route_t> s;
    {
        M_LOCK(m_lock);
        m=all.m_socketId2session;
        s=all.m_subscribers;
    }
    v["sessions.size"]=iUtils->toString(m.size());
    v["subscribers.size"]=iUtils->toString(s.size());
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
    while(!m_networkInitialized && m_bindAddr_main.size())
    {
        if(m_isTerminating) return 0;
        //if(iInstance->isTerminating()) return 0;
        usleep(10000);
        if(m_isTerminating) return 0;
        //if(iInstance->isTerminating()) return 0;
    }
    if(!m_bindAddr_main.size())
    {
        //logErr2("if(!m_bindAddr_main.second)");
        return 0;
    }
    return m_bindAddr_main.begin()->port();
}
std::set<msockaddr_in> RPC::Service::getInternalListenAddrs()
{
    while(!m_networkInitialized && m_bindAddr_main.size())
    {
        usleep(10000);
    }
    M_LOCK(mintAddr);
    return mintAddr.m_internalAddr;

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
    DBG(log(TRACE_log,"--------------before upnp_pulse();"));
#if !defined(WITHOUT_UPNP)
    upnp_enablePortmapping();
#endif
    DBG(log(TRACE_log,"--------------upnp_pulse DONE"));
    route_t r=e->route;
    r.pop_front();

    passEvent(new rpcEvent::UpnpResult(e->cookie,r));

    return true;
}
#endif
bool RPC::Service::on_ConnectFailed(const oscarEvent::ConnectFailed*e)
{

    std::set<route_t> subscribers;
    {
        M_LOCK(sessions->m_lock);
        subscribers=sessions->all.m_subscribers;
    }

    for(auto &i:subscribers)
    {
        route_t r=i;
        r.pop_front();

        iInstance->passEvent(new rpcEvent::ConnectFailed(e->addr,r));
    }
    return true;
}

void RPC::__sessions::on_delete(const REF_getter<epoll_socket_info>&esi, const std::string& reason)
{
    std::set<route_t> subscribers;
    {
        M_LOCK(m_lock);
        subscribers=all.m_subscribers;
    }

    for(auto &i:subscribers)
    {
        route_t r=i;
        r.pop_front();
        if(esi->m_streamType==epoll_socket_info::_CONNECTED)
        {
            msockaddr_in destination_addr=getAddrOnConnected(esi->m_id);
            if(esi->inConnection)
            {
                throw CommonError("invalid USECASE with connect failed "+_DMI());

            }
            else
            {
                DBG(logErr2("-------------DISCONNECTED SUCCESSED"));
                passEvent(new rpcEvent::Disconnected(esi,destination_addr,reason,r));
            }
        }
        else if(esi->m_streamType==epoll_socket_info::_ACCEPTED)
        {
            passEvent(new rpcEvent::Disaccepted(esi,reason,r));
        }
        else throw CommonError("invalid esi type %s %d",__FILE__,__LINE__);
    }
    {
        M_LOCK(m_lock);
        std::map<SOCKET_id,msockaddr_in>::iterator i=connector.m_socketId2sa.find(esi->m_id);
        if(i!=connector.m_socketId2sa.end())
        {
            connector.m_sa2socketId.erase(i->second);
            connector.m_socketId2sa.erase(i->first);
        }
        if(all.m_socketId2session.count(esi->m_id))
        {
            all.m_socketId2session.erase(esi->m_id);
        }
        else
        {
        }
    }

}
bool RPC::Service::handleEvent(const REF_getter<Event::Base>& e)
{
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

    if( oscarEventEnum::SocketClosed==ID)
        return(this->on_SocketClosed((const oscarEvent::SocketClosed*)e.operator->()));

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
    if( rpcEventEnum::Disconnect==ID)
        return(this->on_Disconnect((const rpcEvent::Disconnect*)e.operator->()));
    if( rpcEventEnum::Disaccept==ID)
        return(this->on_Disaccept((const rpcEvent::Disaccept*)e.operator->()));
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

    HTTP::Response cc;
    cc.content+="<h1>RPC report</h1><p>";

    Json::Value v=jdump();
    Json::StyledWriter w;
    cc.content+="<pre>\n"+w.write(v)+"\n</pre>";

    cc.makeResponse(e->esi);
    return true;
}
