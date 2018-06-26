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
#endif
#include <fcntl.h>
#include "version_mega.h"
#include "Events/Tools/webHandler/RegisterHandler.h"

#include "Events/System/Net/socket/Accepted.h"
#include "Events/System/Net/socket/StreamRead.h"
#include "Events/System/Net/socket/Connected.h"
#include "Events/System/Net/socket/NotifyBindAddress.h"
#include "Events/System/Net/socket/NotifyOutBufferEmpty.h"
#include "Events/System/Net/socket/ConnectFailed.h"
#include "Events/Tools/webHandler/RegisterDirectory.h"
#include "events_socket.hpp"


SocketIO::Service::~Service()
{
    m_isTerminating=true;
//    pthread(
    pthread_join(m_pthread_id_worker,NULL);
#ifdef HAVE_EPOLL
    logErr2("close m_socks->m_epoll.m_epollFd %d",m_socks->m_epoll.m_epollFd);
    if(::close(m_socks->m_epoll.m_epollFd))
        logErr2("close: %d", errno);
#endif
#ifdef HAVE_KQUEUE
    logErr2("close m_socks->m_epoll.m_epollFd %d",m_socks->m_kqueue);
    if(::close(m_socks->m_kqueue))
        logErr2("close: %d", errno);
#endif

    m_socks->clear();
}

SocketIO::Service::Service(const SERVICE_id& id, const std::string& nm, IInstance* ifa):
    UnknownBase(nm),Broadcaster(ifa),
    ListenerBuffered(this,nm,ifa->getConfig(),id,ifa),
#ifdef DEBUG
    Logging(this,TRACE_log,ifa),
#else
    Logging(this,INFO_log,ifa),
#endif
    m_total_recv(0L),
    m_total_send(0L),
    m_total_accepted(0L),

    m_socks(new SocketsContainerForSocketIO),m_listen_backlog(128),
    maxOutBufferSize(ifa->getConfig()->get_int64_t("maxOutBufferSize",8*1024*1024,"")),
    iInstance(ifa),m_isTerminating(false)
{
    try {
#ifdef HAVE_EPOLL
        epoll_timeout_millisec=ifa->getConfig()->get_int64_t("timeout_millisec",10,"");
        epoll_size=ifa->getConfig()->get_int64_t("size",1024,"");

        m_socks->m_epoll.m_epollFd=epoll_create(epoll_size);
        if (m_socks->m_epoll.m_epollFd<0) throw CommonError("epoll_create: errno %d",errno);
#endif
#ifdef HAVE_KQUEUE
        kqueue_timeout_millisec=ifa->getConfig()->get_int64_t("timeout_millisec",10,"");
        kqueue_size=ifa->getConfig()->get_int64_t("size",1024,"");

#endif
        m_listen_backlog=ifa->getConfig()->get_int64_t("listen_backlog",128,"");

        if(pthread_create(&m_pthread_id_worker,NULL,worker,this))
        {
            throw CommonError("pthread_create: errno %d",errno);
            //log(ERROR_log,"pthread_create: errno %d",errno);
        }
    }
    catch(...)
    {
        log(ERROR_log,"failed init service socket");
    }

}

