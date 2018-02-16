#include "objectHandler.h"
#include "IInstance.h"

#include "url.h"
#include "Events/System/Net/rpc/IncomingOnConnector.h"

void ObjectHandlerPolled::sendEvent(const std::string & dstHost, const SERVICE_id & dstService, const REF_getter<Event::Base>& e)
{
    try {
        Url u;
        u.parse(dstHost);
        if(u.host=="local")
        {
            sendEvent(dstService,e);
        }
        else
        {
            msockaddr_in sa;
            sa.init(u);
            sendEvent(sa,dstService,e);
        }
    }
    catch(std::exception& ex)
    {
        logErr2("exception: %s",ex.what());
    }

}
void ObjectHandlerThreaded::sendEvent(const std::string & dstHost, const SERVICE_id & dstService, const REF_getter<Event::Base>& e)
{
    try {
        Url u;
        u.parse(dstHost);
        if(u.host=="local")
        {
            sendEvent(dstService,e);
        }
        else
        {
            msockaddr_in sa;
            sa.init(u);
            sendEvent(sa,dstService,e);
        }
    }
    catch(std::exception& ex)
    {
        logErr2("exception: %s",ex.what());
    }

}
void ObjectHandlerPolled::sendEvent(const std::set<msockaddr_in> & dstHost, const SERVICE_id & dstService, const REF_getter<Event::Base>& e)
{
    for(auto &i:dstHost)
    {
        objectProxy->sendObjectRequest(i,dstService,e);
    }
}
void ObjectHandlerThreaded::sendEvent(const std::set<msockaddr_in> & dstHost, const SERVICE_id & dstService, const REF_getter<Event::Base>& e)
{
    for(auto &i:dstHost)
    {
        objectProxy->sendObjectRequest(i,dstService,e);
    }
}

void ObjectHandlerPolled::sendEvent(const msockaddr_in & dstHost, const SERVICE_id & dstService, const REF_getter<Event::Base>& e)
{
    objectProxy->sendObjectRequest(dstHost,dstService,e);
}

void ObjectHandlerThreaded::sendEvent(const msockaddr_in & dstHost, const SERVICE_id & dstService, const REF_getter<Event::Base>& e)
{
    objectProxy->sendObjectRequest(dstHost,dstService,e);
}
void ObjectHandlerPolled::sendEvent(const SERVICE_id & dstService, const REF_getter<Event::Base>& e)
{
    objectProxy->sendObjectRequest(dstService,e);
}
void ObjectHandlerThreaded::sendEvent(const SERVICE_id & dstService, const REF_getter<Event::Base>& e)
{
    objectProxy->sendObjectRequest(dstService,e);
}
ObjectHandlerPolled::ObjectHandlerPolled(const char *name,IInstance* _if)
    :ObjectHandler(name,ObjectHandler::POLLED,_if),
     objectProxy(static_cast<IObjectProxyPolled*>
                 (_if->getServiceOrCreate(ServiceEnum::ObjectProxyPolled)->cast(UnknownCast::IObjectProxyPolled)))
{
    XTRY;
    objectProxy->addObjectHandler(this);
    XPASS;
}
ObjectHandlerThreaded::ObjectHandlerThreaded(const char *name, IInstance *_if)
    :ObjectHandler(name,ObjectHandler::THREADED,_if),
     objectProxy(static_cast<IObjectProxyThreaded*>
                 (_if->getServiceOrCreate(ServiceEnum::ObjectProxyThreaded)->cast(UnknownCast::IObjectProxyThreaded)))
{
    XTRY;
    objectProxy->addObjectHandler(this);
    XPASS;
}


ObjectHandlerPolled::~ObjectHandlerPolled()
{

    XTRY;
    objectProxy->removeObjectHandler(this);
    XPASS;
}
ObjectHandlerThreaded::~ObjectHandlerThreaded()
{

    XTRY;
    objectProxy->removeObjectHandler(this);
    XPASS;
}

void ObjectHandler::passEvent(const REF_getter<Event::Base>& e)
{
    XTRY;

    iInstance->passEvent(e);
    XPASS;
}

