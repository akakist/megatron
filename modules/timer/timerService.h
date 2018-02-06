#ifndef ____SEVICE_TIMERR___H___
#define ____SEVICE_TIMERR___H___
#include "mutexable.h"
#include "unknown.h"
#include "SERVICE_id.h"
#include <sys/time.h>
#include "IInstance.h"
#include "listenerBuffered.h"
#include "listenerBuffered1Thread.h"
#include "broadcaster.h"

#include <ostream>
#include "json/json.h"
#include "logging.h"
#include "mutexInspector.h"
#include "Events/System/Run/startService.h"
#include "Events/System/timer/TickTimer.h"
#include "Events/System/timer/TickAlarm.h"
#include "Events/System/timer/StopAlarm.h"
#include "Events/System/timer/ResetAlarm.h"
#include "Events/System/timer/SetTimer.h"
#include "Events/System/timer/SetAlarm.h"
#include "Events/System/timer/StopTimer.h"


namespace Timer
{

    struct task:public Refcountable
    {
        enum TYP
        {
            TYPE_ALARM,TYPE_TIMER,
        };
        const TYP type;
    private:
    public:
        const int id;
        const REF_getter<refbuffer> data;
        const REF_getter<refbuffer> cookie;
        const route_t destination;
        double period_real;
        bool erased;
        Json::Value jdump()
        {
            Json::Value v;
            v["type"]=TYPE_ALARM?"ALARM":"TIMER";
            v["destination"]=destination.dump();
            v["period_real"]=period_real;
            v["id"]=id;

            return v;
        }
        static int total;
        task(TYP t,const int& _id, const REF_getter<refbuffer> & _data, const REF_getter<refbuffer> & _cookie, const route_t& dst,const double& timeout)
            :type(t),id(_id),data(_data),cookie(_cookie),destination(dst),period_real(timeout),erased(false)
        {
            total++;
        }
        ~task()
        {
            total--;
        }
    };
    struct _searchKey
    {
        REF_getter<task> t;
        _searchKey(const REF_getter<task>& tt):t(tt) {}
        int operator < (const _searchKey& b) const
        {
            const _searchKey& a=*this;
            if(a.t->id != b.t->id)
                return a.t->id < b.t->id;

            if(a.t->type != b.t->type)
                return a.t->type < b.t->type;

            if(!(a.t->destination == b.t->destination))
                return a.t->destination < b.t->destination;

            return std::string((char*)a.t->data->buffer,a.t->data->size) < std::string((char*)b.t->data->buffer,b.t->data->size);
            return 0;
        }
    };
    struct _all:public Mutexable
    {
        _all() {}
        std::map<_searchKey,std::set<REF_getter<task> > >   timers;
        void clear()
        {
            M_LOCK(this);
            timers.clear();
        }
        int exists(const REF_getter<task>& tt)
        {
            M_LOCK(this);
            return timers.count(tt);
        }
        void replace(const REF_getter<task>& t)
        {
            M_LOCK(this);
            std::set<REF_getter<task> > &s=timers[t];
            for(std::set<REF_getter<task> >::iterator i=s.begin(); i!=s.end(); i++)
            {
                REF_getter<task> task=*i;
                task->erased=true;
            }
            timers.erase(t);
            timers[t].insert(t);
        }
        void add(const REF_getter<task>& t)
        {
            M_LOCK(this);
            timers[t].insert(t);
        }
        void remove_t_only(const REF_getter<task>&t)
        {
            M_LOCK(this);
            std::set<REF_getter<task> > &s=timers[t];
            if(s.count(t))
            {
                s.erase(t);
            }
        }
        void remove(const REF_getter<task>& t)
        {
            M_LOCK(this);
            std::set<REF_getter<task> > &s=timers[t];
            bool found=false;
            for(std::set<REF_getter<task> >::iterator i=s.begin(); i!=s.end(); i++)
            {
                REF_getter<task> t=*i;
                t->erased=true;
                found=true;
            }
            timers.erase(t);
            if(!found)
            {
            }
            else
            {
            }
        }
    };
    struct _nexts
    {
        MutexC m_mutex;
        Condition m_condition;
        std::map<Integer,std::deque<REF_getter<task> > > mx_nexts;
        _nexts()
            :m_condition(m_mutex)
        {
        }
        ~_nexts()
        {
        }
        void clear()
        {
            M_LOCKC(m_mutex);
            mx_nexts.clear();
        }
    };

    class Service:
        private UnknownBase,
        private ListenerBuffered1Thread,
        private Broadcaster,
        public Logging
    {

        _all all;

        _nexts nexts;

        pthread_t m_pt_of_thread;
        IInstance *iInstance;
        bool handleEvent(const REF_getter<Event::Base>& e);

        Service(const SERVICE_id&, const std::string& nm, IInstance *ifa);
        ~Service();

        bool on_SetTimer(const timerEvent::SetTimer*);
        bool on_SetAlarm(const timerEvent::SetAlarm*);
        bool on_ResetAlarm(const timerEvent::ResetAlarm*);
        bool on_StopTimer(const timerEvent::StopTimer*);
        bool on_StopAlarm(const timerEvent::StopAlarm*);
        bool on_startService(const systemEvent::startService*);
    public:
        static UnknownBase* construct(const SERVICE_id& id, const std::string&  nm,IInstance* ifa)
        {
            return new Service(id,nm,ifa);
        }
    private:
        void worker();
        static void *__worker(void*);

        bool m_isTerminating;
    };
};
#endif
