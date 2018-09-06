#include "networkMultiplexor.h"
#ifdef __linux__
#include <sys/epoll.h>
#endif
#include "evfilt_name.h"

NetworkMultiplexor::~NetworkMultiplexor()
{
#ifdef HAVE_EPOLL
    logErr2("close m_socks->m_epoll.m_epollFd %d",m_epoll.m_epollFd);
    if(::close(m_epoll.m_epollFd))
        logErr2("close: %d", errno);
#endif
#ifdef HAVE_KQUEUE
//    logErr2("close ,m_socks->m_kqueue %d",m_kqueue);
    if(::close(m_kqueue))
        logErr2("close: %d %s", errno,strerror(errno));
#endif
}



void NetworkMultiplexor::sockAddReadOnNew(epoll_socket_info* esi)
{

    XTRY;
//    logErr2("@@ %s %ld",__FUNCTION__, CONTAINER(esi->m_id));
#ifdef HAVE_EPOLL
    struct epoll_event evz;
    evz.data.u64 = CONTAINER(esi->m_id);
    evz.events = EPOLLIN;
    int fd=CONTAINER(esi->get_fd());
    if(fd!=-1)
    {
        if (epoll_ctl(m_epoll.m_epollFd, EPOLL_CTL_ADD, fd, &evz) < 0)
        {
            logErr2("epoll_ctl add: socket '%d' - errno %d",CONTAINER(esi->get_fd()), errno);
            if(!esi->closed())
            {
                esi->close("EPOLL_CTL_ADD");
            }
        }
    }
#endif
#ifdef HAVE_KQUEUE

    int fd=CONTAINER(esi->get_fd());
    if(fd!=-1)
    {
        struct kevent ev;
        EV_SET(&ev,fd,EVFILT_READ,EV_ADD,0,0,(void*)(long)CONTAINER(esi->m_id));
        addEvent(ev);
    }
    else throw CommonError("fd==-1");

#endif
//#ifdef HAVE_EPOLL
//    {
//        struct epoll_event evtz;
//        evtz.events=EPOLLIN;
//        evtz.data.u64=CONTAINER(esi->m_id);

//        if (epoll_ctl(esi->multiplexor->m_epoll.m_epollFd, EPOLL_CTL_MOD, CONTAINER(esi->get_fd()), &evtz) < 0)
//        {
//            logErr2("epoll_ctl mod: socket '%d' - errno %d",CONTAINER(esi->get_fd()), errno);
//        }

//    }

//#endif

    XPASS;
}
#ifdef HAVE_KQUEUE
void NetworkMultiplexor::clear()
{
    M_LOCK(this);
    evSet.clear();
}
void NetworkMultiplexor::addEvent(const struct kevent& k)
{
//    logErr2("addEvent %s %s ",EVFILT_name(k.filter).c_str(),EVFLAG_name(k.flags).c_str());
    M_LOCK(this);
    evSet.push_back(k);
}
//void NetworkMultiplexor::init()
//{
//    M_LOCK(this);
//    m_kqueue=kqueue();
//    if(m_kqueue==-1)
//        throw CommonError("kqueue(): errno %d",errno);
//}
size_t NetworkMultiplexor::size()
{
    M_LOCK(this);
    return evSet.size();
}
std::vector<struct kevent> NetworkMultiplexor::extractEvents()
{

    M_LOCK(this);
    auto z=evSet;
    evSet.clear();
    evSet.reserve(1024);
    return z;
}
int NetworkMultiplexor::getKqueue()
{
    return m_kqueue;
}
#endif
