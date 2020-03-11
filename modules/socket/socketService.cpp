#ifndef _WIN32
#include <sys/ioctl.h>
#ifdef __linux__
#include <sys/epoll.h>
#endif
#include <sys/socket.h>
#include <arpa/inet.h>
#else
#include "compat_win32.h"
#endif
#include <sys/types.h>
#include "socketService.h"
#include <unistd.h>

#include <unistd.h>
#ifdef __MACH__
#include <sys/select.h>
#include <sys/event.h>
#endif
#include "st_malloc.h"
#ifndef _WIN32
#include <sys/ioctl.h>
#include <colorOutput.h>
#include <logging.h>
#include <tools_mt.h>
#include <Events/System/Net/socket/Accepted.h>
#include <Events/System/Net/socket/StreamRead.h>
#include <Events/System/Net/socket/Connected.h>
#include <Events/System/Net/socket/NotifyOutBufferEmpty.h>

#endif
#include "version_mega.h"
#include "Events/Tools/webHandler/RegisterHandler.h"
#include "Events/System/Net/socket/Connected.h"
#include "Events/System/Net/socket/NotifyBindAddress.h"
#include "Events/System/Net/socket/ConnectFailed.h"
#include "Events/System/Net/socket/Disaccepted.h"
#include "Events/System/Net/socket/Disconnected.h"
#include "events_socket.hpp"
#include "colorOutput.h"



SocketIO::Service::~Service()
{
    m_isTerminating=true;
    int err=pthread_join(m_pthread_id_worker,NULL);
    if(err)
    {
        printf(RED("%s pthread_join: %s"),__PRETTY_FUNCTION__,strerror(errno));
    }


    m_socks->clear();
}

