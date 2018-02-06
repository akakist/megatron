#ifndef _________________SOCKETSCONTAINER_H
#define _________________SOCKETSCONTAINER_H

#include "REF.h"
#include "mutexable.h"
#include "e_poll.h"
#include "SOCKET_id.h"
#include "json/json.h"
#include "SERVICE_id.h"
class epoll_socket_info;

struct SocketsContainerBase: public Refcountable
{
    const char *name;
private:
    Mutex m_lock;
    std::map<SOCKET_id,REF_getter<epoll_socket_info> > container;
public:
    SocketsContainerBase(const char* _name):name(_name) {}
    void add(const SERVICE_id& sid,const REF_getter<epoll_socket_info> &);
    int count(const SOCKET_id& id);
    int size();
    void clear();
    Json::Value jdump();
    std::map<SOCKET_id,REF_getter<epoll_socket_info> >getContainer();
    REF_getter<epoll_socket_info> getOrNull(const SOCKET_id& sid) const;

    void remove(const SOCKET_id& sid, const std::string& reason);
    virtual void on_delete(const REF_getter<epoll_socket_info>&esi, const std::string& reason)=0;

    void mod_write(const SOCKET_id& sid);
    virtual void on_mod_write(const REF_getter<epoll_socket_info>&esi)=0;

};

#endif // SOCKETSCONTAINER_H
