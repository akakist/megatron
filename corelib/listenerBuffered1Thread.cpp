#include "listenerBuffered1Thread.h"
#include "mutexInspector.h"
ListenerBuffered1Thread::~ListenerBuffered1Thread()
{
    m_container.push(NULL);
    if(m_pt)
    {
        m_isTerminating=true;
        m_container.deinit();
        m_container.m_cond.broadcast();
        pthread_join(m_pt,NULL);
    }
}

ListenerBuffered1Thread::ListenerBuffered1Thread(UnknownBase *i, const std::string& name, IConfigObj* , const SERVICE_id & sid, IInstance *ins)
    :ListenerBase(i,name,sid),m_container(name,ins),m_pt(0),instance(ins),m_isTerminating(false) {

    if(pthread_create(&m_pt,NULL,ListenerBuffered1Thread::worker,this))
        throw CommonError("pthread_create: errno %d",errno);

}

void ListenerBuffered1Thread::processEvent(const REF_getter<Event::Base>&e)
{
    if(m_isTerminating) return;
    try {
        bool processed=false;
        {


            for(std::vector<std::pair<eventhandler,void*> >::iterator i=handlers.begin(); i!=handlers.end(); i++)
            {
                if(i->first)
                {
                    if(i->first(e.operator ->(),i->second))processed=true;
                }
            }
        }
        if(!e.valid()) throw CommonError("!.valid() "+_DMI());
        if(!processed)
        {
            XTRY;
            if(m_isTerminating) return;
            if(handleEvent(e))
            {
                processed=true;
            }

            XPASS;
        }
        if(!processed)
        {
            XTRY;
            logErr2("ListenerBuffered1Thread: unhandled event %s svs=%s in listener=%s %s",e->dump().c_str(),listenerName.c_str(), listenerName.c_str(),e->dump().c_str());
            XPASS;

        }
    }
    catch(std::exception &e)
    {
        logErr2("std::exception: %s %s",e.what(),_DMI().c_str());
    }

}

void* ListenerBuffered1Thread::worker(void*p)
{
#ifdef __MACH__
    pthread_setname_np("ListenerBuffered1Thread");
#else
#ifndef _WIN32
    pthread_setname_np(pthread_self(),"ListenerBuffered1Thread");
#endif
#endif
    ListenerBuffered1Thread* l=static_cast<ListenerBuffered1Thread*>(p);
    while(1)
    {
        try
        {
            REF_getter<Event::Base> e(NULL);

            if(l->m_isTerminating)
            {
                return NULL;
            }
            XTRY;
            e=l->m_container.pop();

            XPASS;
            if(l->m_isTerminating)
            {

                DBG(logErr2("ListenerBuffered1Thread exit %s",l->listenerName.c_str()));
                return NULL;
            }

            if(e.valid())
            {
                l->processEvent(e);
            }
            else
            {
                DBG(logErr2("ListenerBuffered1Thread exit %s",l->listenerName.c_str()));
                return NULL;
            }

        }
        catch(std::exception &e)
        {
            logErr2("exception: %s (%s) %s",e.what(),l->listenerName.c_str(),_DMI().c_str());
        }
    }
    return NULL;
}

void ListenerBuffered1Thread::listenToEvent(const std::deque<REF_getter<Event::Base> >&D)
{

    XTRY;
    for(size_t i=0; i<D.size(); i++)
        m_container.push(D[i]);
    XPASS;
}

void ListenerBuffered1Thread::listenToEvent(const REF_getter<Event::Base>& e)
{

    XTRY;
    m_container.push(e);
    XPASS;
}
