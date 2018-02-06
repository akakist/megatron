#ifndef ________________IOBJECT_PROXY____H
#define ________________IOBJECT_PROXY____H

#include "msockaddr_in.h"
#include "SERVICE_id.h"
#include "REF.h"
#include "event.h"
#include "unknown.h"
#include "unknownCastDef.h"
class ObjectHandlerPolled;
class ObjectHandlerThreaded;

/**        Интерфейс для работы с возможностями прохождения евентов между объектами, унаследованными от ObjectHandler
*/

class IObjectProxyPolled
{
public:
    /// добавить хендлер
    virtual void addObjectHandler(ObjectHandlerPolled* h)=0;
    /// удалить хендлер
    virtual void removeObjectHandler(ObjectHandlerPolled* h)=0;

    /// послать евент на удаленный сервис по RPC
    virtual void sendObjectRequest(const msockaddr_in & dstHost, const SERVICE_id & dstService, const REF_getter<Event::Base>& e)=0;
    virtual void sendObjectRequest(const SERVICE_id & dstService, const REF_getter<Event::Base>& e)=0;
    virtual void poll()=0;

    IObjectProxyPolled(UnknownBase* i)
    {
        i->addClass(UnknownCast::IObjectProxyPolled,this);
    }

    virtual ~IObjectProxyPolled() {}

};
class IObjectProxyThreaded
{
public:
    /// добавить хендлер
    virtual void addObjectHandler(ObjectHandlerThreaded* h)=0;
    /// удалить хендлер
    virtual void removeObjectHandler(ObjectHandlerThreaded* h)=0;

    /// послать евент на удаленный сервис по RPC
    virtual void sendObjectRequest(const msockaddr_in & dstHost, const SERVICE_id & dstService, const REF_getter<Event::Base>& e)=0;
    virtual void sendObjectRequest(const SERVICE_id & dstService, const REF_getter<Event::Base>& e)=0;

    IObjectProxyThreaded(UnknownBase* i)
    {
        i->addClass(UnknownCast::IObjectProxyThreaded,this);
    }

    virtual ~IObjectProxyThreaded() {}

};
#endif