void SocketIO::Service::onEPOLLIN(const REF_getter<epoll_socket_info>&esi)
{
    
    XTRY;

    if (esi->m_streamType==epoll_socket_info::_LISTENING)
    {
        XTRY;
        msockaddr_in neu_sa;
        socklen_t len=neu_sa.maxAddrLen();
        SOCKET_fd neu_fd = ::accept(CONTAINER(esi->get_fd()), neu_sa.addr(),&len);

        DBG(logErr2("accept: fd=%d",neu_fd));
        if (CONTAINER(neu_fd) < 0)
        {
            log(ERROR_log,"accept: %d errno %d %s", CONTAINER(neu_fd),errno,strerror(errno));

            return;
        }
        //else
        //{
            msockaddr_in local_name;
            msockaddr_in remote_name;
            socklen_t sl=local_name.maxAddrLen();
            socklen_t sr=remote_name.maxAddrLen();
            if(getsockname(CONTAINER(neu_fd), local_name.addr(), &sl))
            {
                throw CommonError("getsockname: errno %d",errno);
            }
            if(getpeername(CONTAINER(neu_fd), remote_name.addr(), &sr))
            {
                throw CommonError("getsockname: errno %d",errno);
            }


            XTRY;
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
                    log(ERROR_log,"ioctl: errno %d",errno);
                }
                XPASS;

#ifdef USE_SO_LINGER
                struct linger l_;
                l_.l_onoff=1;
                l_.l_linger=(u_short)5;
                if (::setsockopt(CONTAINER(neu_fd),SOL_SOCKET,SO_LINGER,(char*)&l_, sizeof(l_))!=0)
                {
                    log(ERROR_log,"setsockopt: linger: errno %d",errno);
                }
#endif


            }
            SOCKET_id _sid=iUtils->getSocketId();
            REF_getter<epoll_socket_info> nesi=new epoll_socket_info(epoll_socket_info::_ACCEPTED,_sid,neu_fd,esi->m_route,
                    m_socks.___ptr,
                    maxOutBufferSize, esi->socketDescription,esi->bufferVerify);


            nesi->local_name=local_name;
            nesi->remote_name=remote_name;
            //nesi->local_name.setSocketId(_sid);
            //nesi->remote_name.setSocketId(_sid);
            m_socks->addRead(nesi);

            route_t r=nesi->m_route;
            r.pop_front();
            passEvent(new socketEvent::Accepted(nesi,r));
            XPASS;
        //}
        XPASS;
    }
    else if (esi->m_streamType==epoll_socket_info::_CONNECTED ||esi->m_streamType==epoll_socket_info::_ACCEPTED)
    {
        XTRY;
#define READBUFSIZE 1024*1024*4
        st_malloc buf(READBUFSIZE);
        int r=0;

        int fd=CONTAINER(esi->get_fd());
        if(fd==-1)
            return;
        r=::recv(fd,(char*)buf.buf,READBUFSIZE,0);
        if (r==0 )
        {
            XTRY;
            esi->close("SocketService: recv 0, resetted by peer");
            return;
            XPASS;

        }
        if (r==-1)
        {
            if(errno)
            {
                DBG(log(ERROR_log,"recv: errno %d %s",errno,strerror(errno)));
            }
#ifdef _WIN32
            if(!esi->closed())
            {
                esi->close("SocketService: close due recv return 0");
            }
#endif
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
                XTRY;
                if(!esi->closed())
                {
                    esi->close("SocketService: case EBADF:ECONNREFUSED:ENOTCONN:ENOTSOCK:ECONNRESET:");
                }
                return;
                XPASS;
            }
            default:
                if(!esi->closed())
                {
                    esi->close("SocketService: case default");
                }
                return;
            }
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


#ifdef HAVE_EPOLL
                struct epoll_event evtz;
                evtz.events=EPOLLIN/*|EPOLLET*/;
                if(esi->m_outBuffer.size())evtz.events|=EPOLLOUT;
                evtz.data.u64=CONTAINER(esi->m_id);
                int fd=CONTAINER(esi->get_fd());
                if(fd!=-1)
                {
                    if (epoll_ctl(m_socks->m_epoll.m_epollFd, EPOLL_CTL_MOD, fd, &evtz) < 0)
                    {
                        logErr2("epoll_ctl mod: socket '%d' - errno %d",CONTAINER(esi->get_fd()), errno);
                        if(!esi->closed())
                        {
                            esi->close("epoll_mod error");
                        }
                    }
                }
#endif
#ifdef HAVE_KQUEUE
                /// do nothing
                struct kevent e1;
                EV_SET(&e1,fd,EVFILT_READ,EV_ADD,0,0,(void*)(long)CONTAINER(esi->m_id));
                m_socks->evSet.push_back(e1);

