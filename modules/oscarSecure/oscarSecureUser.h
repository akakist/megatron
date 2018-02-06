#ifndef ____________oscar_user____H
#define ____________oscar_user____H
#include "REF.h"
#include "mutexable.h"
#include "SOCKET_id.h"
#include "st_rsa.h"
#include "epoll_socket_info.h"
#include "IInstance.h"
#include "socketsContainer.h"
namespace OscarSecure
{
    class User: public Refcountable, public Mutexable
    {

    public:
        const SOCKET_id socketId;
        bool isServer;
        st_rsa rsa;
        st_AES aes;
        bool inited;
        User(const SOCKET_id& id, bool _isServer);
        ~User();
        std::string m_cache;
        Json::Value jdump();
    };
    class __users:public SocketsContainerBase
    {
        Mutex m_lock;
        std::map<SOCKET_id,REF_getter<User> > container;
    public:
        __users(): SocketsContainerBase("oscarSecureServiceUsers") {}
        REF_getter<User> find(const SOCKET_id&) const;
        REF_getter<User> find_throw(const SOCKET_id&) const;
        void insert(const REF_getter<User>& bi);
        Json::Value jdump();
        void on_delete(const REF_getter<epoll_socket_info>&esi, const std::string& reason);
        void on_mod_write(const REF_getter<epoll_socket_info>&esi);
        void clear()
        {
            SocketsContainerBase::clear();
            {
            M_LOCK(m_lock);
            container.clear();
            }
        }

    };

}

#endif
