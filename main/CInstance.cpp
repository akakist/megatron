#include "CInstance.h"

#include "objectHandler.h"
#include "url.h"



#include "Events/System/Run/startService.h"
#include "Events/System/Net/rpc/PassPacket.h"
#include "Events/System/Net/rpc/SendPacket.h"
#include "Events/DFS/Referrer/___dfsReferrerEvent.h"
#include "utils_local.h"


#ifdef QT_CORE_LIB
#include <QStandardPaths>
#endif

#ifndef _MSC_VER
extern int h_errno;
#endif

CInstance::CInstance(IUtils *_m_utils
                    )

    :config_z(NULL),
     m_utils(_m_utils)
     //,
//     m_terminatingFlag(false)

{

}





void CInstance::passEvent(const REF_getter<Event::Base>&e)
{
    MUTEX_INSPECTOR;
    //if(isTerminating()) return ;
    XTRY;
    //logErr2("e->route.size %d",e->route.size());
    REF_getter<Route> r=e->route.pop_front();
    //logErr2("after REF_getter<Route> r=e->route.pop_front();");
    switch(r->type)
    {
    case Route::LOCALSERVICE:
    {
        MUTEX_INSPECTOR;
        LocalServiceRoute* rt=(LocalServiceRoute* )r.operator ->();
        forwardEvent(rt->id,e);
    }
    break;
    case Route::OBJECTHANDLER_POLLED:
    {
        MUTEX_INSPECTOR;
        e->route.push_front(r);
        forwardEvent(ServiceEnum::ObjectProxyPolled,e);
        break;
    }
    case Route::OBJECTHANDLER_THREADED:
    {
        MUTEX_INSPECTOR;
        e->route.push_front(r);
        forwardEvent(ServiceEnum::ObjectProxyThreaded,e);
        break;
    }
    case Route::REMOTEADDR:
    {
        MUTEX_INSPECTOR;

        XTRY;
        RemoteAddrRoute* rt=(RemoteAddrRoute* )r.operator ->();
        forwardEvent(ServiceEnum::RPC,new rpcEvent::PassPacket(rt->addr,e));
        XPASS;

    }
    break;
    default:
        throw CommonError("passEvent: invalid route type %d",r->type);
    }
    XPASS;

}
void CInstance::sendEvent(const SERVICE_id& svs,  const REF_getter<Event::Base>&e)
{

    //if(isTerminating()) return ;
    XTRY;
    e->route.push_front(new LocalServiceRoute(svs));
    forwardEvent(svs,e);
    XPASS;
}
void CInstance::sendEvent(const std::string& dstHost, const SERVICE_id& dstService,  const REF_getter<Event::Base>&e)
{
    //if(isTerminating()) return ;
    XTRY;
    if(dstHost=="local")
    {
        sendEvent(dstService,e);
    }
    else
    {
        try {
            Url u;
            u.parse(dstHost);

            msockaddr_in sa;
            sa.init(u);
            sendEvent(sa,dstService,e);
        }
        catch(std::exception& ex)
        {
            logErr2("exception: %s",ex.what());
        }
    }

    XPASS;
}
void CInstance::sendEvent(const msockaddr_in& addr, const SERVICE_id& svs,  const REF_getter<Event::Base>&e)
{
    //if(isTerminating()) return ;
    XTRY;
    forwardEvent(ServiceEnum::RPC,new rpcEvent::SendPacket(addr,svs,e));
    XPASS;
}

void CInstance::sendEvent(ListenerBase *l,  const REF_getter<Event::Base>&e)
{
    l->listenToEvent(e);
}
UnknownBase * CInstance::getServiceNoCreate(const SERVICE_id& svs)
{
    M_LOCK(services);
    std::map<SERVICE_id,UnknownBase* > &c=services.container;
    std::map<SERVICE_id,UnknownBase* > ::iterator i=c.find(svs);
    UnknownBase *u=NULL;
    if(i==c.end())
    {
        u=NULL;
    }
    else
    {
        u=i->second;
    }
    return u;
}

