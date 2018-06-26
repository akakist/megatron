#include "eventDeque.h"
#include "mutexInspector.h"


void EventDeque::push(const REF_getter<Event::Base> & e)
{


    {
        M_LOCKC(m_mutex);
        if(container.size()>1000)
        {
            DBG(logErr2("Warning: EventDeque size overflow > 1000 %s",e->dump().toStyledString().c_str()));
        }
        container.push_back(e);
    }
    m_cond.signal();
}
REF_getter<Event::Base> EventDeque::pop()
{
    M_LOCKC(m_mutex);
    while(1)
    {
        if(m_isTerminating)
            return NULL;
        //if(instance->isTerminating()) return NULL;
        if(!container.size())
            m_cond.wait();
        if(m_isTerminating)
            return NULL;
        if(container.size())
        {
            REF_getter<Event::Base> r=*container.begin();
            container.pop_front();
            m_cond.signal();
            return r;
        }
    }
    // throw CommonError("invalied POP");
}
