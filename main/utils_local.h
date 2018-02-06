#ifndef UTILS_LOCAL_H
#define UTILS_LOCAL_H
#include "mutexable.h"
#include "SERVICE_id.h"
#include "ifaces.h"
#include "unknown.h"
#include "ITests.h"
#include "IUtils.h"
class IInstance;

struct Utils_local
{
    struct _ifaces: public Mutexable
    {
        std::map<SERVICE_id,Ifaces::Base*> container;
    };
    _ifaces ifaces;
    struct _itests: public Mutexable
    {
        std::map<SERVICE_id,itest_static_constructor> container;
    };
    _itests itests;
    struct _plugin_info: public Mutexable
    {
        std::map<SERVICE_id,std::string> services;
        std::map<SERVICE_id,std::string> ifaces;
        std::map<SERVICE_id,std::string> itests;
        std::map<EVENT_id, std::set<std::string> > events;

    };
    _plugin_info pluginInfo;
    struct __service_names: public Mutexable
    {
        std::map<std::string, SERVICE_id> name2id;
        std::map<SERVICE_id,std::string> id2name;
    };
    struct __service_constructors: public Mutexable
    {
        std::map<SERVICE_id,unknown_static_constructor> container;
    };
    struct __event_constructors: public Mutexable
    {
        std::map<EVENT_id,std::pair<event_static_constructor,std::string> > container;

    };
    __service_names service_names;
    __service_constructors service_constructors;
    __event_constructors event_constructors;

};
#endif // UTILS_LOCAL_H
