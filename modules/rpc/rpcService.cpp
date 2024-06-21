#include "mutexInspector.h"
#include <unistd.h>
#define STATICLIB 1

#include "rpcService.h"

#include "version_mega.h"
//#include "event.h"
#include <Events/System/Net/rpcEvent.h>
#include "Events/System/Net/oscarEvent.h"
#include "Events/System/timerEvent.h"

#include "logging.h"
#include "tools_mt.h"
#include "events_rpc.hpp"
RPC::Service::Service(const SERVICE_id &svs, const std::string&  nm, IInstance* ifa):
    UnknownBase(nm),
    ListenerSimple(nm,ifa->getConfig(),svs),
    Broadcaster(ifa),
    myOscar(ServiceEnum::Oscar),
    iterateTimeout_(ifa->getConfig()->get_real("IterateTimeoutSec",60,"")),
    m_connectionActivityTimeout(ifa->getConfig()->get_real("ConnectionActivity",600.0,"")),
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
//                M_LOCK(sharedAddr);
                m_bindAddr_main=ifa->getConfig()->get_tcpaddr("BindAddr_MAIN",_main,"Address used to work with dfs network RPC_UL. NONE - no bind");
                m_bindAddr_reserve=ifa->getConfig()->get_tcpaddr("BindAddr_RESERVE",_reserve,"Address used to communicate with local apps RPC_DL, must be fixed. NONE - no bind");
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
    sessions.clear();

}

