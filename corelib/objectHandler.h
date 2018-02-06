#ifndef _______GUI_HANDLER______H
#define _______GUI_HANDLER______H
#include "event.h"
#include "SERVICE_id.h"
#include "epoll_socket_info.h"
#include "IObjectProxy.h"
#include "IInstance.h"
/**
* Базовый класс, добавляющий к объекту свойство маршрутизируемости
*/
class ObjectHandler
{
public:
    const char* name;
    virtual void sendEvent(const std::string & dstHost, const SERVICE_id & dstService, const REF_getter<Event::Base>& e)=0;
    virtual void sendEvent(const msockaddr_in & dstHost, const SERVICE_id & dstService, const REF_getter<Event::Base>& e)=0;
    virtual void sendEvent(const SERVICE_id & dstService, const REF_getter<Event::Base>& e)=0;
    void passEvent(const REF_getter<Event::Base>& e);

    enum Type
    {
        POLLED,THREADED
    };
    Type type;
    ObjectHandler(const char*_name,Type t,IInstance* _if):name(_name),type(t),iInstance(_if)
    {
    }
    virtual ~ObjectHandler()
    {
    }

    virtual bool OH_handleObjectEvent(const REF_getter<Event::Base>& e)=0;
    bool handleIncomingOnConnector(const REF_getter<Event::Base>& e);
protected:
    IInstance *iInstance;


};
class ObjectHandlerPolled: public ObjectHandler
{
    IObjectProxyPolled *objectProxy;
public:
    void sendEvent(const std::string & dstHost, const SERVICE_id & dstService, const REF_getter<Event::Base>& e);
    void sendEvent(const msockaddr_in & dstHost, const SERVICE_id & dstService, const REF_getter<Event::Base>& e);
    void sendEvent(const SERVICE_id & dstService, const REF_getter<Event::Base>& e);

    void sendEvent(const std::set<msockaddr_in> & dstHost, const SERVICE_id & dstService, const REF_getter<Event::Base>& e);

    ObjectHandlerPolled(const char *name,IInstance* _if);
    //ObjectHandlerPolled();
    ~ObjectHandlerPolled();

};
class ObjectHandlerThreaded: public ObjectHandler
{
    IObjectProxyThreaded *objectProxy;
public:
    void sendEvent(const std::string & dstHost, const SERVICE_id & dstService, const REF_getter<Event::Base>& e);
    void sendEvent(const msockaddr_in & dstHost, const SERVICE_id & dstService, const REF_getter<Event::Base>& e);
    void sendEvent(const SERVICE_id & dstService, const REF_getter<Event::Base>& e);

    void sendEvent(const std::set<msockaddr_in> & dstHost, const SERVICE_id & dstService, const REF_getter<Event::Base>& e);

    ObjectHandlerThreaded(const char *name,IInstance* _if);
    //ObjectHandlerThreaded();
    ~ObjectHandlerThreaded();

};

#endif
