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
class epoll_socket_info;
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
    int m_kqueue;
    std::vector<struct kevent> evSet;
public:
    NetworkMultiplexor()
    {
        m_kqueue=kqueue();
        if(m_kqueue==-1)
            throw CommonError("kqueue(): errno %d",errno);
    }
#endif
public:
#ifdef HAVE_KQUEUE
    void clear();
    void addEvent(const struct kevent& k);
    size_t size();
    std::vector<struct kevent> extractEvents();
    int getKqueue();
#endif
#ifdef     HAVE_EPOLL
#endif

    ~NetworkMultiplexor();

    void sockAddReadOnNew(epoll_socket_info *esi);



};


#endif // NETWORKMULTIPLEXOR_H
