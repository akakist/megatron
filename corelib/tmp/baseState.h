#ifndef _____________BASE_STATE_HHH
#define _____________BASE_STATE_HHH
#include "epoll_socket_info.h"
#include "event.h"
struct baseState: public Refcountable
{
    virtual void handleObjectEvent(const REF_getter<Event::Base>& e)=0;
    virtual void handleObjectEventOnConnector(const REF_getter<epoll_socket_info>& esi, const REF_getter<Event::Base>& e, const msockaddr_in& connect_addr)=0;
    virtual void reset()=0;
    virtual void start()=0;
    bool enabled;
    baseState():enabled(false) {}
};
class stateHolder: public Mutexable
{
    std::map<int,REF_getter<baseState> > mx_states;
public:
    void addState(int st, const REF_getter<baseState>& s);
    void setEnabledState(int st, bool enabled);
    void setEnabledStateAll(bool enabled);
    void start(int st);
    void handleObjectEventOnConnector(const REF_getter<epoll_socket_info>& esi, const REF_getter<Event::Base>& e, const msockaddr_in& connect_addr);
    void handleObjectEvent(const REF_getter<Event::Base>& e);

};

#endif
