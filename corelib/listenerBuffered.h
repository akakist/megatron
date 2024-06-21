#pragma once
#include "listenerBase.h"
#include "mutexable.h"
#include "IConfigObj.h"
#include "IInstance.h"
#include "eventDeque.h"
class ListenerBuffered;

/// multithreaded listener implementation

class ListenerBuffered:public ListenerBase, public Mutexable
{
    friend struct st_busy;

    /// event deque for each thread
    std::map<pthread_t, REF_getter<EventDeque> > m_container;

    /// same as deque, needed to choose deque wich process event
    std::vector<std::pair<pthread_t, REF_getter<EventDeque> > > m_vector;

    /// max thread number
    size_t m_maxThreads;

    IInstance * lb_instance;

    static void* worker(void*);
    void addDeque(pthread_t pt, const REF_getter<EventDeque>& d);
    REF_getter<EventDeque> getDeque();
    bool m_isTerminating_lb;


protected:
/// base class funcs implementation
    void listenToEvent(const REF_getter<Event::Base>&e) final;

public:
    void deinit();

    /// call method in inherited class to process event
    virtual bool handleEvent(const REF_getter<Event::Base>& e)=0;

    virtual ~ListenerBuffered();
    ListenerBuffered(const std::string& name, IConfigObj* conf, const SERVICE_id& sid, IInstance *ins);

    /// caller of handleEvent
    void processEvent(const REF_getter<Event::Base>&);
};
