#pragma once
#include <deque>
#include "event_mt.h"
#include "mutexable.h"
#include <json/json.h>
#include "IInstance.h"

/// Used for event processing in Listeners
class EventDeque: public Refcountable
{
    std::deque<REF_getter<Event::Base> > container;
    MutexC m_mutex;
public:
    Condition m_cond;
private:
    bool m_isTerminating;
public:
    EventDeque():
    m_cond(m_mutex),
    m_isTerminating(false) {}
    void push(const REF_getter<Event::Base> & e);
    std::deque<REF_getter<Event::Base> > pop();
    void deinit()
    {
        m_isTerminating=true;
        m_cond.broadcast();
    }
    void clear()
    {
        M_LOCKC(m_mutex);
        container.clear();
        m_cond.broadcast();
    }
};

