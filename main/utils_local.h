#ifndef UTILS_LOCAL_H
#define UTILS_LOCAL_H
#include "mutexable.h"
#include "SERVICE_id.h"
#include "ifaces.h"
#include "unknown.h"
#include "IUtils.h"
#include "mutexInspector.h"
#include "colorOutput.h"
class IInstance;

struct Utils_local
{
    struct _ifaces
    {
        RWLock lk;
        std::map<SERVICE_id,Ifaces::Base*> container;
        void clear()
        {

            MUTEX_INSPECTOR;

            decltype(container) ifs;

            {
                WLocker asdas(lk);
                ifs=container;
                container.clear();

            }
            for(auto & z: ifs)
            {
                std::string name=z.first.dump().c_str();
                printf(BLUE("deleting %s"),z.first.dump().c_str());
                delete z.second;
                printf(BLUE("deleted %s"),name.c_str());

            }

        }
    };
    _ifaces ifaces;
    struct _plugin_info: public Mutexable
    {
        std::map<SERVICE_id,std::string> services;
        std::map<SERVICE_id,std::string> ifaces;
        std::map<EVENT_id, std::set<std::string> > events;
        void clear()
        {

            MUTEX_INSPECTOR;

            M_LOCK(this);
            services.clear();
            ifaces.clear();
            events.clear();
        }

    };
    _plugin_info pluginInfo;
    struct __service_names
    {
        RWLock lk;
        std::map<std::string, SERVICE_id> name2id;
        std::map<SERVICE_id,std::string> id2name;
        void clear()
        {
            MUTEX_INSPECTOR;


            WLocker dddd(lk);
            name2id.clear();
            id2name.clear();
        }
    };
    struct __service_constructors
    {
        RWLock lk;
        std::map<SERVICE_id,unknown_static_constructor> container;
        void clear()
        {

            MUTEX_INSPECTOR;
            WLocker asasd(lk);

            container.clear();
        }
    };
    struct __event_constructors
    {
        RWLock lk;
        std::map<EVENT_id,event_static_constructor> container;
        void clear()
        {

            MUTEX_INSPECTOR;
            WLocker aa(lk);

            container.clear();
        }

    };
    __service_names service_names;
    __service_constructors service_constructors;
    __event_constructors event_constructors;

    void clear()
    {
        MUTEX_INSPECTOR;
        service_constructors.clear();
        event_constructors.clear();
        ifaces.clear();
        pluginInfo.clear();
        service_names.clear();
    }

};
#endif // UTILS_LOCAL_H
