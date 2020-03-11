#ifndef ________________LISTENER_Polled____H
#define ________________LISTENER_Polled____H
#include "listenerBase.h"
#include "mutexable.h"
#include "IConfigObj.h"
#include "IInstance.h"
class ListenerPolled;

///       Polled listener used mainly in GUI windows

class ListenerPolled:public ListenerBase, public Mutexable
{
    std::deque<REF_getter<Event::Base> > m_container_;

protected:

    /// implementation of base virtuals
    void listenToEvent(const REF_getter<Event::Base>&e);
    void listenToEvent(const std::deque<REF_getter<Event::Base> >&);

public:
    /// call method in inherited class to process event
    virtual bool handleEvent(const REF_getter<Event::Base>& e)=0;

    virtual ~ListenerPolled();
    ListenerPolled(const std::string& name, IConfigObj*, const SERVICE_id& sid);

    /// GUI class must call this method in onTimer
    void poll();

    void deinit();

};
#endif