SocketIO::Service::Service(const SERVICE_id& id, const std::string& nm, IInstance* ifa):
    UnknownBase(nm),Broadcaster(ifa),
    ListenerBuffered1Thread(this,nm,ifa->getConfig(),id,ifa),
    m_total_recv(0L),
    m_total_send(0L),
    m_total_accepted(0L),

    m_socks(new SocketsContainerForSocketIO),m_listen_backlog(128),
    maxOutBufferSize(ifa->getConfig()->get_int64_t("maxOutBufferSize",8*1024*1024,"")),
    iInstance(ifa),m_isTerminating(false)
{
    try {
#ifdef HAVE_EPOLL
        epoll_timeout_millisec= static_cast<int>(ifa->getConfig()->get_int64_t("timeout_millisec",
                                10, ""));
        epoll_size= static_cast<int>(ifa->getConfig()->get_int64_t("size", 1024, ""));

        m_socks->multiplexor->m_epoll.m_epollFd=epoll_create(epoll_size);
        if (m_socks->multiplexor->m_epoll.m_epollFd<0) throw CommonError("epoll_create: errno %d",errno);
#endif
#ifdef HAVE_KQUEUE
        kqueue_timeout_millisec=ifa->getConfig()->get_int64_t("timeout_millisec",10,"");
        kqueue_size=ifa->getConfig()->get_int64_t("size",1024,"");

#endif
        m_listen_backlog=ifa->getConfig()->get_int64_t("listen_backlog",128,"");

        if(pthread_create(&m_pthread_id_worker,NULL,worker__,this))
        {
            throw CommonError("pthread_create: errno %d",errno);
        }
    }
    catch(...)
    {
        logErr2("failed init service socket");
    }

}
void SocketIO::Service::onEPOLLIN_SOCK_DGRAM(const REF_getter<epoll_socket_info>&esi)
{
    MUTEX_INSPECTOR;
    S_LOG(__FUNCTION__);

    char buf[0x10000];
    char sockaddBuf[100];
    socklen_t sockaddBufLen=sizeof(sockaddBuf);
    int r=recvfrom(CONTAINER(esi->get_fd()),buf,sizeof(buf),0,(sockaddr*)sockaddBuf,&sockaddBufLen);
    if(r<0)
    {
        logErr2("recvfrom: %s",strerror(errno));
        closeSocket(esi,std::string("recvfrom:")+strerror(errno),errno);
        return;
    }
    msockaddr_in sa;
    memcpy(&sa.u, sockaddBuf, static_cast<size_t>(sockaddBufLen));
    passEvent(new socketEvent::UdpPacketIncoming(esi,sa,0,std::string(buf,r),poppedFrontRoute(esi->m_route)));

}
void SocketIO::Service::onEPOLLIN_STREAMTYPE_LISTENING(const REF_getter<epoll_socket_info>&esi)
{
    MUTEX_INSPECTOR;
    S_LOG(__FUNCTION__);
    msockaddr_in neu_sa;
    socklen_t len=neu_sa.maxAddrLen();
    SOCKET_fd neu_fd = ::accept(CONTAINER(esi->get_fd()), neu_sa.addr(),&len);

    DBG(logErr2("accept: fd=%d",neu_fd));
    if (CONTAINER(neu_fd) < 0)
    {
        logErr2("accept: %d errno %d %s", CONTAINER(neu_fd),errno,strerror(errno));
        return;
    }
    msockaddr_in local_name;
    msockaddr_in remote_name;
    socklen_t sl=local_name.maxAddrLen();
    socklen_t sr=remote_name.maxAddrLen();
    if(getsockname(CONTAINER(neu_fd), local_name.addr(), &sl))
    {
        logErr2("getsockname: errno %d %s (%s %d)",errno,strerror(errno),__FILE__,__LINE__);
    }
    if(getpeername(CONTAINER(neu_fd), remote_name.addr(), &sr))
    {
        logErr2("getsockname: errno %d %s (%s %d)",errno,strerror(errno),__FILE__,__LINE__);
    }


//    XTRY;
    m_total_accepted++;
    if (1)
    {
        XTRY;
#ifdef _WIN32
        u_long f = 1;
        int r=ioctlsocket(CONTAINER(neu_fd), FIONBIO, &f);
#else
        int f = 1;
        int r=ioctl(CONTAINER(neu_fd), FIONBIO, &f);
#endif
        if (r==-1)
        {
            logErr2("ioctl: errno %d",errno);
        }
        XPASS;

#ifdef USE_SO_LINGER
        struct linger l_;
        l_.l_onoff=1;
        l_.l_linger=(u_short)5;
        if (::setsockopt(CONTAINER(neu_fd),SOL_SOCKET,SO_LINGER,(char*)&l_, sizeof(l_))!=0)
        {
            logErr2("setsockopt: linger: errno %d",errno);
        }
#endif


    }
    SOCKET_id _sid=iUtils->getSocketId();
    REF_getter<epoll_socket_info> nesi=new epoll_socket_info(SOCK_STREAM,epoll_socket_info::STREAMTYPE_ACCEPTED,_sid,neu_fd,esi->m_route,
//                m_socks.___ptr,
            maxOutBufferSize, esi->socketDescription,esi->bufferVerify,m_socks->multiplexor);


    nesi->local_name=local_name;
    nesi->remote_name=remote_name;

    m_socks->multiplexor->sockAddReadOnNew(nesi.operator ->());
    m_socks->add(nesi);

    route_t r=nesi->m_route;
    r.pop_front();
    passEvent(new socketEvent::Accepted(nesi,r));
#ifdef HAVE_KQUEUE
    struct kevent ev1;
    EV_SET(&ev1,CONTAINER(esi->get_fd()),EVFILT_READ,EV_ADD|EV_CLEAR,0,0,(void*)(long)CONTAINER(esi->m_id));
    m_socks->multiplexor->addEvent(ev1);
#endif


}
void SocketIO::Service::onEPOLLIN_STREAMTYPE_CONNECTED_or_STREAMTYPE_ACCEPTED(const REF_getter<epoll_socket_info>&esi)
{
    MUTEX_INSPECTOR;
    S_LOG(__FUNCTION__);
//    logErr2("z@@");
    XTRY;
#define READBUFSIZE 1024*1024*4
    st_malloc buf(READBUFSIZE);
    int r=0;

    int fd=CONTAINER(esi->get_fd());
    if(fd==-1)
    {
        throw CommonError("invalid case %s %d",__FILE__,__LINE__);
        return;
    }
    r=::recv(fd,(char*)buf.buf,READBUFSIZE,0);
    if (r==0 )
    {
        XTRY;
//        logErr2("recv 0");
        int err=errno;
//#ifndef __linux__
        closeSocket(esi,"recv 0, resetted by peer",err);
//#endif
        return;
        XPASS;

    }
    if (r==-1)
    {
        if(errno)
        {
            DBG(logErr2("recv: errno %d %s",errno,strerror(errno)));
        }
#ifdef _WIN32
        closeSocket(esi,"SocketService: close due recv return 0",errno);
//        if(!esi->closed())
//        {
//            esi->close("SocketService: close due recv return 0");
//        }
#endif
        std::string errText;
        bool needClose=false;
        XTRY;
        switch (errno)
        {
        case EBADF:
        case ECONNREFUSED:
        case ENOTCONN:
        case ENOTSOCK:
#ifndef _WIN32
        case ECONNRESET:
#endif
        {
            needClose=true;
            errText=(std::string)"SocketService: case EBADF:ECONNREFUSED:ENOTCONN:ENOTSOCK:ECONNRESET: "+strerror(errno);
            XTRY;
            return;
            XPASS;
        }
        default:
            needClose=true;
            errText=(std::string)"SocketService: case default "+strerror(errno);
            return;
        }
        if(needClose)
            closeSocket(esi,errText,errno);
        XPASS;
    }
    if(r>=0)
    {
#ifdef WITH_TCP_STATS
        m_stats.addIn(esi,r);
#endif
        {
            XTRY;
            esi->m_inBuffer.append((char*)buf.buf,r);
            XPASS;
        }
        m_total_recv+=r;
        {
            route_t rt=esi->m_route;
            rt.pop_front();
            bool verified=true;
            if(esi->bufferVerify)
            {

                M_LOCK(esi->m_inBuffer);
                verified=esi->bufferVerify(esi->m_inBuffer._mx_data);
            }
            else
            {
                DBG(logErr2("!if(esi->bufferVerify)"));
                verified=true;
            }
            if(verified)
            {
                passEvent(new socketEvent::StreamRead(esi,rt));
            }

        }
#ifdef HAVE_KQUEUE
        struct kevent ev1;
        EV_SET(&ev1,CONTAINER(esi->get_fd()),EVFILT_READ,EV_ADD|EV_CLEAR,0,0,(void*)(long)CONTAINER(esi->m_id));
        m_socks->multiplexor->addEvent(ev1);
#endif

    }
    XPASS;

}