UnknownBase * CInstance::getServiceOrCreate(const SERVICE_id& svs)
{

    //if(isTerminating()) return NULL;
    XTRY;
    UnknownBase *u=NULL;
    {
        XTRY;
        M_LOCK(services);
        std::map<SERVICE_id,UnknownBase* > &c=services.container;
        std::map<SERVICE_id,UnknownBase* >::iterator i=c.find(svs);
        if(i==c.end())
        {
            u=NULL;
        }
        else
        {
            u=i->second;
        }
        XPASS;
    }
    XTRY;
    if(!u)
    {
        u=initService(svs);
    }
    XPASS;
    XTRY;
    if(u)
    {
        return u;
    }
    XPASS;
    XPASS;
    throw CommonError("cannot find service %s",svs.dump().c_str());

}

void CInstance::forwardEvent(const SERVICE_id& svs,  const REF_getter<Event::Base>&e)
{
    MUTEX_INSPECTOR;
    //if(isTerminating()) return ;

    XTRY;
    try
    {
        MUTEX_INSPECTOR;
        UnknownBase *u=NULL;
        {
            MUTEX_INSPECTOR;
            XTRY;
            M_LOCK(services);
            std::map<SERVICE_id,UnknownBase* >  &c=services.container;
            std::map<SERVICE_id,UnknownBase* >::iterator i=c.find(svs);
            if(i==c.end())
            {
                u=NULL;
            }
            else
            {
                u=i->second;
            }
            XPASS;
        }
        if(!u)
        {
            MUTEX_INSPECTOR;
            XTRY;
            u=initService(svs);

            if(!u)
                throw CommonError("Service '%s' not registered",iUtils->serviceName(svs).c_str());
            XPASS;
        }
        if(u)
        {
            MUTEX_INSPECTOR;
            XTRY;
            ((ListenerBase*) u->cast(UnknownCast::listener))->listenToEvent(e);
            XPASS;
        }
        else throw CommonError("!U ------------- %s %d",__FILE__,__LINE__);
    }
    catch(std::exception &err)
    {
        logErr2("catched for event %s %s %s",e->name,e->route.dump().c_str(),err.what());
    }

    XPASS;
}