#endif

                XPASS;
            }
            m_total_recv+=r;
            //logErr2("sockeService %s",jdump().toStyledString().c_str());

            {
                route_t rt=esi->m_route;
                rt.pop_front();
                bool verified=true;
                if(esi->bufferVerify)
                {

                    M_LOCK(esi->m_inBuffer);
                    verified=esi->bufferVerify(esi->m_inBuffer._mx_data);
                    //logErr2("verified buffer %d data size %d",verified,esi->m_inBuffer._mx_data.size());
                }
                else
                {
                    DBG(logErr2("!if(esi->bufferVerify)"));
                    verified=true;
                }
                if(verified)
                {
                    passEvent(new socketEvent::StreamRead(esi,rt));
                    //logErr2("pass event socketEvent::StreamRead esi->inBuffer._mx_data.size()=%d",esi->inBuffer._mx_data.size());
                }
            }
        }
        XPASS;
    }

    XPASS;

}
void SocketIO::Service::onEPOLLOUT(const REF_getter<epoll_socket_info>&__EV_)
{

    XTRY;
    REF_getter<epoll_socket_info> esi=__EV_;
    if(esi->inConnection)
    {
        XTRY;
        if(!m_socks->count(esi->m_id))
        {
            //log(ERROR_log,"socket not found %d (%s %d)",CONTAINER(esi->m_id),__FILE__,__LINE__);
            return;
        }

        msockaddr_in local_name;
        msockaddr_in remote_name;
        socklen_t sl=local_name.maxAddrLen();
        socklen_t sr=remote_name.maxAddrLen();
        if(getsockname(CONTAINER(esi->get_fd()), local_name.addr(), &sl))
        {
            throw CommonError("getsockname: errno %d",errno);
        }
        if(getpeername(CONTAINER(esi->get_fd()),  remote_name.addr(), &sr))
        {
            throw CommonError("getsockname: errno %d",errno);
        }
        esi->local_name=local_name;
        esi->remote_name=remote_name;
        route_t r=esi->m_route;
        r.pop_front();
        passEvent(new socketEvent::Connected(esi, r));
        esi->inConnection=false;
        return;
        XPASS;
    }


    bool need_to_close=false;
    {
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
                            DBG(log(TRACE_log,"set need_to_close=true"));

                        }
                        XPASS;
                    }
#ifdef HAVE_EPOLL
                    struct epoll_event evtz;
                    evtz.events=EPOLLIN/*|EPOLLET*/;
                    if(esi->m_outBuffer.size())evtz.events|=EPOLLOUT;
                    evtz.data.u64=CONTAINER(esi->m_id);

                    int fd=CONTAINER(esi->get_fd());
                    if(fd!=-1)
                    {
                        if (epoll_ctl(m_socks->m_epoll.m_epollFd, EPOLL_CTL_MOD, fd, &evtz) < 0)
                        {
                            logErr2("epoll_ctl mod: socket '%d' - errno %d",CONTAINER(esi->get_fd()), errno);
                            if(!esi->closed())
                            {
                                esi->close("epoll mod error 2");
                            }
                        }
                    }

#endif
#ifdef HAVE_KQUEUE
                    int fd=CONTAINER(esi->get_fd());
                    if(fd!=-1)
                    {
                        if(esi->m_outBuffer.size()==0)
                        {
                            /*
                            struct kevent ev;
                            EV_SET(&ev,fd,EVFILT_WRITE,EV_DELETE,0,0,(void*)(long)CONTAINER(esi->m_id));
                            m_socks->evSet.push_back(ev);
                            */
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
            const char *reason="marked to destroy on send";
            esi->close(reason);
            return;
            XPASS;
        }
        if (res==-1)
        {
            XTRY;
            DBG(log(TRACE_log,"send: errno %d",errno));
            switch(errno)
            {
#ifndef _WIN32
            case ENOBUFS:
#endif
            case EAGAIN:
                return;
            default:
            {
                esi->close("send result (-1)");

            }
            }

            XPASS;
            return;
        }
        XPASS;
    }
    XPASS;
}
void SocketIO::Service::onEPOLLERR(const REF_getter<epoll_socket_info>& esi)
{

    
    //DBG(log(TRACE_log,"SocketIO::Service::onEPOLLERR"));
    XTRY;

    closeSocket(esi,"EPOLERR");
    if(esi->m_streamType==epoll_socket_info::_CONNECTED && esi->inConnection)
    {
        route_t r=esi->m_route;
        r.pop_front();
        passEvent(new socketEvent::ConnectFailed(esi->remote_name,r));
    }


    XPASS;
}