void SocketIO::Service::onEPOLLIN(const REF_getter<epoll_socket_info>&esi)
{
    MUTEX_INSPECTOR;
    S_LOG(__FUNCTION__);
    XTRY;

    if(esi->m_socketType==SOCK_DGRAM)
    {
        onEPOLLIN_SOCK_DGRAM(esi);
        return;
    }

    if (esi->m_streamType==epoll_socket_info::STREAMTYPE_LISTENING)
    {
        XTRY;
        onEPOLLIN_STREAMTYPE_LISTENING(esi);
        XPASS;
    }
    else if (esi->m_streamType==epoll_socket_info::STREAMTYPE_CONNECTED)
    {
        onEPOLLIN_STREAMTYPE_CONNECTED_or_STREAMTYPE_ACCEPTED(esi);

    }
    else if(esi->m_streamType==epoll_socket_info::STREAMTYPE_ACCEPTED)
    {
        onEPOLLIN_STREAMTYPE_CONNECTED_or_STREAMTYPE_ACCEPTED(esi);
    }

    XPASS

}
void SocketIO::Service::onEPOLLOUT(const REF_getter<epoll_socket_info>&__EV_)
{

    MUTEX_INSPECTOR;

    S_LOG(__FUNCTION__);

    XTRY;
    const REF_getter<epoll_socket_info>& esi=__EV_;
    if(esi->m_socketType!=SOCK_STREAM)
    {
        throw CommonError("if(esi->m_socketType!=SOCK_STREAM) %s",esi->socketDescription.c_str());
    }
    if(esi->inConnection)
    {
        S_LOG("esi->inConnection");

        XTRY;
        if(!m_socks->count(esi->m_id))
        {
            logErr2("socket not found %d (%s %d)",CONTAINER(esi->m_id),__FILE__,__LINE__);
            return;
        }

        msockaddr_in local_name;
        msockaddr_in remote_name;
        socklen_t sl=local_name.maxAddrLen();
        socklen_t sr=remote_name.maxAddrLen();
        if(getsockname(CONTAINER(esi->get_fd()), local_name.addr(), &sl))
        {
            throw CommonError("getsockname: errno fd=%d %d %s (%s %d)",CONTAINER(esi->get_fd()),errno,strerror(errno),__FILE__,__LINE__);
        }
        if(getpeername(CONTAINER(esi->get_fd()),  remote_name.addr(), &sr))
        {
            return;
            throw CommonError("getpeername: errno fd=%d %d %s (%s %d)",CONTAINER(esi->get_fd()),errno,strerror(errno),__FILE__,__LINE__);
        }
        esi->local_name=local_name;
        if(esi->remote_name!=remote_name)
            logErr2("if(esi->remote_name!=remote_name)!!! TODO");
        esi->remote_name=remote_name;
        route_t r=esi->m_route;
//        logErr2("emit Connected");
        passEvent(new socketEvent::Connected(esi, poppedFrontRoute(r)));
        esi->inConnection=false;
#ifdef HAVE_KQUEUE
        {
            struct kevent ev;
            EV_SET(&ev,CONTAINER(esi->get_fd()),EVFILT_WRITE,EV_DELETE|EV_CLEAR,0,0,(void*)(long)CONTAINER(esi->m_id));
            m_socks->multiplexor->addEvent(ev);
        }
        {
            struct kevent ev;
            EV_SET(&ev,CONTAINER(esi->get_fd()),EVFILT_READ,EV_ADD|EV_CLEAR,0,0,(void*)(long)CONTAINER(esi->m_id));
            m_socks->multiplexor->addEvent(ev);
        }
#endif
#ifdef HAVE_EPOLL
        {
            struct epoll_event evtz {};
            evtz.events=EPOLLIN;
            evtz.data.u64= static_cast<uint64_t>(CONTAINER(esi->m_id));

            if (epoll_ctl(m_socks->multiplexor->m_epoll.m_epollFd, EPOLL_CTL_MOD, CONTAINER(esi->get_fd()), &evtz) < 0)
            {
                logErr2("epoll_ctl mod: socket '%d' - errno %d",CONTAINER(esi->get_fd()), errno);
            }

        }

#endif

        return;
        XPASS;
    }


    bool need_to_close=false;
    {
        MUTEX_INSPECTOR;
        XTRY;
        int res=0;
        {
            size_t sz;
            if(esi->closed()) return;
            if(CONTAINER(esi->get_fd())==-1)
                return;
            sz=esi->m_outBuffer.size();
            if(sz)
            {
                res=esi->m_outBuffer.send(esi->get_fd());
                if(res>0)
                {
#ifdef WITH_TCP_STATS
                    m_stats.addOut(esi,res);
#endif
                    m_total_send+=res;
                    if (res!=(int)sz)
                    {
                        XTRY;
                        XPASS;
                    }
                    else
                    {
                        XTRY;
                        if (esi->markedToDestroyOnSend)
                        {
                            need_to_close=true;
                            DBG(logErr2("set need_to_close=true"));

                        }
                        XPASS;
                    }

                    int fd=CONTAINER(esi->get_fd());
#ifdef HAVE_KQUEUE
                    if(fd!=-1)
                    {
                        if(esi->m_outBuffer.size()==0)
                        {

                            struct kevent ev;
                            EV_SET(&ev,fd,EVFILT_WRITE,EV_DELETE|EV_CLEAR,0,0,(void*)(long)CONTAINER(esi->m_id));
                            m_socks->multiplexor->addEvent(ev);

                        }
                    }
#endif
#ifdef HAVE_EPOLL
                    {

                        if(esi->m_outBuffer.size()==0 && fd!=-1)
                        {
                            struct epoll_event evtz {};
                            evtz.events=EPOLLIN;
                            evtz.data.u64= static_cast<uint64_t>(CONTAINER(esi->m_id));
                            if (epoll_ctl(m_socks->multiplexor->m_epoll.m_epollFd, EPOLL_CTL_MOD, CONTAINER(esi->get_fd()), &evtz) < 0)
                            {
                                logErr2("epoll_ctl mod: socket '%d' - errno %d",CONTAINER(esi->get_fd()), errno);
                            }
                        }

                    }

#endif

                }
                if(esi->m_outBuffer.size()==0)
                {
                    route_t r=esi->m_route;
                    r.pop_front();
                    passEvent(new socketEvent::NotifyOutBufferEmpty(esi, r));
                }
            }
        }
        if(need_to_close)
        {
            XTRY;
            closeSocket(esi,"marked to destroy on send",0);
            return;
            XPASS;
        }
        if (res==-1)
        {
            XTRY;
            DBG(logErr2("send: errno %d",errno));
            switch(errno)
            {
#ifndef _WIN32
            case ENOBUFS:
#endif
            case EAGAIN:
                return;
            default:
            {
//                esi->close("send result (-1)");
                closeSocket(esi,"send result (-1)",0);

            }
            }
            /**                EBADF   Указан неверный дескриптор.

                            ENOTSOCK
                                    Аргумент s не является сокетом.

                            EFAULT  В качестве параметра передан неверный адрес.

                            EMSGSIZE
                                    Сокет  требует,  чтобы  сообщение  было  отослано  за   одну   операцию
                                    (атомарно), а размер сообщения не позволяет этого.

                            EAGAIN или EWOULDBLOCK
                                    Сокет  помечен как неблокирующий, а запрошенная операция должна была бы
                                    заблокировать его.

                            ENOBUFS Исходящая очередь сетевого интерфейса заполнена.  Обычно это  означает,
                                    что интерфейс прекратил отправку, но может быть также вызвано временной
                                    перегрузкой.  (Этого не может произойти под  Linux,  потому  что  здесь
                                    пакеты просто отбрасываются, когда очередь устройства переполняется.)

                            EINTR   Появился сигнал.

                            ENOMEM  Не хватило памяти.

                            EINVAL  Передан неверный аргумент.

                            EPIPE   Локальный  конец сокета, ориентированного на соединение, был закрыт.  В
                                    этом случае процесс  также  получит  сигнал  SIGPIPE,  если  только  не
                                    установлен флаг MSG_NOSIGNAL.*/

            XPASS;
            return;
        }
        XPASS;
    }
    XPASS;
}
void SocketIO::Service::onEPOLLERR(const REF_getter<epoll_socket_info>& esi)
{
    MUTEX_INSPECTOR;

    S_LOG(__FUNCTION__);

    if(esi->m_socketType!=SOCK_STREAM)
    {
        throw CommonError("if(esi->m_socketType!=SOCK_STREAM) %s %s",esi->socketDescription.c_str(),__PRETTY_FUNCTION__);
    }

    XTRY;

    int       error = 0;
    socklen_t errlen = sizeof(error);
    if (getsockopt(CONTAINER(esi->get_fd()), SOL_SOCKET, SO_ERROR,
#ifdef _WIN32
                   (char *)&error,
#else
                   (void *)&error,
#endif
                   &errlen) == 0)
    {
        if(error!=0)
        {
#ifndef __FreeBSD__
//            printf("error = %s\n", strerror(error));
#endif
        }
    }

    closeSocket(esi,"EPOLERR",error);


    XPASS;
}
void *SocketIO::Service::worker__(void*p)
{
    auto* obj=(SocketIO::Service*)p;
    obj->worker();
    return nullptr;
}