UnknownBase* CInstance::initService(const SERVICE_id& sid)
{
    //  MUTEX_INSPECTOR;
    //if(isTerminating()) return NULL;
    auto locals=m_utils->getLocals();
    Mutex *initMX=NULL;
    {
        M_LOCK(initService_MX);
        if(!mx_initInProcess.count(sid))
            mx_initInProcess[sid]=new Mutex;
        initMX=mx_initInProcess[sid];
    }

    M_LOCK(initMX);


    XTRY;
    UnknownBase *u=NULL;

    unknown_static_constructor constr=0;

    {
        std::map<SERVICE_id,UnknownBase* >::iterator i=services.container.find(sid);
        if(i!=services.container.end())
            return i->second;
    }
    {
        M_LOCK(locals->service_constructors);
        std::map<SERVICE_id,unknown_static_constructor >::const_iterator i=locals->service_constructors.container.find(sid);
        if(i==locals->service_constructors.container.end())
        {
            DBG(logErr2("cannot find service registered constructor ServiceId %s",sid.dump().c_str()));
        }
        else
        {
            constr=i->second;
        }
    }
    if(constr==0)
    {
        std::string pn;
        {
            M_LOCK(locals->pluginInfo);
            std::map<SERVICE_id,std::string> ::iterator i=locals->pluginInfo.services.find(sid);
            if(i!=locals->pluginInfo.services.end())
            {
                pn=i->second;
            }
            else
            {
                XTRY;
                {
                    M_UNLOCK(locals->pluginInfo);
                    for(auto &z: locals->pluginInfo.services)
                    {
                        logErr2("have: %s -> %s",z.first.dump().c_str(),z.second.c_str());
                    }
                }

                throw CommonError("cannot find plugin for service %s",iUtils->serviceName(sid).c_str());
                XPASS;
            }
        }
        m_utils->registerPluginDLL(pn);
        {
            //    MUTEX_INSPECTOR;
            M_LOCK(locals->service_constructors);
            std::map<SERVICE_id,unknown_static_constructor >::const_iterator i=locals->service_constructors.container.find(sid);
            if(i==locals->service_constructors.container.end())
            {
                XTRY;
                M_UNLOCK(locals->service_constructors);
                throw CommonError("cannot load service ServiceId %s",sid.dump().c_str());
                XPASS;
            }
            else
            {
                constr=i->second;
            }
        }
    }

    {
        //   MUTEX_INSPECTOR;
        M_LOCK(services);
        auto iii=services.container.find(sid);
        if(iii!=services.container.end())
        {
            return iii->second;
        }
        else
        {
            {
                M_UNLOCK(services);
                std::string name;
                {
                    name=iUtils->serviceName(sid);
                }
                DBG(logErr2("running service %s",name.c_str()));
                CFG_PUSH(name,config_z);
                {
                    XTRY;
                    if(constr==0) {
                        XTRY;
                        throw CommonError("if(constr==0)");
                        XPASS;
                    }
                    XTRY;
//                    printf("@@@ constr %s\n",name.c_str());
                    u=constr(sid,name.c_str(),this);
                    XPASS;
                    XPASS;
                }
            }
            {
                services.container.insert(std::make_pair(sid,u));
            }
        }
    }
    try {
        ((ListenerBase*) u->cast(UnknownCast::listener))->listenToEvent(new systemEvent::startService);
    }
    catch(std::exception &e)
    {
        logErr2("startService %s",e.what());
        return NULL;
    }

    return u;
    XPASS;

}
void CInstance::initServices()
{
    // MUTEX_INSPECTOR;
    XTRY;
    std::vector<std::string> svs;
#if !defined __CLI__
    svs.push_back("RPC");
#endif
#ifndef QT_CORE_LIB
#if !defined __MOBILE__
#if !defined __CLI__
    //svs.push_back("DFSReferrer");
#endif

#endif
#endif

    std::set<std::string> run=config_z->get_stringset("Start",iUtils->join(",",svs),"list of initially started services");
    for(std::set<std::string>::iterator i=run.begin(); i!=run.end(); ++i)
    {
        getServiceOrCreate(iUtils->serviceIdByName(*i));
    }

    XPASS;
}
void CInstance::deinitServices()
{
    auto locals=m_utils->getLocals();
    std::map<SERVICE_id,UnknownBase* >svs;
    {
        M_LOCK(services);
        svs=services.container;
        services.container.clear();
    }
    for(std::map<SERVICE_id,UnknownBase* >::iterator i=svs.begin(); i!=svs.end(); i++)
    {
        UnknownBase* u=i->second;
        logErr2("deleting service %s",u->classname.c_str());
        try {
            delete u;
        }
        catch(...)
        {

        }
    }
    {
        M_LOCK(locals->event_constructors);
        locals->event_constructors.container.clear();
    }
    //m_utils->m_terminatedServices=true;
}




std::vector<SERVICE_id> CInstance::getAllRunningServices()
{
    XTRY;
    std::vector<SERVICE_id> v;
    {
        M_LOCK(services);
        for(std::map<SERVICE_id,UnknownBase* >::iterator i=services.container.begin(); i!=services.container.end(); i++)
            v.push_back(i->first);
    }
    return v;
    XPASS;
}


GlobalCookie_id CInstance::globalCookie()
{
    M_LOCK(mx_globalCookie);
    if(CONTAINER(m_globalCookie).size()==0)
    {
        I_ssl *ssl=(I_ssl*)iUtils->queryIface(Ifaces::SSL);
        CONTAINER(m_globalCookie)=ssl->rand_bytes(8);

    }
    return m_globalCookie;
}
