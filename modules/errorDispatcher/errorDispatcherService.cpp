#include "errorDispatcherService.h"
#include <stdarg.h>
#include "unknownCastDef.h"

#include "IRPC.h"
#include "version_mega.h"
#include "VERSION_id.h"
#include "tools_mt.h"
#include "events_errorDispatcher.hpp"
ErrorDispatcher::Service::Service(const SERVICE_id& id, const std::string& nm, IInstance *ifa):
    UnknownBase(nm),
    ListenerBuffered1Thread(this,nm,ifa->getConfig(),id,ifa),Broadcaster(ifa)

{


}

ErrorDispatcher::Service::~Service()
{
}
bool ErrorDispatcher::Service::on_startService(const systemEvent::startService* e)
{
    if(!e) throw CommonError("!e");
    return true;
}

void registerErrorDispatcherService(const char* pn)
{
    XTRY;
    if(pn)
    {

        iUtils->registerPlugingInfo(COREVERSION,pn,IUtils::PLUGIN_TYPE_SERVICE,ServiceEnum::ErrorDispatcher,"ErrorDispatcher",getEvents_errorDispatcher());
    }
    else
    {
        iUtils->registerService(COREVERSION,ServiceEnum::ErrorDispatcher,ErrorDispatcher::Service::construct,"ErrorDispatcher");
        regEvents_errorDispatcher();
    }
    XPASS;
}

bool ErrorDispatcher::Service::on_errorDispatcherSendMessage(const errorDispatcherEvent::SendMessage* e)
{
    MUTEX_INSPECTOR;
    XTRY;
    if(!e) throw CommonError("!e");
    for(auto& i:m_subscribers)
    {
        {
            passEvent(new errorDispatcherEvent::NotifySubscriber(e->opcode,e->msg,poppedFrontRoute(i)));
        }
    }
    XPASS;
    return true;
}

bool ErrorDispatcher::Service::on_IncomingOnAcceptor(const rpcEvent::IncomingOnAcceptor*ev)
{
    MUTEX_INSPECTOR;
    XTRY;
    if(!ev) throw CommonError("!ev");
    auto &IDA=ev->e->id;
    if( errorDispatcherEventEnum::Subscribe==IDA)
        return on_errorDispatcherSubscribe(static_cast<const errorDispatcherEvent::Subscribe* > (ev->e.operator ->()));
    if( errorDispatcherEventEnum::Unsubscribe==IDA)
        return on_errorDispatcherUnsubscribeAll(static_cast<const errorDispatcherEvent::Unsubscribe* > (ev->e.operator ->()));
    if( errorDispatcherEventEnum::SendMessage==IDA)
        return on_errorDispatcherSendMessage(static_cast<const errorDispatcherEvent::SendMessage* > (ev->e.operator ->()));

    logErr2("ErrorDispatcher: unhandled event %s %s %d",ev->id.dump().c_str(),__FILE__,__LINE__);
    XPASS;
    return false;
}
bool ErrorDispatcher::Service::on_errorDispatcherSubscribe(const errorDispatcherEvent::Subscribe* e)
{
    MUTEX_INSPECTOR;
    XTRY;
    if(!e) throw CommonError("!e");
    m_subscribers.insert(e->route);
    /*for(auto &i:m_cache)
    {
        passEvent(new errorDispatcherEvent::NotifySubscriber(i.first,i.second,poppedFrontRoute(e->route)));

    }*/
    XPASS;
    return true;
}
bool ErrorDispatcher::Service::on_errorDispatcherUnsubscribeAll(const errorDispatcherEvent::Unsubscribe* e)
{
    MUTEX_INSPECTOR;
    XTRY;
    if(!e) throw CommonError("!e");
    m_subscribers.erase(e->route);
    XPASS;
    return true;
}
bool ErrorDispatcher::Service::handleEvent(const REF_getter<Event::Base>& e)
{
    MUTEX_INSPECTOR;
    XTRY;
    auto &ID=e->id;

    auto z=dynamic_cast<const errorDispatcherEvent::Subscribe*>(e.operator->());
    if(z)
        return on_errorDispatcherSubscribe(z);

    if( errorDispatcherEventEnum::Subscribe==ID)
        return on_errorDispatcherSubscribe(dynamic_cast<const errorDispatcherEvent::Subscribe* > (e.operator ->()));
    if( errorDispatcherEventEnum::Unsubscribe==ID)
        return on_errorDispatcherUnsubscribeAll(dynamic_cast<const errorDispatcherEvent::Unsubscribe* > (e.operator ->()));
    if( errorDispatcherEventEnum::SendMessage==ID)
        return on_errorDispatcherSendMessage(dynamic_cast<const errorDispatcherEvent::SendMessage* > (e.operator ->()));
    if( systemEventEnum::startService==ID)
        return on_startService(dynamic_cast<const systemEvent::startService*>(e.operator->()));
    if( rpcEventEnum::IncomingOnAcceptor==ID)
        return(this->on_IncomingOnAcceptor(dynamic_cast<const rpcEvent::IncomingOnAcceptor*>(e.operator->())));

    logErr2("ErrorDispatcher: unhandled event %s %s %d",e->id.dump().c_str(),__FILE__,__LINE__);
    XPASS;
    return false;
}
