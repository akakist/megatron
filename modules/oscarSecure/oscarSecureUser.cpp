#include "oscarSecureUser.h"
//using namespace OscarSecure;
REF_getter<OscarSecure::User> OscarSecure::__users::find(const SOCKET_id&s) const
{
    XTRY;
    REF_getter<User> ret(NULL);
    M_LOCK(m_lock);
    std::map<SOCKET_id,REF_getter<User> >::const_iterator i=container.find(s);
    if (i!=container.end()) ret=i->second;

    return ret;
    XPASS;
}
REF_getter<OscarSecure::User> OscarSecure::__users::find_throw(const SOCKET_id&s) const
{
    REF_getter<User> ret(NULL);
    M_LOCK(m_lock);
    std::map<SOCKET_id,REF_getter<User> >::const_iterator i=container.find(s);
    if (i!=container.end()) return i->second;
    throw CommonError("user not found %s %d",__FILE__,__LINE__);
}
OscarSecure::User::User(const SOCKET_id& id, bool _isServer):socketId(id),isServer(_isServer), inited(false)
{
}


OscarSecure::User::~User()
{
}
void OscarSecure::__users::insert(const REF_getter<User>& bi)
{

    M_LOCK(m_lock);
    container.insert(std::make_pair(bi->socketId,bi));

}

Json::Value OscarSecure::User::jdump()
{
    Json::Value v;
    v["id"]=iUtils->toString(CONTAINER(socketId));
    v["isServer"]=isServer;
    //v["user_id"]=CONTAINER(peer_email);
    return v;
}

Json::Value OscarSecure::__users::jdump()
{
    Json::Value v;
    std::map<SOCKET_id,REF_getter<User> > m;
    {
        M_LOCK(m_lock);
        m=container;
    }

    for(std::map<SOCKET_id,REF_getter<User> >::iterator i=m.begin(); i!=m.end(); i++)
    {
        v[iUtils->toString(CONTAINER(i->first))].append(i->second->jdump());
    }
    v["SocketsContainerBase"]=SocketsContainerBase::jdump();
    return v;
}

void OscarSecure::__users::on_delete(const REF_getter<epoll_socket_info>&esi, const std::string& )
{
    M_LOCK(m_lock);
    if(!container.count(esi->m_id))
        throw CommonError("if(!container.count(esi->m_id)) %s %d",__FILE__,__LINE__);
    container.erase(esi->m_id);
}
void OscarSecure::__users::on_mod_write(const REF_getter<epoll_socket_info>&)
{
}