bool RPC::Service::on_Connected(const oscarEvent::Connected* E)
{
    MUTEX_INSPECTOR;

    REF_getter<Session> S(NULL);
    msockaddr_in addr;
    std::set<route_t> subscribers;
    {
        RLocker lk(sessions.sa2sess_.lock_);
        if(!E->esi->request_for_connect_.has_value())
            throw CommonError("if(!E->esi->request_for_connect.has_value())");
        auto it=sessions.sa2sess_.container_.find(*E->esi->request_for_connect_);
        if(it==sessions.sa2sess_.container_.end())
        {
            throw CommonError("2cannot find RPC session on connected %s",E->esi->request_for_connect_->dump().c_str());
            return true;
        }
        S=it->second;
    }
    {
        RLocker l(sessions.subscribers_.lock_);
        subscribers=sessions.subscribers_.container_;


    }


    for(auto &i:subscribers)
    {
        passEvent(new rpcEvent::Connected(E->esi,addr,i));
    }

    if(!S.valid()) throw CommonError("!!!!! FATAL invalid S %s %d",__FILE__,__LINE__);

    S->esi=E->esi;
    flushOutCache(S);
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
                LocalServiceRoute* l=(LocalServiceRoute*) r.get();
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

            e->route.push_front(new RemoteAddrRoute(E->esi->id_));
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
                LocalServiceRoute* l=(LocalServiceRoute*) r.get();
                if(!E->esi->request_for_connect_.has_value())
                    throw CommonError("if(!E->esi->request_for_connect.valid())");

                sendEvent(l->id,new rpcEvent::IncomingOnConnector(E->esi,*E->esi->request_for_connect_,e));
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

            e->route.push_front(new RemoteAddrRoute(E->esi->id_));
            e->route.push_front(new LocalServiceRoute(dst));
            if(!E->esi->request_for_connect_.has_value())
                throw CommonError("if(!E->esi->request_for_connect.valid())");
            {
                sendEvent(dst,new rpcEvent::IncomingOnConnector(E->esi,*E->esi->request_for_connect_,e));
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
void RPC::Service::flushOutCache(const REF_getter<Session> & S)
{
    if(S->outCache_.valid())
    {
        std::deque<REF_getter<refbuffer> > d;
        {
            WLocker l(S->outCache_->lk);
            d=std::move(S->outCache_->container);
        }
        S->outCache_=nullptr;
        for(auto &p:d)
        {
            sendEvent(myOscarListener,new oscarEvent::SendPacket(S->esi,p,dynamic_cast<ListenerBase*>(this)));
        }
    }

}

void RPC::Service::addSendPacket(const REF_getter<Session>&S, const REF_getter<refbuffer>&P)
{
    MUTEX_INSPECTOR;
    S_LOG("addSendPacket");
    bool need_cache=false;

    if(!S.valid())
        need_cache=true;
    if(!need_cache && !S->esi.valid())
        need_cache=true;
    if(need_cache)
    {
        if(!S->outCache_.valid())
            S->outCache_=new outCache;
        WLocker l(S->outCache_->lk);
        S->outCache_->container.push_back(P);
        return;
    }
    else
    {
        flushOutCache(S);
        sendEvent(myOscarListener,new oscarEvent::SendPacket(S->esi,P,dynamic_cast<ListenerBase*>(this)));
    }
}
bool RPC::Service::on_PassPacket(const rpcEvent::PassPacket* E)
{

    MUTEX_INSPECTOR;

    XTRY;
    if(!E)
        return false;
    outBuffer o;
    o<<'p';
    iUtils->packEvent(o,E->e);

    REF_getter<Session> S(NULL);
    {
        RLocker lk(sessions.sock2sess_.lock_);

        auto it=sessions.sock2sess_.container_.find(E->socketIdTo);
        if(it!=sessions.sock2sess_.container_.end())
        {
            S=it->second;
        }
    }

    if(S.valid())
    {
        addSendPacket(S,o.asString());
    }
    else{
        WLocker (sessions.passCache_.lock_);
        sessions.passCache_.passCache[E->socketIdTo].push_back(o.asString());
    }

    XPASS;
    return true;
}

bool RPC::Service::on_NotifyBindAddress(const oscarEvent::NotifyBindAddress*e)
{
    MUTEX_INSPECTOR;

    {
        WLocker l(sharedAddr.lk);
        sharedAddr.m_bindAddr_mainSH.insert(e->addr);
    }
    flushAll();
    return true;
}

bool RPC::Service::on_startService(const systemEvent::startService* )
{
    MUTEX_INSPECTOR;
    XTRY;

    myOscarListener=dynamic_cast<ListenerBase*>(iInstance->getServiceOrCreate(Service::myOscar));


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

    XPASS;
    return true;
}
bool RPC::Service::on_Accepted(const oscarEvent::Accepted* E)
{
    MUTEX_INSPECTOR;
    S_LOG("on_Accepted");


    std::set<route_t> subscribers;
    {
        {
            REF_getter<Session> S=new Session(E->esi->id_,E->esi);
            std::deque<REF_getter<refbuffer>> d;
            {
                WLocker lk(sessions.sock2sess_.lock_);

                sessions.sock2sess_.container_.emplace(E->esi->id_,S);
            }
            {
                RLocker l(sessions.subscribers_.lock_);
                subscribers=sessions.subscribers_.container_;
            }
            {
                WLocker l(sessions.passCache_.lock_);
                d=std::move(sessions.passCache_.passCache[E->esi->id_]);
                sessions.passCache_.passCache.erase(E->esi->id_);
            }

            for(auto &dd: d)
            {
              addSendPacket(S,dd);
            }

        }
    }

    for(auto &i:subscribers)
    {
        passEvent(new rpcEvent::Accepted(E->esi,i));

    }


    return true;
}

bool RPC::Service::on_SendPacket(const rpcEvent::SendPacket* E)
{

    MUTEX_INSPECTOR;
    S_LOG("on_SendPacket");
    XTRY;
    bool found=false;

    REF_getter<Session> S(NULL);
    {
        {
            RLocker lk(sessions.sa2sess_.lock_);
            auto it=sessions.sa2sess_.container_.find(E->addressTo);
            if(it!=sessions.sa2sess_.container_.end())
                S=it->second;
        }

        if(!S.valid())
        {
            SOCKET_id sockId=iUtils->getSocketId();
            msockaddr_in addressTo=E->addressTo;
            S=new Session(sockId,NULL);
            {
                WLocker lk(sessions.sa2sess_.lock_);
                sessions.sa2sess_.container_.emplace(E->addressTo,S);
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
    WLocker lk(sessions.subscribers_.lock_);

    sessions.subscribers_.container_.erase(E->route);
    return true;

}

bool RPC::Service::on_SubscribeNotifications(const rpcEvent::SubscribeNotifications* E)
{
    WLocker lk(sessions.subscribers_.lock_);
    sessions.subscribers_.container_.insert(poppedFrontRoute(E->route));
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

unsigned short RPC::Service::getExternalListenPortMain()
{
    MUTEX_INSPECTOR;
    bool cond=false;
    {
        RLocker l(sharedAddr.lk);
        cond=sharedAddr.m_bindAddr_mainSH.empty();
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
            RLocker(sharedAddr.lk);
            cond=/*!sharedAddr.m_networkInitialized &&*/ sharedAddr.m_bindAddr_mainSH.empty();

        }

    }
    {
        RLocker l(sharedAddr.lk);
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
        RLocker l(sharedAddr.lk);
        cond= sharedAddr.m_bindAddr_mainSH.empty();
    }
    while(cond)
    {
        usleep(10000);
        {
            RLocker l(sharedAddr.lk);
            cond= sharedAddr.m_bindAddr_mainSH.empty();
        }
    }
    RLocker l(sharedAddr.lk);
    return sharedAddr.m_internalAddr;

}
void RPC::Service::flushAll()
{
    std::set<REF_getter<Session> > s;
    {
        RLocker lk(sessions.sock2sess_.lock_);
        for(auto& z: sessions.sock2sess_.container_)
            s.insert(z.second);

    }
    {
        RLocker l(sessions.sa2sess_.lock_);
        for(auto& z: sessions.sa2sess_.container_)
            s.insert(z.second);
    }
    for(auto &i:s)
    {

        flushOutCache(i);
    }

}
bool RPC::Service::on_ConnectFailed(const oscarEvent::ConnectFailed*e)
{

    MUTEX_INSPECTOR;
    std::set<route_t> subscribers;
    if(!e->esi->request_for_connect_.has_value())
        throw CommonError("if(!e->esi->request_for_connect.valid())");

    {
        WLocker lk(sessions.subscribers_.lock_);
        subscribers=sessions.subscribers_.container_;
    }
    {
        WLocker l(sessions.sa2sess_.lock_);
        sessions.sa2sess_.container_.erase(*e->esi->request_for_connect_);
    }

    for(auto &i:subscribers)
    {
        passEvent(new rpcEvent::ConnectFailed(e->addr,i));
    }
    return true;
}
bool RPC::Service::on_Disconnected(const oscarEvent::Disconnected* e)
{
    MUTEX_INSPECTOR;
    std::set<route_t> subscribers;
    {
        RLocker lk(sessions.subscribers_.lock_);
        subscribers=sessions.subscribers_.container_;
    }


    for(auto &i:subscribers)
    {

        DBG(logErr2("-------------DISCONNECTED SUCCESSED"));
        if(!e->esi->request_for_connect_.has_value())
            throw CommonError("if(!e->esi->request_for_connect.valid())");
        passEvent(new rpcEvent::Disconnected(e->esi,*e->esi->request_for_connect_,e->reason,i));
    }
    {
        WLocker lk(sessions.sa2sess_.lock_);

        sessions.sa2sess_.container_.erase(*e->esi->request_for_connect_);
    }

    return true;
}
bool RPC::Service::on_Disaccepted(const oscarEvent::Disaccepted*e)
{
    MUTEX_INSPECTOR;

    std::set<route_t> subscribers;
    {
        RLocker lk(sessions.subscribers_.lock_);

        subscribers=sessions.subscribers_.container_;
    }

    for(auto &i:subscribers)
    {
        passEvent(new rpcEvent::Disaccepted(e->esi,e->reason,i));
    }
    {
        WLocker lk(sessions.sock2sess_.lock_);
        sessions.sock2sess_.container_.erase(e->esi->id_);
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
        return on_TickAlarm((const timerEvent::TickAlarm*)e.get());
    if(timerEventEnum::TickTimer==ID)
        return true;
    if( oscarEventEnum::PacketOnAcceptor==ID)
        return(this->on_PacketOnAcceptor((const oscarEvent::PacketOnAcceptor*)e.get()));

    if( oscarEventEnum::PacketOnConnector==ID)
        return(this->on_PacketOnConnector((const oscarEvent::PacketOnConnector*)e.get()));

    if( oscarEventEnum::Connected==ID)
        return(this->on_Connected((const oscarEvent::Connected*)e.get()));

    if( oscarEventEnum::Accepted==ID)
        return(this->on_Accepted((const oscarEvent::Accepted*)e.get()));

    if( oscarEventEnum::NotifyBindAddress==ID)
        return(this->on_NotifyBindAddress((const oscarEvent::NotifyBindAddress*)e.get()));

    if( oscarEventEnum::NotifyOutBufferEmpty==ID)
        return(this->on_NotifyOutBufferEmpty((const oscarEvent::NotifyOutBufferEmpty*)e.get()));


    if( oscarEventEnum::ConnectFailed==ID)
        return(this->on_ConnectFailed((const oscarEvent::ConnectFailed*)e.get()));

    if(systemEventEnum::startService==ID)
        return on_startService((const systemEvent::startService*)e.get());

    if( rpcEventEnum::PassPacket==ID)
        return(this->on_PassPacket((const rpcEvent::PassPacket*)e.get()));
    if( rpcEventEnum::SendPacket==ID)
        return(this->on_SendPacket((const rpcEvent::SendPacket*)e.get()));
    if( rpcEventEnum::SubscribeNotifications==ID)
        return(this->on_SubscribeNotifications((const rpcEvent::SubscribeNotifications*)e.get()));
    if( rpcEventEnum::UnsubscribeNotifications==ID)
        return(this->on_UnsubscribeNotifications((const rpcEvent::UnsubscribeNotifications*)e.get()));
    if( oscarEventEnum::Disconnected==ID)
        return(this->on_Disconnected((const oscarEvent::Disconnected*)e.get()));
    if( oscarEventEnum::Disaccepted==ID)
        return(this->on_Disaccepted((const oscarEvent::Disaccepted*)e.get()));


    XPASS;
    return false;

}
