#include "listenerBuffered.h"
#include "mutexInspector.h"
#include <pthread.h>

ListenerBuffered::ListenerBuffered(UnknownBase *i, const std::string& name, IConfigObj* obj, const SERVICE_id& sid, IInstance *ins)
    :ListenerBase(i,name,sid),instance(ins),m_isTerminating_lb(false)
{
    CFG_PUSH("ListenerBuffered",obj);
    m_maxThreads=obj->get_int64_t("MaxThreadsCount",10,"Max number of working threads");

}
void ListenerBuffered::processEvent(const REF_getter<Event::Base>&e)
{
    bool processed=false;
    try {
        {


            for(std::vector<std::pair<eventhandler,void*> >::iterator i=handlers.begin(); i!=handlers.end(); i++)
            {
                if(i->first)
                {
                    if(i->first(e.operator ->(),i->second))processed=true;
                }
            }
        }
        if(!e.valid()) throw CommonError("!.valid()"+_DMI());
        if(!processed)
        {
            XTRY;
            if(handleEvent(e))
            {
                processed=true;
            }
            XPASS;

        }
        if(!processed)
        {
            XTRY;
            logErr2("ListenerBuffered: unhandled event %s svs=%s in listener=%s %s",e->dump().c_str(),iUtils->serviceName(serviceId).c_str(), listenerName.c_str(),e->dump().c_str());
            XPASS;

        }
    }
    catch(std::exception &e)
    {
        logErr2("std::exception: %s %s",e.what(),_DMI().c_str());
    }


}

struct st_busy
{


    REF_getter<EventDeque>  __ed;
    ListenerBuffered* impl;
    st_busy(ListenerBuffered* _impl): __ed(NULL),impl(_impl)
    {

        XTRY;
        M_LOCK(impl);
        std::map<pthread_t, REF_getter<EventDeque> >::iterator i=impl->m_container.find(pthread_self());
        if(i==impl->m_container.end()) throw CommonError("cannot find element "+_DMI());
        {
            __ed=i->second;
            impl->m_container.erase(pthread_self());
        }
        XPASS;
    }
    ~st_busy()
    {
        XTRY;
        M_LOCK(impl);
        impl->m_container.insert(std::make_pair(pthread_self(),__ed));
        XPASS;
    }
};

void ListenerBuffered::addDeque(pthread_t pt, const REF_getter<EventDeque>& d)
{


    XTRY;
    M_LOCK(this);
    m_container.insert(std::make_pair(pt,d));
    m_vector.push_back(std::make_pair(pt,d));
    XPASS;
}


REF_getter<EventDeque> ListenerBuffered::getDeque()
{

    XTRY;
    M_LOCK(this);
    auto i=m_container.find(pthread_self());

    if(i==m_container.end()) return NULL;
    return i->second;
    XPASS;
}

void* ListenerBuffered::worker(void*p)
{

#ifdef __MACH__
    pthread_setname_np("ListenerBuffered");
#else
#ifndef _WIN32
    pthread_setname_np(pthread_self(),"ListenerBuffered");
#endif
#endif
    ListenerBuffered* l=static_cast<ListenerBuffered*>(p);
    REF_getter<EventDeque> d(NULL);

    while(!d.valid())
    {
        XTRY;
        d=l->getDeque();
#ifdef _MSC_VER
        Sleep(10);
#else
        usleep(10000);
#endif
        XPASS;
    }
    while(1)
    {
        try
        {
            REF_getter<Event::Base> e(NULL);

            if(l->m_isTerminating_lb)
            {
                return NULL;
            }
            XTRY;
            e=d->pop();
            XPASS;

            if(e.valid())
            {
                XTRY;
                st_busy tmpVar$$(l);
                XTRY;
                l->processEvent(e);
                XPASS;
                XPASS;
            }
            else
            {
                DBG(logErr2("ListenerBufferedImpl exit %s",l->listenerName.c_str()));
                return NULL;
            }

        }
        catch(std::exception &e)
        {
            logErr2("exception: %s (%s) %s ",e.what(),l->listenerName.c_str(),_DMI().c_str());
        }
    }
    return NULL;
}
static int rnd=(int)time(NULL);
int rrand()
{
    return rnd+=213231+rnd*rnd;
}
void ListenerBuffered::listenToEvent(const std::deque<REF_getter<Event::Base> >&D)
{
    XTRY;
    REF_getter<EventDeque> __ed(NULL);
    {
        XTRY;
        M_LOCK(this);
        if(this->m_container.size())
        {
            __ed=this->m_container.begin()->second;
        }
        XPASS;
    }
    if(!__ed.valid())
    {
        XTRY;
        M_LOCK(this);
        if(this->m_vector.size()<this->m_maxThreads)
        {
            __ed=new EventDeque(listenerName,instance);
            pthread_t __pt;
            if(pthread_create(&__pt,NULL,ListenerBuffered::worker,this))
                throw CommonError("pthread_create: errno %d",errno);

            DBG(logErr2("pthread_create %s",listenerName.c_str()));
            //iUtils->getIThreadNameController()->print_term(100);
            this->m_container.insert(std::make_pair(__pt,__ed));
            this->m_vector.push_back(std::make_pair(__pt,__ed));
        }
        else
        {
            __ed=this->m_vector[rrand()%this->m_vector.size()].second;
        }
        XPASS;
    }
    if(!__ed.valid()) throw CommonError("cannot find event deque " +_DMI());

    for(size_t i=0; i<D.size(); i++)
        __ed->push(D[i]);
    XPASS;
}

void ListenerBuffered::listenToEvent(const REF_getter<Event::Base>& e)
{

    XTRY;
    REF_getter<EventDeque> __ed(NULL);
    {

        XTRY;
        M_LOCK(this);
        if(this->m_container.size())
        {
            __ed=this->m_container.begin()->second;
        }
        XPASS;
    }
    if(!__ed.valid())
    {

        XTRY;
        M_LOCK(this);
        if(this->m_vector.size()<this->m_maxThreads)
        {

            __ed=new EventDeque(listenerName,instance);
            pthread_t __pt;
            if(pthread_create(&__pt,NULL,ListenerBuffered::worker,this))
                throw CommonError("pthread_create: errno %d",errno);

            DBG(logErr2("pthread_create %s",listenerName.c_str()));
            //iUtils->getIThreadNameController()->print_term(100);

            this->m_container.insert(std::make_pair(__pt,__ed));
            this->m_vector.push_back(std::make_pair(__pt,__ed));
        }
        else
        {

            __ed=this->m_vector[rrand()%this->m_vector.size()].second;
        }
        XPASS;
    }
    if(!__ed.valid()) throw CommonError("cannot find event deque "+_DMI());

    __ed->push(e);
    XPASS;
}
ListenerBuffered::~ListenerBuffered()
{
    m_isTerminating_lb=true;
    std::map<pthread_t, REF_getter<EventDeque> > m_evts;
    {
        M_LOCK(this);
        m_evts=m_container;
    }
    for(std::map<pthread_t, REF_getter<EventDeque> >::iterator i=m_evts.begin(); i!=m_evts.end(); i++)
    {
        REF_getter<EventDeque> d(i->second);
        d->push(NULL);
        d->deinit();
        d->m_cond.broadcast();
        pthread_join(i->first,NULL);
    }

}
