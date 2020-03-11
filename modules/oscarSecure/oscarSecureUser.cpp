#include "oscarSecureUser.h"

REF_getter<OscarSecure::User> OscarSecure::__users::findOrCreateAndFind(const REF_getter<epoll_socket_info>&esi)
{
    XTRY;
//    REF_getter<User> ret(NULL);
    M_LOCK(m_lock);
    auto i=container.find(esi->m_id);
    if (i!=container.end())
        return i->second;

    REF_getter<User> u= new User(esi);
    container.insert(std::make_pair(esi->m_id,u));
    return u;
    XPASS;
}
REF_getter<OscarSecure::User> OscarSecure::__users::find_throw(const SOCKET_id&s) const
{
    REF_getter<User> ret(NULL);
    M_LOCK(m_lock);
    auto i=container.find(s);
    if (i!=container.end()) return i->second;
    throw CommonError("user not found id=%ld %s %d",CONTAINER(s),__FILE__,__LINE__);
}
OscarSecure::User::User(const REF_getter<epoll_socket_info> &_esi):inited(false),esi(_esi)
{
}


OscarSecure::User::~User()
{
}
void OscarSecure::__users::insert(const REF_getter<User>& bi)
{

    M_LOCK(m_lock);
    if(!container.count(bi->esi->m_id))
    {
        container.insert(std::make_pair(bi->esi->m_id,bi));
    }

}

Json::Value OscarSecure::User::jdump()
{
    Json::Value v;
    v["id"]=std::to_string(CONTAINER(esi->m_id));
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

    for(auto&  i:m)
    {
        v[std::to_string(CONTAINER(i.first))].append(i.second->jdump());
    }
//    v["SocketsContainerBase"]=SocketsContainerBase::jdump();
    return v;
}

void OscarSecure::__users::on_delete(const REF_getter<epoll_socket_info>&esi, const std::string& )
{
    M_LOCK(m_lock);
    if(!container.count(esi->m_id))
        throw CommonError("if(!container.count(esi->m_id)) %s %d",__FILE__,__LINE__);
    container.erase(esi->m_id);
}
