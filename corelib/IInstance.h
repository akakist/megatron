#ifndef _____________IFACTORY_____H
#define _____________IFACTORY_____H
#include <string>
#include "event.h"
#include "SERVICE_id.h"
#include "unknown.h"
#include "IUtils.h"
#include "route_t.h"
#include "ifaces.h"
#include "VERSION_id.h"
#include "GlobalCookie_id.h"
#include "sqlite3Wrapper.h"
/**       Глобальный интерфейс доступа к глобальным данным
*/

class IInstance
{
public:


    virtual void forwardEvent(const SERVICE_id& svs,  const REF_getter<Event::Base>&e)=0;

    /// послать евент в сервис
    virtual void sendEvent(const SERVICE_id& svs,  const REF_getter<Event::Base>&e)=0;

    /// послать евент в удаленный сервис по RPC
    virtual void sendEvent(const msockaddr_in& addr, const SERVICE_id& svs,  const REF_getter<Event::Base>&e)=0;

    /// послать евент в удаленный сервис по RPC
    virtual void sendEvent(const std::string& addr, const SERVICE_id& svs,  const REF_getter<Event::Base>&e)=0;


    /// послать евент в обратную сторону
    virtual void passEvent(const REF_getter<Event::Base>&e)=0;


    virtual void initServices()=0;
    virtual void deinitServices()=0;
    virtual UnknownBase* initService(const SERVICE_id& sid)=0;



    /// получить указатель на сервис по его ИД
    virtual UnknownBase * getServiceOrCreate(const SERVICE_id& id)=0;

    /// получить указатель на сервис по его ИД без создания сервиса
    virtual UnknownBase * getServiceNoCreate(const SERVICE_id& id)=0;





    virtual IConfigObj* getConfig()=0;





    virtual ~IInstance() {}



    virtual GlobalCookie_id globalCookie()=0;





    virtual std::set<msockaddr_in> myExternalAddressesGet()=0;
    virtual void myExternalAddressesInsert(const msockaddr_in&s)=0;
    virtual size_t myExternalAddressesCount(const msockaddr_in&s)=0;
    virtual size_t myExternalAddressesSize()=0;

    virtual void setConfig(IConfigObj *value)=0;

};

#endif

