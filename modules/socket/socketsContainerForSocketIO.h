#ifndef _____________socketsContainerForSocketIO_h
#define _____________socketsContainerForSocketIO_h
#include "socketsContainer.h"
#include "epoll_socket_info.h"
#ifdef HAVE_KQUEUE
#include <sys/event.h>
#endif
namespace SocketIO
{
#ifdef KALL
    struct SocketsContainerForSocketIO:public SocketsContainerBase
    {

#ifdef HAVE_EPOLL
        e_poll m_epoll;
#endif
#ifdef HAVE_KQUEUE
        //k_queue m_kqueue;
        int m_kqueue;
        std::vector<struct kevent> evSet;
#endif

        SocketsContainerForSocketIO(): SocketsContainerBase("SocketsContainerForSocketIO") {}
        ~SocketsContainerForSocketIO();

        void addRead(const REF_getter<epoll_socket_info>&esi);
        void addRW(const REF_getter<epoll_socket_info>&esi);
        Json::Value jdump();
        void on_delete(const REF_getter<epoll_socket_info>&esi, const std::string& reason);
        void on_mod_write(const REF_getter<epoll_socket_info>&esi);

        void clear()
        {
            SocketsContainerBase::clear();
#ifdef HAVE_KQUEUE
            evSet.clear();
#endif
        }

    private:
    };
#endif
}
#endif