void SocketIO::Service::worker()
{
#if defined __MACH__
    pthread_setname_np("SocketIO");
#else
#if !defined _WIN32 && !defined __FreeBSD__
    pthread_setname_np(pthread_self(),"SocketIO");
#endif
#endif

    //
#ifdef HAVE_EPOLL
    struct epoll_event events[m_socks->multiplexor->m_epoll.size+10];
#endif
#ifdef HAVE_KQUEUE
//    m_socks->multiplexor->init();
#endif
    while (1)
    {
        XTRY;
        try
        {
#ifdef HAVE_SELECT

            if(m_isTerminating) return NULL;
            std::map<SOCKET_id,REF_getter<epoll_socket_info> > cc=m_socks->getContainer();
            fd_set readfds;
            fd_set writefds;
            fd_set exceptfds;
            FD_ZERO(&readfds);
            FD_ZERO(&writefds);
            FD_ZERO(&exceptfds);
            std::set<int> socklist;
            auto i=cc.begin();
            i!=cc.end();
            i++)
            {
                REF_getter<epoll_socket_info> esi=i->second;
                if(CONTAINER(esi->get_fd())!=-1)
                {
                    FD_SET(CONTAINER(esi->get_fd()),&readfds);
                    FD_SET(CONTAINER(esi->get_fd()),&exceptfds);
                    if(esi->m_outBuffer.size())
                        FD_SET(CONTAINER(esi->get_fd()),&writefds);
                    socklist.insert(CONTAINER(esi->get_fd()));


                }
            }
            if(socklist.size())
            {
                struct timeval tv;
                tv.tv_sec=0;
                tv.tv_usec=10000;

                int r=::select(*socklist.rbegin()+1,&readfds,&writefds,&exceptfds,&tv);
                if(r>0)
                {
                    for(auto i:cc)
                    {
                        REF_getter<epoll_socket_info> esi=i.second;
                        if(FD_ISSET(CONTAINER(esi->get_fd()),&readfds))
                            obj->onEPOLLIN(esi);
                        if(FD_ISSET(CONTAINER(esi->get_fd()),&writefds))
                            obj->onEPOLLOUT(esi);
                        if(FD_ISSET(CONTAINER(esi->get_fd()),&exceptfds))
                        {
                            obj->onEPOLLERR(esi);
                            m_socks->remove(esi->m_id,"exceptfds on");
                        }

                    }
                }
                else
                {
                    if(r==-1)
                    {
                        if(errno==EBADF)
                        {

                        }
                        logErr2("select: errno %d",errno);
                    }
                }
            }
#endif
#ifdef HAVE_EPOLL

            if(m_isTerminating) return ;
//            if(iUtils->isTerminating()) return NULL;
            int nfds;
        {
            nfds=epoll_wait(m_socks->multiplexor->m_epoll.m_epollFd,events,m_socks->multiplexor->m_epoll.size,m_socks->multiplexor->m_epoll.timeout_millisec);
            }

            int __ERRNO=errno;
            if(iUtils->isTerminating())
                return ;
                if (nfds==-1)
            {
                if(__ERRNO==EINTR)
                    {
                        continue;
                    }
                    logErr2("epoll_wait: __ERRNO %d",__ERRNO);


                    std::map<SOCKET_id,REF_getter<epoll_socket_info> >cc=m_socks->getContainer();
                    for(auto & i : cc)
                    {

                        REF_getter<epoll_socket_info> &ss=i.second;

                        if(ss->m_streamType==epoll_socket_info::STREAMTYPE_LISTENING)
                        {
                            closeSocket(ss,"epoll_wait failed",__ERRNO);
                            m_socks->remove(ss->m_id);
                            const REF_getter<socketEvent::AddToListenTCP> e=new socketEvent::AddToListenTCP(ss->m_id,ss->local_name,ss->socketDescription,true,ss->bufferVerify,ss->m_route);
                            on_AddToListenTCP(e.operator ->());
                        }
                    }
                    continue;
                }
            for (int i=0; i<nfds; i++)
        {
            XTRY;
            SOCKET_id id;
            CONTAINER(id)=events[i].data.u64;
                REF_getter<epoll_socket_info> __EV=m_socks->getOrNull(id);
                if (!__EV.valid())
                {
                    continue;
                }
                if (events[i].events&EPOLLERR && !__EV->closed())
                {
                    onEPOLLERR(__EV);
                }
                if (events[i].events&EPOLLIN && !__EV->closed())
                {
                    onEPOLLIN(__EV);
                }
                if (events[i].events&EPOLLOUT && !__EV->closed())
                {
                    onEPOLLOUT(__EV);
                    //__EV->lastIO=time(NULL);
                }
                XPASS;
            }
#endif
#ifdef HAVE_KQUEUE
            if(m_isTerminating) return;

            timespec ts;
            ts.tv_sec=0;
            ts.tv_nsec=kqueue_timeout_millisec*1000;
            auto evs=m_socks->multiplexor->extractEvents();


            struct kevent evList[1024];

            int nev = kevent(m_socks->multiplexor->getKqueue(), &evs[0], evs.size(), evList, 1024, &ts);
//            logErr2("kevent result: %d %s", errno, strerror(errno));
                if(m_isTerminating) return;
                if (nev < 0)
                {
//                int _errno=errno;
                    logErr2("kevent error: %d %s", errno, strerror(errno));
                        continue;

                    }
            for (int i=0; i<nev; i++) {
            SOCKET_id sid;
            CONTAINER(sid)=(long)evList[i].udata;
                REF_getter<epoll_socket_info> esi=m_socks->getOrNull(sid);
                if(!esi.valid())
                {
                    if(CONTAINER(sid)!=-1)
                    {
                        DBG(logErr2("kevent: esi == NULL sid %d",CONTAINER(sid)));
                    }
                    continue;
                }
                auto& l=evList[i];
//                logErr2("fd %d id %d",CONTAINER(esi->get_fd()),CONTAINER(esi->m_id));

//                logErr2("NEV %s %s (%d %d)",EVFILT_name(l.filter).c_str(),EVFLAG_name(l.flags).c_str(),l.filter,l.flags);

                if(l.filter==EVFILT_READ)
                {
                    S_LOG("EVFILT_READ");
//                    if(l.flags&EV_ADD)
                    {
                        S_LOG("EV_ADD");
                        if(l.flags & EV_EOF)
                        {
                            S_LOG("EV_EOF");
#ifdef HAVE_KQUEUE
                            struct kevent ev1,ev2;
                            EV_SET(&ev1,CONTAINER(esi->get_fd()),EVFILT_READ,EV_DELETE|EV_CLEAR,0,0,(void*)(long)CONTAINER(esi->m_id));
                            EV_SET(&ev2,CONTAINER(esi->get_fd()),EVFILT_WRITE,EV_DELETE|EV_CLEAR,0,0,(void*)(long)CONTAINER(esi->m_id));
                            m_socks->multiplexor->addEvent(ev1);
                            m_socks->multiplexor->addEvent(ev2);
#endif
                            if(!esi->closed())
                                closeSocket(esi,"EV_EOF",l.data);
                            m_socks->remove(esi->m_id);
                        }
                        else if(l.flags & EV_ERROR)
                        {
                            S_LOG("EV_ERROR");
#ifdef HAVE_KQUEUE
                            struct kevent ev1,ev2;
                            EV_SET(&ev1,CONTAINER(esi->get_fd()),EVFILT_READ,EV_DELETE|EV_CLEAR,0,0,(void*)(long)CONTAINER(esi->m_id));
                            EV_SET(&ev2,CONTAINER(esi->get_fd()),EVFILT_WRITE,EV_DELETE|EV_CLEAR,0,0,(void*)(long)CONTAINER(esi->m_id));
                            m_socks->multiplexor->addEvent(ev1);
                            m_socks->multiplexor->addEvent(ev2);
#endif
//                            logErr2("closing: id %d  err %d errs %s",CONTAINER(esi->m_id),l.data,strerror(l.data));
                            if(!esi->closed())
                                closeSocket(esi,"EV_EOF",l.data);
                            m_socks->remove(esi->m_id);
                        }
                        else
                        {
                            onEPOLLIN(esi);
                            continue;
                        }
                    }

                }
                else if(l.filter==EVFILT_WRITE)
                {

                    S_LOG("EVFILT_WRITE");
//                    if(l.flags&EV_ADD)
                    {
                        S_LOG("EV_ADD");
                        if(l.flags & EV_EOF)
                        {
                            S_LOG("EV_EOF");
#ifdef HAVE_KQUEUE
                            struct kevent ev1,ev2;
                            EV_SET(&ev1,CONTAINER(esi->get_fd()),EVFILT_READ,EV_DELETE|EV_CLEAR,0,0,(void*)(long)CONTAINER(esi->m_id));
                            EV_SET(&ev2,CONTAINER(esi->get_fd()),EVFILT_WRITE,EV_DELETE|EV_CLEAR,0,0,(void*)(long)CONTAINER(esi->m_id));
                            m_socks->multiplexor->addEvent(ev1);
                            m_socks->multiplexor->addEvent(ev2);
#endif

                            if(!esi->closed())
                                closeSocket(esi,"EV_EOF",0);
                            m_socks->remove(esi->m_id);
                        }
                        else if(l.flags & EV_ERROR)
                        {
                            S_LOG("EV_ERROR");
#ifdef HAVE_KQUEUE
                            struct kevent ev1,ev2;
                            EV_SET(&ev1,CONTAINER(esi->get_fd()),EVFILT_READ,EV_DELETE|EV_CLEAR,0,0,(void*)(long)CONTAINER(esi->m_id));
                            EV_SET(&ev2,CONTAINER(esi->get_fd()),EVFILT_WRITE,EV_DELETE|EV_CLEAR,0,0,(void*)(long)CONTAINER(esi->m_id));
                            m_socks->multiplexor->addEvent(ev1);
                            m_socks->multiplexor->addEvent(ev2);
#endif
//                            logErr2("closing: id %d  err %d errs %s",CONTAINER(esi->m_id),l.data,strerror(l.data));
                            if(!esi->closed())
                                closeSocket(esi,"EV_EOF",l.data);
                            m_socks->remove(esi->m_id);
                        }
                        else
                        {
                            onEPOLLOUT(esi);
                            continue;
                        }
                    }
//                    else logErr2("invalid case %s %d",__FILE__,__LINE__);


                }
                else logErr2("invalid case %s %d",__FILE__,__LINE__);

            }
#endif
        }
        catch(std::exception &e)
        {
            logErr2("exception: %s %s %d",e.what(),__FILE__,__LINE__);
        }
        XPASS;
    }
}
void SocketIO::Service::closeSocket(const REF_getter<epoll_socket_info>&esi,const std::string &reason, int errNo)
{
    MUTEX_INSPECTOR;
    if(esi->closed())
    {
        logErr2("@@  socket already closed %s",reason.c_str());
        return;
    }

    if(esi->m_streamType==epoll_socket_info::STREAMTYPE_CONNECTED)
    {
        if(esi->inConnection)
        {
            passEvent(new socketEvent::ConnectFailed(esi,esi->remote_name,errNo,poppedFrontRoute(esi->m_route)));
        }
        else
        {
            passEvent(new socketEvent::Disconnected(esi,reason,poppedFrontRoute(esi->m_route)));
        }
    }
    else if(esi->m_streamType==epoll_socket_info::STREAMTYPE_ACCEPTED)
    {
        passEvent(new socketEvent::Disaccepted(esi,reason,poppedFrontRoute(esi->m_route)));

    }
    else throw CommonError("invalid case %s %d",__FILE__,__LINE__);



    XTRY;

    m_socks->remove(esi->m_id);
    DBG(logErr2("socks remove %s",reason.c_str()));
    esi->close(reason);
    XPASS;
}
bool  SocketIO::Service::on_startService(const systemEvent::startService*)
{
    XTRY;


    if(iUtils->isServiceRegistered(ServiceEnum::WebHandler))
    {
        sendEvent(ServiceEnum::WebHandler, new webHandlerEvent::RegisterHandler("socket","Socket",ListenerBase::serviceId));
    }

    XPASS;
    return true;
}
bool SocketIO::Service::on_TickTimer(const timerEvent::TickTimer*e)
{
    return true;
}


