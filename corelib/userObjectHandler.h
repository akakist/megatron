#ifndef _________UserObjectHandler_h
#define _________UserObjectHandler_h
#include "commonError.h"
#include "objectHandler.h"
class UserObjectHandler
{
public:
    IInstance *instance;
    ObjectHandler* parent;
    route_t route_this;
    UserObjectHandler(IInstance * _instance, const route_t& _route_this, ObjectHandler *_parent):instance(_instance),route_this(_route_this),parent(_parent)
    {


    }
    virtual bool OH_handleObjectEvent(const REF_getter<Event::Base>& e)=0;
};
class UserObjectHandlerPolled:
    public
        ObjectHandlerPolled
{
public:
    UserObjectHandlerPolled(const std::string& name, IInstance *ins): ObjectHandlerPolled(name,ins),user(NULL)
    {

    }
    bool OH_handleObjectEvent(const REF_getter<Event::Base>& e)
    {
        if(!user) throw CommonError("if(!user) %s",__PRETTY_FUNCTION__);
        return user->OH_handleObjectEvent(e);
    }
    UserObjectHandler* user;
};
class UserObjectHandlerThreaded:
    public
        ObjectHandlerThreaded
{
public:
    UserObjectHandlerThreaded(const std::string& name, IInstance *ins): ObjectHandlerThreaded(name,ins),user(NULL)
    {

    }
    bool OH_handleObjectEvent(const REF_getter<Event::Base>& e)
    {
        if(!user) throw CommonError("if(!user) %s",__PRETTY_FUNCTION__);
        return user->OH_handleObjectEvent(e);
    }

    UserObjectHandler* user;
};

#endif
