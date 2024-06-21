#include "listenerBuffered.h"
#include "IUtils.h"
#include "mutexInspector.h"
#include <pthread.h>
#include <unistd.h>
#include "colorOutput.h"

ListenerBuffered::ListenerBuffered(const std::string& name, IConfigObj* obj, const SERVICE_id& sid, IInstance *ins)
    :ListenerBase(name,sid),lb_instance(ins),m_isTerminating_lb(false)
{
    if(!obj)
        throw CommonError("config==NULL %s %d",__FILE__,__LINE__);
    CFG_PUSH("ListenerBuffered",obj);
    m_maxThreads=obj->get_int64_t("MaxThreadsCount",10,"Max number of working threads");

}
void ListenerBuffered::processEvent(const REF_getter<Event::Base>&e)
{
    bool processed=false;
    try {
        {


//            for(auto& i:handlers)
//            {
//                if(i.first)
//                {
//                    if(i.first(e.get(),i.second))processed=true;
//                }
//            }
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
            logErr2("ListenerBuffered: unhandled event %s svs=%s in listener=%s %s",e->dump().toStyledString().c_str(),iUtils->serviceName(serviceId).c_str(), listenerName_.c_str(),e->dump().toStyledString().c_str());
            XPASS;

        }
    }
    catch(CommonError& e)
    {
        logErr2("ListenerBuffered CommonError:" RED2(" %s %s"),e.what(),_DMI().c_str());

    }
    catch(std::exception &e)
    {
        logErr2("ListenerBuffered std::exception:"  RED2("%s %s"),e.what(),_DMI().c_str());
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
        auto i=impl->m_container.find(pthread_self());
        if(i==impl->m_container.end()) throw CommonError("cannot find element "+_DMI());
        {
            __ed=i->second;
            impl->m_container.erase(pthread_self());
        }
        XPASS;
    }
    ~st_busy()
    {
        try {
            M_LOCK(impl);
            impl->m_container.insert(std::make_pair(pthread_self(),__ed));
        }
        catch(...) {}
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
#if !defined _WIN32 && !defined __FreeBSD__
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
                DBG(logErr2("ListenerBufferedImpl exit %s",l->listenerName_.c_str()));
                return NULL;
            }

        }
        catch(std::exception &e)
        {
            logErr2("exception: %s (%s) %s ",e.what(),l->listenerName_.c_str(),_DMI().c_str());
        }
    }
    return NULL;
}
static int rnd=(int)time(NULL);
int rrand()
{
    return rnd+=213231+rnd*rnd;
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

            __ed=new EventDeque(listenerName_,lb_instance);
            pthread_t __pt;
            if(pthread_create(&__pt,NULL,ListenerBuffered::worker,this))
                throw CommonError("pthread_create: errno %d",errno);

            DBG(logErr2("pthread_create %s",listenerName_.c_str()));

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

}
void ListenerBuffered::deinit()
{
    m_isTerminating_lb=true;

    std::map<pthread_t, REF_getter<EventDeque> > m_evts;
    {
        M_LOCK(this);
        m_evts=m_container;
        m_container.clear();
    }
    for(auto& i:m_evts)
    {
        REF_getter<EventDeque> d(i.second);
        d->clear();
        d->push(NULL);
        d->deinit();
        d->m_cond.broadcast();
        int err=pthread_join(i.first,NULL);
        if(err)
        {
            printf(RED("%s pthread_join: %s"),__PRETTY_FUNCTION__,strerror(errno));
        }

    }

}