UnknownBase* SocketIO::Service::construct(const SERVICE_id& id, const std::string&  nm, IInstance* ifa)
{
    XTRY;
    return new Service(id,nm,ifa);
    XPASS;
}





bool  SocketIO::Service::handleEvent(const REF_getter<Event::Base>&e)
{
    MUTEX_INSPECTOR;
    XTRY;
    auto &ID=e->id;
    if(timerEventEnum::TickTimer==ID)
        return on_TickTimer((const timerEvent::TickTimer*)e.operator->());
    if(socketEventEnum::AddToListenTCP==ID)
        return on_AddToListenTCP((const socketEvent::AddToListenTCP*)e.operator->());
    if(socketEventEnum::AddToConnectTCP==ID)
        return on_AddToConnectTCP((const socketEvent::AddToConnectTCP*)e.operator->());
    if(socketEventEnum::UdpAssoc==ID)
        return on_UdpAssoc((const socketEvent::UdpAssoc*)e.operator->());
    if(socketEventEnum::Write==ID)
        return on_Write((const socketEvent::Write*)e.operator->());
    if(webHandlerEventEnum::RequestIncoming==ID)
        return on_RequestIncoming((const webHandlerEvent::RequestIncoming*)e.operator->());
    if(systemEventEnum::startService==ID)
        return on_startService((const systemEvent::startService*)e.operator->());

    XPASS;
    return false;
}

