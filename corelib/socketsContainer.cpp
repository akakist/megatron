#include "socketsContainer.h"
#include "epoll_socket_info.h"
#ifdef __linux__
#include <sys/epoll.h>
#endif
#include "mutexInspector.h"
void SocketsContainerBase::add(const SERVICE_id& sid,const REF_getter<epoll_socket_info>&esi)
{
    M_LOCK(m_lock);

    container.insert(std::make_pair(esi->m_id,esi));
    esi->m_socketsContainers.add(sid,this);
}
int SocketsContainerBase::count(const SOCKET_id& id)
{
    M_LOCK(m_lock);
    return container.count(id);
}
int SocketsContainerBase::size()
{
    M_LOCK(m_lock);
    return container.size();
}
REF_getter<epoll_socket_info> SocketsContainerBase::getOrNull(const SOCKET_id& sid) const
{
    M_LOCK(m_lock);
    std::map<SOCKET_id,REF_getter<epoll_socket_info> > ::const_iterator i=container.find(sid);
    if(i==container.end()) return NULL;
    return i->second;


}
void SocketsContainerBase::remove(const SOCKET_id& sid, const std::string& reason)
{
    REF_getter<epoll_socket_info> S=getOrNull(sid);
    if(S.valid())
    {
        on_delete(S,reason);
    }
    else
    {
    }
    {
        M_LOCK(m_lock);
        if(!container.count(sid))
        {

            DBG(logErr2("!!!!!!!!!!!!!!!!!! close socket: ! container.count %s %d",__FILE__,__LINE__));
        }
        container.erase(sid);
    }

}
void SocketsContainerBase::clear()
{
    M_LOCK(m_lock);
    container.clear();

}
std::map<SOCKET_id,REF_getter<epoll_socket_info> >SocketsContainerBase::getContainer()
{
    M_LOCK(m_lock);
    return container;
}
Json::Value SocketsContainerBase::jdump()
{
    Json::Value v;
    std::map<SOCKET_id,REF_getter<epoll_socket_info> >m=getContainer();
    for(std::map<SOCKET_id,REF_getter<epoll_socket_info> >::iterator i=m.begin(); i!=m.end(); i++)
    {
        v[iUtils->toString(CONTAINER(i->first))].append(i->second->getWebDumpableLink(iUtils->toString(CONTAINER(i->second->m_id))));
    }
    return v;
}
void SocketsContainerBase::mod_write(const SOCKET_id& sid)
{
    REF_getter<epoll_socket_info> S=getOrNull(sid);
    if(S.valid())
    {
        on_mod_write(S);
    }


}
