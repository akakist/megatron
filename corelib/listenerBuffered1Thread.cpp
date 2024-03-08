#include "listenerBuffered1Thread.h"
#include "mutexInspector.h"
#include "logging.h"
#include "colorOutput.h"
ListenerBuffered1Thread::~ListenerBuffered1Thread()
{
}
void ListenerBuffered1Thread::deinit()
{
    auto cont=m_container;
    m_container=NULL;
    cont->clear();
    cont->push(NULL);
    if(m_pt)
    {
        m_isTerminating=true;
        cont->deinit();
        cont->m_cond.broadcast();
        int err=pthread_join(m_pt,NULL);
        if(err)
        {
            printf(RED("%s pthread_join: %s"),__PRETTY_FUNCTION__,strerror(errno));
        }

    }

}
ListenerBuffered1Thread::ListenerBuffered1Thread(UnknownBase *i, const std::string& name, IConfigObj*, const SERVICE_id & sid, IInstance *ins)
    :ListenerBase(name,sid),m_container(new EventDeque(name,ins)),m_pt(0),m_isTerminating(false) {


    XTRY;
    if(pthread_create(&m_pt,NULL,ListenerBuffered1Thread::worker,this))
        throw CommonError("pthread_create: errno %d",errno);
    XPASS;

}

void ListenerBuffered1Thread::processEvent(const REF_getter<Event::Base>&e)
{
    if(m_isTerminating) return;
    try {
        bool processed=false;
        {


//            for(auto& i:handlers)
//            {
//                if(i.first)
//                {
//                    if(i.first(e.operator ->(),i.second))processed=true;
//                }
//            }
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
            logErr2("ListenerBuffered1Thread: unhandled event %s svs=%s in listener=%s %s",
                    e->dump().toStyledString().c_str(),listenerName.c_str(), listenerName.c_str(),e->dump().toStyledString().c_str());
            XPASS;

        }
    }
    catch(CommonError& e)
    {
        logErr2("CommonError: " RED2("%s %s"),e.what(),_DMI().c_str());

    }
    catch(std::exception &e)
    {
        logErr2("ListenerBuffered1Thread std::exception: " RED2("%s %s"),e.what(),_DMI().c_str());
    }

}

void* ListenerBuffered1Thread::worker(void*p)
{
#ifdef __MACH__
    pthread_setname_np("ListenerBuffered1Thread");
#else
#if !defined _WIN32 && !defined __FreeBSD__

    pthread_setname_np(pthread_self(),"ListenerBuffered1Thread");
#endif
#endif
    ListenerBuffered1Thread* l=static_cast<ListenerBuffered1Thread*>(p);
    while(1)
    {
        try
        {
            REF_getter<EventDeque> dq=l->m_container;
            if(!dq.valid())
            {
                logErr2("!dq valid");
                return NULL;
            }
            REF_getter<Event::Base> e(NULL);

            if(l->m_isTerminating)
            {
                return NULL;
            }
            XTRY;
            e=dq->pop();

            XPASS;
            if(l->m_isTerminating)
            {

                DBG(printf(BLUE("ListenerBuffered1Thread exit %s"),l->listenerName.c_str()));
                return NULL;
            }

            if(e.valid())
            {
                l->processEvent(e);
            }
            else
            {
                DBG(printf(BLUE("ListenerBuffered1Thread exit %s"),l->listenerName.c_str()));
                return NULL;
            }

        }
        catch(std::exception &e)
        {
            logErr2("exception: " RED2("%s") " (%s) %s",e.what(),l->listenerName.c_str(),_DMI().c_str());
        }
    }
    return NULL;
}


void ListenerBuffered1Thread::listenToEvent(const REF_getter<Event::Base>& e)
{

    if(!m_container.valid())
    {
        logErr2("if(!m_container.valid())");
        return;
    }

    if(m_isTerminating) return;
    XTRY;
    m_container->push(e);
    XPASS;
}
