#ifndef ____________oscar_user____H
#define ____________oscar_user____H
#include <REF.h>
#include <st_rsa.h>
#include <epoll_socket_info.h>

namespace OscarSecure
{
    class User: public Refcountable, public Mutexable
    {

    public:
        st_rsa rsa;
        st_AES aes;
        bool inited;
        REF_getter<epoll_socket_info> esi;
        User(const REF_getter<epoll_socket_info>& _esi);
        ~User();
        std::string m_cache;
        Json::Value jdump();
    };
    class __users: public Refcountable
    {
        Mutex m_lock;
        std::map<SOCKET_id,REF_getter<User> > container;
    public:
        __users()
        {}
        REF_getter<User> findOrCreateAndFind(const REF_getter<epoll_socket_info> &esi);
        REF_getter<User> find_throw(const SOCKET_id&) const;
        void insert(const REF_getter<User>& bi);
        Json::Value jdump();
        void on_delete(const REF_getter<epoll_socket_info>&esi, const std::string& reason);
        void clear()
        {
//            SocketsContainerBase::clear();
            {
                M_LOCK(m_lock);
                container.clear();
            }
        }

    };

}

#endif
