#include "timerService.h"
#include "events_timer.hpp"
#if !defined __MOBILE__

#include <sys/timeb.h>
#endif
#ifndef __ANDROID_API__
#include <sys/timeb.h>
#endif
#include "version_mega.h"
int Timer::task::total=0;
Integer getNow()
{


    uint64_t now;
#if defined(_WIN32)
    timeb tb;
    ftime(&tb);
    uint64_t add=tb.millitm;
    add*=1000;
    now=tb.time;
    now*=1000000;
    now+=add;
    Integer v;
    v.set(now);
    return v;
#else
    timespec tstmp;
    if(clock_gettime(CLOCK_REALTIME,&tstmp))
    {
        throw CommonError("clock_gettime: errno %d",errno);
    }
    uint64_t _1=tstmp.tv_sec;
    _1*=1000000;
    uint64_t _2=tstmp.tv_nsec;
    _2/=1000;

    now=_1;
    now+=_2;
    Integer v;
    v.set(now);
    return v;
#endif


}



Timer::Service::Service(const SERVICE_id& id, const std::string& nm, IInstance* ifa):
    UnknownBase(nm),

    ListenerBuffered1Thread(this,nm,ifa->getConfig(),id,ifa),
    Broadcaster(ifa),
    Logging(this,ERROR_log,ifa),
    iInstance(ifa),
    m_isTerminating(false)

{
}

Timer::Service::~Service()
{
    m_isTerminating=true;
    nexts.clear();
    all.clear();
    {
        nexts.m_condition.broadcast();
    }
    pthread_join(m_pt_of_thread,NULL);
}
bool Timer::Service::on_startService(const systemEvent::startService*)
{
    XTRY;
    if(pthread_create(&m_pt_of_thread,NULL,__worker,this))
    {
        throw CommonError("pthread_create: errno %d",errno);
    }
    XPASS;

    return true;
}

void* Timer::Service::__worker(void*p)
{
#ifdef __MACH__
    pthread_setname_np("Timer");
#else
#ifndef _WIN32
    pthread_setname_np(pthread_self(),"Timer");
#endif
#endif

    static_cast<Timer::Service*>(p)->worker();
    return NULL;
}