void registerSocketModule(const char* pn)
{
    if(pn)
    {
        iUtils->registerPlugingInfo(COREVERSION,pn,IUtils::PLUGIN_TYPE_SERVICE,ServiceEnum::Socket,"SocketIO",getEvents_socket());
    }
    else
    {
        iUtils->registerService(COREVERSION,ServiceEnum::Socket,SocketIO::Service::construct,"SocketIO");
        regEvents_socket();
    }


}
bool  SocketIO::Service::on_UdpAssoc(const socketEvent::UdpAssoc *ev)
{
    MUTEX_INSPECTOR;
    logErr2(GREEN("on_UdpAssoc %s"),ev->dump().toStyledString().c_str());
    int fd4=::socket (ev->addr4.family(), SOCK_DGRAM, 0);
    int fd6=::socket (ev->addr6.family(), SOCK_DGRAM, 0);
    logErr2("fd4=%d",fd4);
    REF_getter<epoll_socket_info> nesi4(new epoll_socket_info(SOCK_DGRAM,epoll_socket_info::STREAMTYPE_UDP_ASSOC,ev->socketId4,fd4,ev->route,
                                        maxOutBufferSize,ev->socketDescription,nullptr,m_socks->multiplexor));
    REF_getter<epoll_socket_info> nesi6(new epoll_socket_info(SOCK_DGRAM,epoll_socket_info::STREAMTYPE_UDP_ASSOC,ev->socketId6,fd6,ev->route,
                                        maxOutBufferSize,ev->socketDescription,nullptr,m_socks->multiplexor));


    if (nesi4->closed())
    {
        throw CommonError("invalid case %s %d",__FILE__,__LINE__);
    }
    if (nesi6->closed())
    {
        throw CommonError("invalid case %s %d",__FILE__,__LINE__);
    }
    {
        int i = 1;
        if(setsockopt(CONTAINER(nesi4->get_fd()),SOL_SOCKET,SO_REUSEADDR,(char *)&i,sizeof(i)))
        {
            logErr2("setsockopt: %s",strerror(errno));
            closeSocket(nesi4,"setsockopt",errno);
            return true;
        }
    }
    {
        int i = 1;
        if(setsockopt(CONTAINER(nesi6->get_fd()),SOL_SOCKET,SO_REUSEADDR,(char *)&i,sizeof(i)))
        {
            logErr2("setsockopt: %s",strerror(errno));
            closeSocket(nesi6,"setsockopt",errno);
            return true;
        }
    }

    while (bind (CONTAINER(nesi4->get_fd()),ev->addr4.addr(), ev->addr4.addrLen()) == -1)
    {
        int err=errno;
#ifdef DEBUG
        logErr2("Server: bind()  failed: errno %d %s %s (%s) %s %d", errno,strerror(errno),ev->addr4.dump().c_str(),ev->route.dump().c_str(),__FILE__,__LINE__);
#else
        logErr2("Server: bind()  failed: errno %d %s %s", errno, strerror(errno),ev->addr4.dump().c_str());
#endif
        passEvent(new socketEvent::UdpAssocRSP(
                      ev->socketId4,
                      ev->socketId6,
                      ev->addr4,
                      ev->addr6,
                      nullptr,nullptr,
                      err,poppedFrontRoute(ev->route)));
//        iUtils->setTerminate();
        return true;
    }
    while (bind (CONTAINER(nesi6->get_fd()),ev->addr6.addr(), ev->addr6.addrLen()) == -1)
    {
        int err=errno;
#ifdef DEBUG
        logErr2("Server: bind()  failed: errno %d %s %s (%s) %s %d", errno,strerror(errno),ev->addr6.dump().c_str(),ev->route.dump().c_str(),__FILE__,__LINE__);
#else
        logErr2("Server: bind()  failed: errno %d %s %s", errno, strerror(errno),ev->addr6.dump().c_str());
#endif
        passEvent(new socketEvent::UdpAssocRSP(
                      ev->socketId4,
                      ev->socketId6,
                      ev->addr4,
                      ev->addr6,
                      nullptr,nullptr,
                      err,poppedFrontRoute(ev->route)));
//        iUtils->setTerminate();
        return true;
    }

    msockaddr_in addr4;
    msockaddr_in addr6;
    socklen_t len4=addr4.maxAddrLen();
    socklen_t len6=addr6.maxAddrLen();
    if(getsockname(CONTAINER(nesi4->get_fd()),addr4.addr(),&len4))
    {
        closeSocket(nesi4,"getsockname4",errno);
        closeSocket(nesi6,"getsockname4",errno);
        logErr2("getsockname: %s",strerror(errno));
        return true;

    }
    if(getsockname(CONTAINER(nesi6->get_fd()),addr6.addr(),&len6))
    {
        closeSocket(nesi4,"getsockname6",errno);
        closeSocket(nesi6,"getsockname6",errno);
        logErr2("getsockname: %s",strerror(errno));
        return true;

    }
    logErr2(BLUE("binded4 to %s (%s)"),addr4.dump().c_str(),ev->route.dump().c_str());
    logErr2(BLUE("binded6 to %s (%s)"),addr6.dump().c_str(),ev->route.dump().c_str());

    nesi4->local_name=addr4;
    nesi6->local_name=addr6;

    passEvent(new socketEvent::UdpAssocRSP(
                  ev->socketId4,
                  ev->socketId6,
                  addr4,
                  addr6,
                  nesi4,nesi6,
                  0,poppedFrontRoute(ev->route)));


    m_socks->multiplexor->sockAddReadOnNew(nesi4.operator ->());
    m_socks->multiplexor->sockAddReadOnNew(nesi6.operator ->());
    m_socks->add(nesi4);
    m_socks->add(nesi6);
    return true;
}
bool  SocketIO::Service::on_AddToListenTCP(const socketEvent::AddToListenTCP*ev)
{
    MUTEX_INSPECTOR;

    S_LOG(__FUNCTION__);

    int fd=::socket (ev->addr.family(), SOCK_STREAM, IPPROTO_TCP);
    REF_getter<epoll_socket_info> nesi(new epoll_socket_info(SOCK_STREAM,epoll_socket_info::STREAMTYPE_LISTENING,ev->socketId,fd,ev->route,
                                       maxOutBufferSize,ev->socketDescription,ev->bufferVerify,m_socks->multiplexor));


    if (nesi->closed())
    {
        throw CommonError("invalid case %s %d",__FILE__,__LINE__);

    }
    {
        int i = 1;
        if(setsockopt(CONTAINER(nesi->get_fd()),SOL_SOCKET,SO_REUSEADDR,(char *)&i,sizeof(i)))
        {
            closeSocket(nesi,"setsockopt",errno);
            return true;
        }
    }
    while (bind (CONTAINER(nesi->get_fd()),ev->addr.addr(), ev->addr.addrLen()) == -1)
    {
#ifdef DEBUG
        logErr2("Server: bind()  failed: errno %d %s %s (%s) %s %d", errno,strerror(errno),ev->addr.dump().c_str(),ev->route.dump().c_str(),__FILE__,__LINE__);
#else
        logErr2("Server: bind()  failed: errno %d %s %s", errno, strerror(errno),ev->addr.dump().c_str());
#endif
        iUtils->setTerminate();
        return true;
    }

    msockaddr_in addr;
    socklen_t len=addr.maxAddrLen();
    if(getsockname(CONTAINER(nesi->get_fd()),addr.addr(),&len))
    {
        closeSocket(nesi,"getsockname",errno);
        return true;

    }
    logErr2("binded to %s (%s)",addr.dump().c_str(),ev->route.dump().c_str());

    if (listen(CONTAINER(nesi->get_fd()),(int)m_listen_backlog)==-1)
    {
        closeSocket(nesi,"listen",errno);
        return true;
    }
    nesi->local_name=addr;
    route_t r=nesi->m_route;
    r.pop_front();
    passEvent(new socketEvent::NotifyBindAddress(nesi,ev->socketDescription,ev->rebind,r));


    m_socks->multiplexor->sockAddReadOnNew(nesi.operator ->());
    m_socks->add(nesi);
    return true;
}

