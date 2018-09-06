#ifndef _______________NETWORKMULTIPLEXOR_H
#define _______________NETWORKMULTIPLEXOR_H

#include "e_poll.h"
#include "unistd.h"
#include <vector>
#ifdef __FreeBSD__
#include <sys/types.h>
 #include <sys/event.h>
 #include <sys/time.h>
#endif
#ifdef __MACH__
#include <sys/select.h>
#include <sys/event.h>
#endif
#include "REF.h"
#include "mutexable.h"
#include "epoll_socket_info.h"

#if defined(HAVE_EPOLL) || defined(HAVE_SELECT) || defined(HAVE_KQUEUE)
#else
#error you must specify HAVE_EPOLL or HAVE_SELECT
#endif

struct NetworkMultiplexor: public Refcountable, public Mutexable
{
#ifdef HAVE_EPOLL
    e_poll m_epoll;
#endif
private:
#ifdef HAVE_KQUEUE
    //k_queue m_kqueue;
    int m_kqueue;
    std::vector<struct kevent> evSet;
public:
    NetworkMultiplexor()
    {
            m_kqueue=kqueue();
            if(m_kqueue==-1)
                throw CommonError("kqueue(): errno %d",errno);
    }
//    ~NetworkMultiplexor()
//    {

//    }
#endif
public:
#ifdef HAVE_KQUEUE
    void clear();
    void addEvent(const struct kevent& k);
//    void init();
    size_t size();
    std::vector<struct kevent> extractEvents();
    int getKqueue();
#endif
#ifdef     HAVE_EPOLL
//    std::vector<std::pair<SOCKET_fd,struct epoll_event> > evSet;
//    void addEvent(SOCKET_fd fd, const struct epoll_event& k);
//    std::vector<std::pair<SOCKET_fd,struct epoll_event> > extractEvents();
#endif

    ~NetworkMultiplexor();

    void sockAddReadOnNew(epoll_socket_info *esi);



};


#endif // NETWORKMULTIPLEXOR_H
