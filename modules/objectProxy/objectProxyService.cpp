#include "objectProxyService.h"
#ifndef _MSC_VER
#endif

#include "objectHandler.h"



#include "Events/System/Net/rpc/IncomingOnAcceptor.h"
#include "Events/System/Net/rpc/IncomingOnConnector.h"
#include "events_ObjectProxy.hpp"

bool ObjectProxy::Threaded::handleEvent(const REF_getter<Event::Base>& e)
{
    XTRY;
    
    //logErr2("ObjectProxy::Threaded::handleEvent %s",e->dump().c_str());
    auto &ID=e->id;
    if(systemEventEnum::startService==ID)
        return on_startService((const systemEvent::startService*)e.operator->());
    if( rpcEventEnum::IncomingOnAcceptor==ID)
        throw CommonError("rpcIncomingOnAcceptor here %s %d",__FILE__,__LINE__);
    if( rpcEventEnum::IncomingOnConnector==ID)
    {
        
        rpcEvent::IncomingOnConnector* ez=static_cast<rpcEvent::IncomingOnConnector*>(e.operator ->());
        e->route=ez->e->route;
    }

    REF_getter<Route> r=((Event::Base*)e.operator ->())->route.pop_front();
    if(r->type==Route::OBJECTHANDLER_THREADED)
    {
        //logErr2("----------------if(r->type==Route::OBJECTHANDLER_THREADED)");
        
        ObjectHandlerRouteThreaded* l=(ObjectHandlerRouteThreaded*) r.operator ->();

        {
            
            M_LOCK(__m_lock);
            std::set<std::string>::const_iterator i=__mx_handlers.find(l->addr);
            if(i==__mx_handlers.end())
            {
                for(std::set<std::string>::iterator it=__mx_handlers.begin(); it!=__mx_handlers.end(); it++)
                {
                    DBG(log(TRACE_log,"*it %s",iUtils->bin2hex(*it).c_str()));
                }
                DBG(log(TRACE_log,"if(!m_handlers.count(l)) l->addr %s",iUtils->bin2hex(l->addr).c_str()));
                return false;
            }
            {
                ObjectHandlerThreaded* OH=NULL;
                std::string aid=*i;
                if(aid.size()!=sizeof(OH))
                {
                    DBG(log(ERROR_log,"if(aid.size()!=sizeof(OH)) %d %d",aid.size(),sizeof(OH)));
                    return false;
                }
                memcpy((char*)&OH,aid.data(),sizeof(OH));
                try {
                    M_UNLOCK(__m_lock);
                    //logErr2("if(!OH->OH_handleObjectEvent(e)) %s ",OH->name);
                    if(!OH->OH_handleObjectEvent(e))
                    {
                        logErr2("ObjectProxy::Threaded: unhandled event %s in %s",e->dump().c_str(), OH->name);
                    }
                }
                catch(...)
                {
                    logErr2("PASS in %s, %s %d",OH->name,__FILE__,__LINE__);
                    throw;
                }
            }
        }
    }
    else
    {
        logErr2("!(r->type==Route::OBJECTHANDLER_THREADED) %s",r->dump().c_str());
    }
    return true;

    XPASS;
    return false;

}