void Timer::Service::worker()
{
    m_pt_of_thread=pthread_self();
    while (1)
    {
        try
        {
            {
                {
                    XTRY;
                    //if(iInstance->isTerminating()) return;
                    if(m_isTerminating) return;

                    {
                        M_LOCKC(nexts.m_mutex);
                        if(nexts.mx_nexts.size()==0)
                        {
                            XTRY;
                            if(m_isTerminating) return;
                            //if(iInstance->isTerminating()) return;
                            nexts.m_condition.wait();
                            if(m_isTerminating) return;
                            //if(iInstance->isTerminating()) return;
                            XPASS;
                        }
                    }

                    while(1)
                    {
                        Integer itFirst;
                        std::deque<REF_getter<task> > itSecondCopy;
                        {
                            M_LOCKC(nexts.m_mutex);
                            if(nexts.mx_nexts.size())
                            {
                                Integer now=getNow();
                                std::map<Integer,std::deque<REF_getter<task> > >::iterator it2=nexts.mx_nexts.begin();
                                itFirst=it2->first;
                                std::deque<REF_getter<task> > &itSecondRef=it2->second;
                                if(getNow()<itFirst)
                                {
                                    now=getNow();

                                    mtimespec ts;
                                    {
                                        Integer tmp=itFirst;
                                        tmp/=1000000;
                                        {
                                            ts.tv_sec=(time_t)tmp.get();
                                            tmp=itFirst;
                                            tmp%=1000000;
                                            tmp*=1000;
                                        }
                                        ts.tv_nsec=(long)tmp.get();
                                    }

                                    XTRY;
                                    if(m_isTerminating) return;
                                    //if(iInstance->isTerminating()) return;
                                    {
                                        nexts.m_condition.timedwait(ts);
                                    }
                                    //if(iInstance->isTerminating()) return;
                                    if(m_isTerminating) return;

                                    XPASS;
                                    continue;
                                }
                                else
                                {
                                    itSecondCopy=itSecondRef;
                                    nexts.mx_nexts.erase(itFirst);
                                }
                            }
                            //if(iInstance->isTerminating()) return;
                            if(m_isTerminating) return;

                        }
                        for(size_t i=0; i<itSecondCopy.size(); i++)
                        {
                            REF_getter<task> t=itSecondCopy[i];
                            if(!t->erased)
                            {
                                if (t->type==task::TYPE_TIMER)
                                {
                                    XTRY;
                                    REF_getter<Event::Base> e=new timerEvent::TickTimer(t->id, t->data,t->cookie,t->destination);
                                    passEvent(e);
                                    {
                                        M_LOCKC(nexts.m_mutex);
                                        nexts.mx_nexts[getNow() + t->period_real*1000000].push_back(t);
                                    }
                                    XPASS;
                                }
                                else if (t->type==task::TYPE_ALARM)
                                {
                                    REF_getter<Event::Base> e=new timerEvent::TickAlarm(t->id,t->data, t->cookie, t->destination);
                                    passEvent(e);
                                    REF_getter<task> t=itSecondCopy[i];
                                    all.remove_t_only(t);
                                }
                            }
                            else
                            {
                                all.remove_t_only(t);
                            }
                        }
                    }
                    XPASS;
                }
            }
        }
        catch (std::exception& e)
        {
            logErr2("catched here %s %s %d",e.what(),__FILE__,__LINE__);
        }
        catch (...)
        {
            logErr2("catched here %s %d",__FILE__,__LINE__);
        }
    }

}
bool Timer::Service::on_SetTimer(const timerEvent::SetTimer* ev)
{
    route_t r=ev->route;
    r.pop_front();
    REF_getter<task> t=new task(task::TYPE_TIMER,ev->tid,ev->data,ev->cookie,r,ev->delay_secs);
    all.add(t);

    Integer tb=getNow();
    real d=ev->delay_secs;
    d*=1000000;
    tb+=d;

    {
        M_LOCKC(nexts.m_mutex);
        nexts.mx_nexts[tb].push_back(t);
    }
    //logErr2("setTimer signal");
    nexts.m_condition.signal();
    return true;
}
bool Timer::Service::on_SetAlarm(const timerEvent::SetAlarm* ev)
{
    XTRY;

    route_t r=ev->route;
    r.pop_front();
    REF_getter<task> t=new task(task::TYPE_ALARM,ev->tid,ev->data,ev->cookie,r,ev->delay_secs);
    all.add(t);
    Integer tb=getNow();
    real d=ev->delay_secs;
    d*=1000000;
    tb+=d;
    {
        XTRY;
        M_LOCKC(nexts.m_mutex);
        nexts.mx_nexts[tb].push_back(t);
        XPASS;
    }
    nexts.m_condition.signal();
    XPASS;
    return true;
}
bool Timer::Service::on_StopTimer(const timerEvent::StopTimer* ev)
{
    XTRY;
    route_t r=ev->route;
    r.pop_front();
    REF_getter<task> t=new task(task::TYPE_TIMER,ev->tid,ev->data,new refbuffer,r,0);
    all.remove(t);
    XPASS;
    return true;
}
bool Timer::Service::on_ResetAlarm(const timerEvent::ResetAlarm* ev)
{
    XTRY;
    route_t r=ev->route;
    r.pop_front();
    REF_getter<task> t=new task(task::TYPE_ALARM,ev->tid,ev->data,ev->cookie,r,ev->delay_secs);
    all.replace(t);
    Integer tb=getNow();
    real d=ev->delay_secs;
    d*=1000000;
    tb+=d;
    {
        M_LOCKC(nexts.m_mutex);
        nexts.mx_nexts[tb].push_back(t);
    }
    nexts.m_condition.signal();
    XPASS;
    return true;
}

bool Timer::Service::on_StopAlarm(const timerEvent::StopAlarm* ev)
{
    XTRY;
    route_t r=ev->route;
    r.pop_front();
    REF_getter<task> t=new task(task::TYPE_ALARM,ev->tid,ev->data,new refbuffer,r,0);
    all.remove(t);
    XPASS;
    return true;
}

void registerTimerService(const char* pn)
{
    if(pn)
    {
        iUtils->registerPlugingInfo(COREVERSION,pn,IUtils::PLUGIN_TYPE_SERVICE,ServiceEnum::Timer,"Timer");
    }
    else
    {
        iUtils->registerService(COREVERSION,ServiceEnum::Timer,Timer::Service::construct,"Timer");
        regEvents_timer();
    }
}

bool Timer::Service::handleEvent(const REF_getter<Event::Base>& e)
{
    XTRY;
    auto &ID=e->id;
    if(timerEventEnum::SetTimer==ID)
        return on_SetTimer((const timerEvent::SetTimer*)e.operator->());
    if( timerEventEnum::SetAlarm==ID)
        return on_SetAlarm((const timerEvent::SetAlarm*)e.operator->());
    if( timerEventEnum::ResetAlarm==ID)
        return on_ResetAlarm((const timerEvent::ResetAlarm*)e.operator->());
    if( timerEventEnum::StopTimer==ID)
        return on_StopTimer((const timerEvent::StopTimer*)e.operator->());
    if( timerEventEnum::StopAlarm==ID)
        return on_StopAlarm((const timerEvent::StopAlarm*)e.operator->());
    if(systemEventEnum::startService==ID)
        return on_startService((const systemEvent::startService*)e.operator->());

    XPASS;
    return false;

}

