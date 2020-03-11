#ifndef ______________________EVENT_DEQUE_____H
#define ______________________EVENT_DEQUE_____H
#include <deque>
#include "event.h"
#include "mutexable.h"
#include "json/json.h"
#include "IInstance.h"

/// Used for event processing in Listeners
class EventDeque: public Refcountable
{
    std::deque<REF_getter<Event::Base> > container;
    MutexC m_mutex;
    const std::string name;
public:
    Condition m_cond;
private:
    IInstance *instance;
    bool m_isTerminating;
public:
    EventDeque(const std::string& _name,IInstance* ins):name(_name), m_cond(m_mutex),instance(ins),m_isTerminating(false) {}
    void push(const REF_getter<Event::Base> & e);
    REF_getter<Event::Base> pop();
    virtual ~EventDeque() {}
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
#endif