bool ObjectProxy::Polled::handleEvent(const REF_getter<Event::Base>& e)
{
    XTRY;
    
    //logErr2("ObjectProxy::Polled::handleEvent %s",e->dump().c_str());
    auto &ID=e->id;
    if(systemEventEnum::startService==ID)
        return on_startService((const systemEvent::startService*)e.operator->());

    if( rpcEventEnum::IncomingOnAcceptor==ID)
        throw CommonError("rpcIncomingOnAcceptor here %s %d",__FILE__,__LINE__);
    if( rpcEventEnum::IncomingOnConnector==ID)
    {
        
        rpcEvent::IncomingOnConnector* ez=static_cast<rpcEvent::IncomingOnConnector*>(e.operator ->());
        e->route=ez->e->route;
    }
    REF_getter<Route> r=((Event::Base*)e.operator ->())->route.pop_front();
    if(r->type==Route::OBJECTHANDLER_POLLED)
    {
        //logErr2("----------------if(r->type==Route::OBJECTHANDLER_POLLED)");
        
        ObjectHandlerRoutePolled* l=(ObjectHandlerRoutePolled*) r.operator ->();

        {
            M_LOCK(__m_lock);
            std::set<std::string>::const_iterator i=__mx_handlers.find(l->addr);
            if(i==__mx_handlers.end())
            {
                for(std::set<std::string>::iterator it=__mx_handlers.begin(); it!=__mx_handlers.end(); it++)
                {
                    DBG(log(TRACE_log,"*it %s",iUtils->bin2hex(*it).c_str()));
                }
                DBG(log(TRACE_log,"if(!m_handlers.count(l)) l->addr %s",iUtils->bin2hex(l->addr).c_str()));
                return false;
            }
            {
                ObjectHandlerPolled* OH=NULL;
                std::string aid=*i;
                if(aid.size()!=sizeof(OH))
                {
                    DBG(log(ERROR_log,"if(aid.size()!=sizeof(OH)) %d %d",aid.size(),sizeof(OH)));
                    return false;
                }
                memcpy((char*)&OH,aid.data(),sizeof(OH));
                try {
                    M_UNLOCK(__m_lock);
                    //logErr2("if(!OH->OH_handleObjectEvent(e)) %s",OH->name);
                    if(!OH->OH_handleObjectEvent(e))
                    {
                        logErr2("ObjectProxy::Threaded: unhandled event %s in %s",e->dump().c_str(), OH->name);
                    }
                }
                catch(...)
                {
                    std::string nm;
                    if(OH)
                        nm=OH->name;
                    logErr2("PASS in %s, %s %d",nm.c_str(),__FILE__,__LINE__);
                    //throw;
                }
            }
        }
    }
    else
    {
        logErr2("!(r->type==Route::OBJECTHANDLER_POLLED) %s",r->dump().c_str());
    }
    return true;

    XPASS;
    return false;

}
void ObjectProxy::Polled::sendObjectRequest(const msockaddr_in& addr, const SERVICE_id& dst, const REF_getter<Event::Base>& e)
{
    XTRY;

    

    e->route.push_front(new LocalServiceRoute(ListenerBase::serviceId));
    sendEvent(addr,dst,e);
    XPASS;
}
void ObjectProxy::Polled::sendObjectRequest(const SERVICE_id& dst, const REF_getter<Event::Base>& e)
{
    XTRY;
    

    e->route.push_front(new LocalServiceRoute(ListenerBase::serviceId));
    sendEvent(dst,e);
    XPASS;
}
void ObjectProxy::Threaded::sendObjectRequest(const msockaddr_in& addr, const SERVICE_id& dst, const REF_getter<Event::Base>& e)
{
    XTRY;

    

    e->route.push_front(new LocalServiceRoute(ListenerBase::serviceId));
    sendEvent(addr,dst,e);
    XPASS;
}
void ObjectProxy::Threaded::sendObjectRequest(const SERVICE_id& dst, const REF_getter<Event::Base>& e)
{
    XTRY;
    

    e->route.push_front(new LocalServiceRoute(ListenerBase::serviceId));
    sendEvent(dst,e);
    XPASS;
}

void registerObjectProxyModule(const char* pn)
{
    XTRY;
    
    if(pn)
    {
        iUtils->registerPlugingInfo(COREVERSION,pn,IUtils::PLUGIN_TYPE_SERVICE,ServiceEnum::ObjectProxyPolled,"ObjectProxyPolled");
        iUtils->registerPlugingInfo(COREVERSION,pn,IUtils::PLUGIN_TYPE_SERVICE,ServiceEnum::ObjectProxyThreaded,"ObjectProxyThreaded");
    }
    else
    {
        iUtils->registerService(COREVERSION,ServiceEnum::ObjectProxyPolled,ObjectProxy::Polled::construct,"ObjectProxyPolled");
        iUtils->registerService(COREVERSION,ServiceEnum::ObjectProxyThreaded,ObjectProxy::Threaded::construct,"ObjectProxyThreaded");
        regEvents_ObjectProxy();
    }
    XPASS;
}
void ObjectProxy::Polled::addObjectHandler(ObjectHandlerPolled* h)
{
    XTRY;
    
    M_LOCK(__m_lock);
    std::string s((char*)&h,sizeof(h));
    __mx_handlers.insert(s);
    XPASS;
}
void ObjectProxy::Polled::removeObjectHandler(ObjectHandlerPolled* h)
{
    XTRY;
    
    M_LOCK(__m_lock);
    std::string s((char*)&h,sizeof(h));
    __mx_handlers.erase(s);
    XPASS;
}
////////////////////////


void ObjectProxy::Threaded::addObjectHandler(ObjectHandlerThreaded* h)
{
    XTRY;
    
    M_LOCK(__m_lock);
    std::string s((char*)&h,sizeof(h));
    __mx_handlers.insert(s);
    XPASS;
}
void ObjectProxy::Threaded::removeObjectHandler(ObjectHandlerThreaded* h)
{
    XTRY;
    
    M_LOCK(__m_lock);
    std::string s((char*)&h,sizeof(h));
    __mx_handlers.erase(s);
    XPASS;
}

