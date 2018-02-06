#include "baseState.h"
#include "IInstance.h"
void stateHolder::addState(int st, const REF_getter<baseState>& s)
{
    M_LOCK(this);
    mx_states.insert(std::make_pair(st,s));
}
void stateHolder::setEnabledState(int st, bool enabled)
{
    M_LOCK(this);
    std::map<int,REF_getter<baseState> >::iterator i=mx_states.find(st);
    if(i==mx_states.end()) throw CommonError("if(i==states.end())");
    i->second->enabled=enabled;
}
void stateHolder::setEnabledStateAll(bool enabled)
{
    M_LOCK(this);
    for(std::map<int,REF_getter<baseState> >::iterator i=mx_states.begin(); i!=mx_states.end(); i++)
    {
        i->second->enabled=enabled;
    }

}
void stateHolder::start(int st)
{
    REF_getter<baseState> S(NULL);
    {
        M_LOCK(this);
        std::map<int,REF_getter<baseState> >::iterator i=mx_states.find(st);
        if(i==mx_states.end()) throw CommonError("if(i==states.end())");
        S=i->second;
    }
    S->start();
}

void stateHolder::handleObjectEventOnConnector(const REF_getter<epoll_socket_info>& esi, const REF_getter<Event::Base>& e, const msockaddr_in& connect_addr)
{
    std::vector<REF_getter<baseState> >v;
    {
        M_LOCK(this);
        for(std::map<int,REF_getter<baseState> >::iterator i=mx_states.begin(); i!=mx_states.end(); i++)
        {
            if(i->second->enabled)
            {
                v.push_back(i->second);
            }
        }
    }
    for(size_t i=0; i<v.size(); i++)
        v[i]->handleObjectEventOnConnector(esi,e,connect_addr);
}
void stateHolder::handleObjectEvent(const REF_getter<Event::Base>& e)
{
    std::vector<REF_getter<baseState> >v;
    {
        M_LOCK(this);
        for(std::map<int,REF_getter<baseState> >::iterator i=mx_states.begin(); i!=mx_states.end(); i++)
        {
            if(i->second->enabled)
            {
                v.push_back(i->second);
            }
        }
    }
    for(size_t i=0; i<v.size(); i++)
        v[i]->handleObjectEvent(e);
}