void* SocketIO::Service::worker(void*p)
{
#ifdef __MACH__
    pthread_setname_np("SocketIO");
#else
#ifndef _WIN32
    pthread_setname_np(pthread_self(),"SocketIO");
#endif
#endif

    //
    SocketIO::Service* obj=(SocketIO::Service*)p;
#ifdef HAVE_EPOLL
    struct epoll_event events[obj->m_socks->m_epoll.size+10];
#endif
#ifdef HAVE_KQUEUE
    obj->m_socks->m_kqueue=kqueue();
    if (obj->m_socks->m_kqueue==-1) throw CommonError("epoll_create: errno %d",errno);
#endif
    while (1)
    {
        XTRY;
        try
        {
            // MUTEX_INSPECTOR;

#ifdef HAVE_SELECT

            if(obj->m_isTerminating) return NULL;
//            if(iUtils->isTerminating())
//                return NULL;
            std::map<SOCKET_id,REF_getter<epoll_socket_info> > cc=obj->m_socks->getContainer();
            fd_set readfds;
            fd_set writefds;
            fd_set exceptfds;
            FD_ZERO(&readfds);
            FD_ZERO(&writefds);
            FD_ZERO(&exceptfds);
            std::set<int> socklist;
            for(std::map<SOCKET_id,REF_getter<epoll_socket_info> >::iterator i=cc.begin(); i!=cc.end(); i++)
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
                    for(std::map<SOCKET_id,REF_getter<epoll_socket_info> >::iterator i=cc.begin(); i!=cc.end(); i++)
                    {
                        REF_getter<epoll_socket_info> esi=i->second;
                        if(FD_ISSET(CONTAINER(esi->get_fd()),&readfds))
                            obj->onEPOLLIN(esi);
                        if(FD_ISSET(CONTAINER(esi->get_fd()),&writefds))
                            obj->onEPOLLOUT(esi);
                        if(FD_ISSET(CONTAINER(esi->get_fd()),&exceptfds))
                        {
                            obj->onEPOLLERR(esi);
                            obj->m_socks->remove(esi->m_id,"exceptfds on");
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

            if(obj->m_isTerminating) return NULL;
//            if(iUtils->isTerminating()) return NULL;
            int nfds;
            {
                nfds=epoll_wait(obj->m_socks->m_epoll.m_epollFd,events,obj->m_socks->m_epoll.size,obj->m_socks->m_epoll.timeout_millisec);
            }

            int __ERRNO=errno;
            if(iUtils->isTerminating())
                return NULL;
            if (nfds==-1)
            {
                if(__ERRNO==EINTR)
                {
                    continue;
                }
                obj->log(ERROR_log,"epoll_wait: __ERRNO %d",__ERRNO);


                std::map<SOCKET_id,REF_getter<epoll_socket_info> >cc=obj->m_socks->getContainer();
                obj->m_socks->clear();
                for(auto i=cc.begin(); i!=cc.end(); i++)
                {

                    REF_getter<epoll_socket_info> &ss=i->second;
                    ss->close("epoll_wait failed");
                    if(ss->m_streamType==epoll_socket_info::_LISTENING)
                    {
                        const REF_getter<socketEvent::AddToListenTCP> e=new socketEvent::AddToListenTCP(ss->m_id,ss->local_name,ss->socketDescription,true,ss->bufferVerify,ss->m_route);
                        obj->on_AddToListenTCP(e.operator ->());
                    }
                }
                continue;
            }
            for (int i=0; i<nfds; i++)
            {
                XTRY;
                SOCKET_id id;
                CONTAINER(id)=events[i].data.u64;
                REF_getter<epoll_socket_info> __EV=obj->m_socks->getOrNull(id);
                if (!__EV.valid())
                {
                    continue;
                }
                if (events[i].events&EPOLLERR && !__EV->closed())
                {
                    obj->onEPOLLERR(__EV);
                }
                if (events[i].events&EPOLLIN && !__EV->closed())
                {
                    obj->onEPOLLIN(__EV);
                }
                if (events[i].events&EPOLLOUT && !__EV->closed())
                {
                    obj->onEPOLLOUT(__EV);
                    //__EV->lastIO=time(NULL);
                }
                XPASS;
            }
#endif
#ifdef HAVE_KQUEUE
            if(obj->m_isTerminating) return NULL;
            //if(obj->iInstance->isTerminating()) return NULL;

            timespec ts;
            ts.tv_sec=0;
            ts.tv_nsec=obj->kqueue_timeout_millisec*1000;
            int evsSz=obj->m_socks->evSet.size();
            struct kevent evs[evsSz];
            for(int i=0; i<evsSz; i++)
            {
                evs[i]=obj->m_socks->evSet[i];
            }

            obj->m_socks->evSet.clear();
            obj->m_socks->evSet.reserve(1024);

            struct kevent evList[1024];

            int nev = kevent(obj->m_socks->m_kqueue, evs, evsSz, evList, 1024, &ts);
            if (nev < 0)
            {
                logErr2("kevent error: %d %s", errno, strerror(errno));
                continue;

            }
//                        err(1, "kevent");
            for (int i=0; i<nev; i++) {
                //logErr2("nev %d",nev);
                SOCKET_id sid;
                CONTAINER(sid)=(long)evList[i].udata;
                REF_getter<epoll_socket_info> esi=obj->m_socks->getOrNull(sid);
                if(!esi.valid())
                {
                    //logErr2("kevent: esi == NULL");
                    continue;
                }
                //logErr2("evList[%d].flags %d %04x",i,evList[i].flags,evList[i].flags);
                if (evList[i].flags & EV_EOF || evList[i].flags & EV_ERROR) {
                    struct kevent ev1,ev2;
                    EV_SET(&ev1, CONTAINER(esi->get_fd()), EVFILT_READ, EV_DELETE, 0, 0, (void*)CONTAINER(sid));
                    EV_SET(&ev2, CONTAINER(esi->get_fd()), EVFILT_WRITE, EV_DELETE, 0, 0, (void*)CONTAINER(sid));
                    obj->m_socks->evSet.push_back(ev1);
                    obj->m_socks->evSet.push_back(ev2);
                    obj->onEPOLLERR(esi);
                }
                //logErr2("evList[i].filter %d",evList[i].filter);
                if(evList[i].filter==EVFILT_READ)
                {
                    //logErr2("if(evList[i].flags==(uint16_t)EVFILT_READ)");
                    obj->onEPOLLIN(esi);
                }
                if(evList[i].filter==EVFILT_WRITE)
                {
                    //logErr2("if(evList[i].flags==(uint16_t)EVFILT_WRITE)");
                    obj->onEPOLLOUT(esi);
                }
            }
#endif
        }
        catch(std::exception &e)
        {
            obj->log(ERROR_log,"exception: %s %s %d",e.what(),__FILE__,__LINE__);
        }
        XPASS;
    }
    return NULL;
}
void SocketIO::Service::closeSocket(const REF_getter<epoll_socket_info>&esi,const std::string &reason)
{

    XTRY;
    //DBG(log(TRACE_log,"close socket %s",reason.c_str()));
    if(!esi->closed())
    {
        DBG(log(TRACE_log,"socks remove %s",reason.c_str()));
        esi->close(reason);
    }
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
    return new Service(id,nm,ifa);
}





bool  SocketIO::Service::handleEvent(const REF_getter<Event::Base>&e)
{
    XTRY;
    auto &ID=e->id;
    if(timerEventEnum::TickTimer==ID)
        return on_TickTimer((const timerEvent::TickTimer*)e.operator->());
    if(socketEventEnum::AddToListenTCP==ID)
        return on_AddToListenTCP((const socketEvent::AddToListenTCP*)e.operator->());
    if(socketEventEnum::AddToConnectTCP==ID)
        return on_AddToConnectTCP((const socketEvent::AddToConnectTCP*)e.operator->());
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
        iUtils->registerPlugingInfo(COREVERSION,pn,IUtils::PLUGIN_TYPE_SERVICE,ServiceEnum::Socket,"SocketIO");
    }
    else
    {
        iUtils->registerService(COREVERSION,ServiceEnum::Socket,SocketIO::Service::construct,"SocketIO");
        regEvents_socket();
    }


}
bool  SocketIO::Service::on_AddToListenTCP(const socketEvent::AddToListenTCP*ev)
{
    
    //if(iInstance->no_bind())
    //  throw CommonError("SocketIO::Service::on_AddToListenTCP %s",_DMI().c_str());

    int fd=::socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
    REF_getter<epoll_socket_info> nesi(new epoll_socket_info(epoll_socket_info::_LISTENING,ev->socketId,fd,ev->route,
                                       m_socks.___ptr,
                                       maxOutBufferSize,ev->socketDescription,ev->bufferVerify));


    if (nesi->closed())
    {
        return true;
    }
    {
        int i = 1;
        if(setsockopt(CONTAINER(nesi->get_fd()),SOL_SOCKET,SO_REUSEADDR,(char *)&i,sizeof(i)))
        {
            nesi->close((std::string)"setsockopt");
            return true;
        }
    }
    //logErr2("")
    while (bind (CONTAINER(nesi->get_fd()),ev->addr.addr(), ev->addr.addrLen()) == -1)
    {
#ifdef DEBUG
        log(ERROR_log,"Server: bind()  failed: errno %d %s %s (%s) %s %d", errno,strerror(errno),ev->addr.dump().c_str(),ev->route.dump().c_str(),__FILE__,__LINE__);
#else
        log(ERROR_log,"Server: bind()  failed: errno %d %s %s", errno, strerror(errno),ev->addr.dump().c_str());
#endif
        iUtils->setTerminate();
        return true;
    }

    msockaddr_in addr;
    socklen_t len=addr.maxAddrLen();
    if(getsockname(CONTAINER(nesi->get_fd()),addr.addr(),&len))
    {
        nesi->close((std::string)"getsockname");
        return true;

    }
    log(INFO_log,"binded to %s (%s)",addr.dump().c_str(),ev->route.dump().c_str());

    if (listen(CONTAINER(nesi->get_fd()),(int)m_listen_backlog)==-1)
    {
        nesi->close((std::string)"listen");
        return true;
    }
    nesi->local_name=addr;
    route_t r=nesi->m_route;
    r.pop_front();
    passEvent(new socketEvent::NotifyBindAddress(nesi,ev->socketDescription,ev->rebind,r));


    m_socks->addRead(nesi);
    return true;
}
bool  SocketIO::Service::on_AddToConnectTCP(const socketEvent::AddToConnectTCP*ev)
{

    XTRY;
    SOCKET_fd sock=::socket (PF_INET, SOCK_STREAM, IPPROTO_TCP);
    REF_getter<epoll_socket_info> nesi=new epoll_socket_info(epoll_socket_info::_CONNECTED,ev->socketId,sock,ev->route,
            m_socks.___ptr,
            maxOutBufferSize,ev->socketDescription,ev->bufferVerify);

    nesi->inConnection=true;

    if (CONTAINER(sock)==-1)
        return true;
    {
        int i = 1;
        if(setsockopt(CONTAINER(sock),SOL_SOCKET,SO_REUSEADDR,(char *)&i,sizeof(i)))
        {
            nesi->close((std::string)"setsockopt");
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
            nesi->close((std::string)"ioctl");
            return true;

        }
    }
//#endif

    {
        struct linger l_;
        l_.l_onoff=1;
        l_.l_linger=(u_short)5;
        if (::setsockopt(CONTAINER(sock),SOL_SOCKET,SO_LINGER,(char*)&l_, sizeof(l_))!=0)
        {
            nesi->close((std::string)"SO_LINGER");
            return true;
        }
    }

    msockaddr_in sa=ev->addr;
#ifdef __IOS__
    sa.u.sa4.sin_len=16;
#endif
//    sa.sa4.sin_family=AF_INET;
    if(::connect(CONTAINER(sock),sa.addr(), sa.addrLen()))
    {

#ifndef _WIN32
        if(errno != EINPROGRESS)
        {

            logErr2("::connect %s %s",strerror(errno),__func__);
            nesi->close((std::string)"connect err");

            return true;
        }
#endif

    }
    else {
        logErr2("socket connect OK");

    }


    nesi->remote_name=ev->addr;
    m_socks->addRW(nesi);

    XPASS;
    return true;
}

bool SocketIO::Service::on_Write(const socketEvent::Write* e)
{
    
    REF_getter<epoll_socket_info> esi=m_socks->getOrNull(e->socketId);
    if(esi.valid())
        esi->write_(e->buf);
    return true;
}
bool SocketIO::Service::on_RequestIncoming(const webHandlerEvent::RequestIncoming* e)
{
    
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
    return SocketsContainerBase::jdump();
}

void SocketIO::SocketsContainerForSocketIO::on_delete(const REF_getter<epoll_socket_info>&esi, const std::string& )
{
    
#ifdef HAVE_EPOLL

    int fd=CONTAINER(esi->get_fd());
    if(fd!=-1)
    {
        if (epoll_ctl(m_epoll.m_epollFd,EPOLL_CTL_DEL,fd,NULL) < 0)
        {
            logErr2("epoll_ctl remove: socket '%d' - errno %d",CONTAINER(esi->get_fd()), errno);
        }
    }

#endif

#ifdef HAVE_KQUEUE
    int fd=CONTAINER(esi->get_fd());
    if(fd!=-1)
    {
        struct kevent ev1,ev2;
        EV_SET(&ev1,fd,EVFILT_READ,EV_DELETE,0,0,(void*)(long)CONTAINER(esi->m_id));
        EV_SET(&ev2,fd,EVFILT_WRITE,EV_DELETE,0,0,(void*)(long)CONTAINER(esi->m_id));
        evSet.push_back(ev1);
        evSet.push_back(ev2);
    }
#endif


}

void SocketIO::SocketsContainerForSocketIO::on_mod_write(const REF_getter<epoll_socket_info>&esi)
{
#ifdef HAVE_EPOLL
    struct epoll_event evtz;
    evtz.events=EPOLLIN|/*EPOLLET|*/EPOLLOUT;
    evtz.data.u64=CONTAINER(esi->m_id);

    int fd=CONTAINER(esi->get_fd());
    if(fd!=-1)
    {
        if (epoll_ctl(m_epoll.m_epollFd, EPOLL_CTL_MOD, fd, &evtz) < 0)
        {
            logErr2("epoll_ctl mod: socket '%d' - errno %d",CONTAINER(esi->get_fd()), errno);
            if(!esi->closed())
            {
                esi->close("EPOLL_CTL_MOD error");
            }
        }
    }
#endif
#ifdef HAVE_KQUEUE


    int fd=CONTAINER(esi->get_fd());
    if(fd!=-1)
    {
        struct kevent ev2;
        EV_SET(&ev2,fd,EVFILT_WRITE,EV_ADD,0,0,(void*)(long)CONTAINER(esi->m_id));
        evSet.push_back(ev2);
    }

#endif
}

void SocketIO::SocketsContainerForSocketIO::addRead(const REF_getter<epoll_socket_info>&esi)
{
    
    XTRY;
    add(ServiceEnum::Socket,esi);
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
        evSet.push_back(ev);
    }

#endif
    XPASS;
}
void SocketIO::SocketsContainerForSocketIO::addRW(const REF_getter<epoll_socket_info>&esi)
{
    
    XTRY;
    add(ServiceEnum::Socket,esi);
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
            esi->close("EPOLL_CTL_ADD 2");
        }
    }
#endif
#ifdef HAVE_KQUEUE

    int fd=CONTAINER(esi->get_fd());
    if(fd!=-1)
    {
        struct kevent e1,e2;
        EV_SET(&e1,fd,EVFILT_READ,EV_ADD,0,0,(void*)(long)CONTAINER(esi->m_id));
        EV_SET(&e2,fd,EVFILT_WRITE,EV_ADD,0,0,(void*)(long)CONTAINER(esi->m_id));
        evSet.push_back(e1);
        evSet.push_back(e2);
    }

#endif

    XPASS;
}

SocketIO::SocketsContainerForSocketIO::~SocketsContainerForSocketIO()
{
#ifdef HAVE_KQUEUE
    evSet.clear();
#endif

}