bool  SocketIO::Service::on_AddToConnectTCP(const socketEvent::AddToConnectTCP*ev)
{
    MUTEX_INSPECTOR;

    XTRY;
    S_LOG(__FUNCTION__);
    int PF=ev->addr.family()==AF_INET?PF_INET:PF_INET6;
    SOCKET_fd sock=::socket (PF, SOCK_STREAM, IPPROTO_TCP);
    REF_getter<epoll_socket_info> nesi=new epoll_socket_info(SOCK_STREAM,epoll_socket_info::STREAMTYPE_CONNECTED,ev->socketId,sock,ev->route,
//            m_socks.___ptr,
            maxOutBufferSize,ev->socketDescription,ev->bufferVerify,m_socks->multiplexor);

    nesi->inConnection=true;

    if (CONTAINER(sock)==-1)
        return true;
    {
        int i = 1;
        if(setsockopt(CONTAINER(sock),SOL_SOCKET,SO_REUSEADDR,(char *)&i,sizeof(i)))
        {
            closeSocket(nesi,"setsockopt",errno);
            return true;
        }
    }

//#ifndef __IOS__
    if (1)
    {
#ifdef _WIN32
        u_long f = 1;
        int r=ioctlsocket(CONTAINER(sock), FIONBIO, &f);
#elif __ANDROID__
        int flags = fcntl(CONTAINER(sock), F_GETFL);
        int r=fcntl(CONTAINER(sock),F_SETFL,O_NONBLOCK|flags);
#else
        int f = 1;
        int r=ioctl(CONTAINER(sock), FIONBIO, &f);
#endif
        if (r==-1)
        {
            closeSocket(nesi,"ioctl",errno);
            return true;

        }
    }

    {
        struct linger l_ {};
        l_.l_onoff=1;
        l_.l_linger=(u_short)5;
        if (::setsockopt(CONTAINER(sock),SOL_SOCKET,SO_LINGER,(char*)&l_, sizeof(l_))!=0)
        {
            closeSocket(nesi,"SP_LINGER",errno);
            return true;
        }
    }

    msockaddr_in sa=ev->addr;
#ifdef __IOS__
    if(ev->addr.family()==AF_INET)
    {
        sa.u.sa4.sin_len=16;
    }
#endif
    nesi->remote_name=ev->addr;
#ifdef HAVE_KQUEUE
    {
        struct kevent ev;
        EV_SET(&ev,CONTAINER(nesi->get_fd()),EVFILT_WRITE,EV_ADD|EV_CLEAR,0,0,(void*)(long)CONTAINER(nesi->m_id));
        m_socks->multiplexor->addEvent(ev);
    }
#endif
#ifdef HAVE_EPOLL
    {
        struct epoll_event evtz {};
        evtz.events=EPOLLIN|EPOLLOUT;
        evtz.data.u64= static_cast<uint64_t>(CONTAINER(nesi->m_id));

        if (epoll_ctl(m_socks->multiplexor->m_epoll.m_epollFd, EPOLL_CTL_ADD, CONTAINER(nesi->get_fd()), &evtz) < 0)
        {
            logErr2("epoll_ctl mod: socket '%d' - errno %d",CONTAINER(nesi->get_fd()), errno);
        }

    }

#endif


    m_socks->add(nesi);
    nesi->remote_name=sa;
    if(::connect(CONTAINER(sock),sa.addr(), sa.addrLen()))
    {
#ifndef _WIN32
        if(errno != EINPROGRESS)
        {

//            logErr2("::connect %s %s %s",strerror(errno),__func__,sa.jdump().toStyledString().c_str());
            closeSocket(nesi,"connect",errno);
            passEvent(new socketEvent::ConnectFailed(nesi,sa,errno,poppedFrontRoute(ev->route)));
            return true;
        }
#endif

    }
    else {
        passEvent(new socketEvent::Connected(nesi,poppedFrontRoute(ev->route)));
    }



    XPASS;
    return true;
}

bool SocketIO::Service::on_Write(const socketEvent::Write* e)
{
    MUTEX_INSPECTOR;

    REF_getter<epoll_socket_info> esi=m_socks->getOrNull(e->socketId);
    if(esi.valid())
        esi->write_(e->buf);
    return true;
}
bool SocketIO::Service::on_RequestIncoming(const webHandlerEvent::RequestIncoming* e)
{
    MUTEX_INSPECTOR;

    HTTP::Response cc;
    cc.content+="<h1>Socket report</h1><p>";

    Json::Value v=jdump();
    Json::StyledWriter w;
    cc.content+="<pre>\n"+w.write(v)+"\n</pre>";

    cc.makeResponse(e->esi);
    return true;
}

Json::Value SocketIO::SocketsContainerForSocketIO::jdump()
{

    Json::Value v;
    return v;
}



SocketIO::SocketsContainerForSocketIO::~SocketsContainerForSocketIO()
{
#ifdef HAVE_KQUEUE
//    multiplexor->clear();
#endif

}
