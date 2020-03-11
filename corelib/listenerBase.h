#ifndef ______LISTENER____H
#define ______LISTENER____H
#include "unknownCastDef.h"
#include "unknown.h"
#include "event.h"

typedef bool (*eventhandler) (const REF_getter<Event::Base> &, void*);

/**
* Base listener class
*/

class ListenerBase
{
protected:
public:
    /// deprecated, list of handler to work with ifacegeb
    std::vector<std::pair<eventhandler,void*> > handlers;

    /// declaration, implementation in inherited
    virtual void listenToEvent(const REF_getter<Event::Base>&)=0;
    virtual void listenToEvent(const std::deque<REF_getter<Event::Base> >&)=0;

    /// name of listener
    const std::string listenerName;


    ListenerBase(const std::string& nm, const SERVICE_id& sid);
    virtual ~ListenerBase() {}

    const SERVICE_id serviceId;
};

#endif

