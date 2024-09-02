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
#include <colorOutput.h>
#include <logging.h>
#include <tools_mt.h>
#include <Events/System/Net/socketEvent.h>

#include "version_mega.h"
#include "Events/Tools/webHandlerEvent.h"
#include "events_socket.hpp"
#include "colorOutput.h"
#include <thread>


SocketIO::Service::~Service()
{
    isTerminating_=true;
    for(int i=0; i<pthread_id_worker_.size(); i++)
    {
        int err=pthread_join(pthread_id_worker_[i],NULL);
        if(err)
        {
            printf(RED("%s pthread_join: %s"),__PRETTY_FUNCTION__,strerror(errno));
        }
    }


    {
        W_LOCK(m_io_socks_pollers_.lock);
        m_io_socks_pollers_.socks_pollers_.clear();
    }
}

SocketIO::Service::Service(const SERVICE_id& id, const std::string& nm, IInstance* ifa):
    UnknownBase(nm),Broadcaster(ifa),
    ListenerBuffered1Thread(nm,id),
    n_workers_(2),
    listen_backlog_(128),
    iInstance(ifa),isTerminating_(false)
{
    try {
        epoll_timeout_millisec_=ifa->getConfig()->get_int64_t("epoll_timeout_millisec",1000,"");

        listen_backlog_=ifa->getConfig()->get_int64_t("listen_backlog",128,"");
        n_workers_=ifa->getConfig()->get_int64_t("n_workers",2,"socket poll thread count");
        logErr2("socket nworkers %d",n_workers_);
        for(int i=0; i<n_workers_; i++)
        {
            REF_getter<SocketsContainerForSocketIO> MS=new SocketsContainerForSocketIO;

            {
                W_LOCK(m_io_socks_pollers_.lock);
                m_io_socks_pollers_.socks_pollers_.push_back(MS);
                m_io_socks_pollers_.for_threads_.push_back(MS);
            }

        }
        pthread_id_worker_.resize(n_workers_);
        for(int i=0; i<n_workers_; i++)
        {
            if(pthread_create(&pthread_id_worker_[i],NULL,worker__,this))
            {
                throw CommonError("pthread_create: errno %d",errno);
            }
        }
    }
    catch(...)
    {
        logErr2("failed init service socket");
    }

}
void SocketIO::Service::handle_accepted1(const SOCKET_fd &neu_fd,const REF_getter<epoll_socket_info> esi,
        const REF_getter<SocketsContainerForSocketIO>& MS, const msockaddr_in& remote_sa)
{
    if(CONTAINER(neu_fd)!=-1)
    {
        if (CONTAINER(neu_fd) < 0)
        {
            return;
        }

//        msockaddr_in local_name;
//        msockaddr_in remote_name;
//        socklen_t sl=local_name.maxAddrLen();
//        socklen_t sr=remote_name.maxAddrLen();
//        if(getsockname(CONTAINER(neu_fd), local_name.addr(), &sl))
//        {
//            logErr2("getsockname: errno %d %s (%s %d)",errno,strerror(errno),__FILE__,__LINE__);
//        }
//        if(getpeername(CONTAINER(neu_fd), remote_name.addr(), &sr))
//        {
//            logErr2("getsockname: errno %d %s (%s %d)",errno,strerror(errno),__FILE__,__LINE__);
//        }

        if(1) {
            MUTEX_INSPECTOR;
            int i = 1;
            if(setsockopt(CONTAINER(neu_fd),SOL_SOCKET,SO_REUSEADDR,(char *)&i,sizeof(i)))
            {
                throw CommonError("if(setsockopt(CONTAINER(neu_fd),SOL_SOCKET,SO_REUSEADDR,(char *)&i,sizeof(i)))");

            }
        }

        if(1) {
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

        REF_getter<SocketsContainerForSocketIO> MS2=m_io_socks_pollers_.getPoller(this);


        SOCKET_id _sid=iUtils->getNewSocketId();
        REF_getter<epoll_socket_info> nesi=new epoll_socket_info(SOCK_STREAM,epoll_socket_info::STREAMTYPE_ACCEPTED,_sid,neu_fd,esi->m_route,
            esi->socketDescription_,MS2->multiplexor_);

        esi->remote_name_.emplace(remote_sa);


        MS2->multiplexor_->sockAddReadOnNew(nesi.get());
        MS2->add(nesi);

        passEvent(new socketEvent::Accepted(nesi,nesi->m_route));


    }

}


void SocketIO::Service::onEPOLLIN_STREAMTYPE_LISTENING(const REF_getter<epoll_socket_info>&esi,const REF_getter<SocketsContainerForSocketIO>& MS)
{
    MUTEX_INSPECTOR;
    S_LOG(__FUNCTION__);
    msockaddr_in neu_sa;
    socklen_t len=neu_sa.maxAddrLen();
    SOCKET_fd neu_fd = ::accept(CONTAINER(esi->fd_), neu_sa.addr(),&len);
//    printf("accepted %d\n",neu_fd);
    handle_accepted1(neu_fd,esi,MS,neu_sa);


}
void SocketIO::Service::onEPOLLIN_STREAMTYPE_CONNECTED_or_STREAMTYPE_ACCEPTED(
    const REF_getter<epoll_socket_info>&esi, const  REF_getter<SocketsContainerForSocketIO>& MS)
{
    MUTEX_INSPECTOR;
    S_LOG(__FUNCTION__);
    XTRY;
#define READBUFSIZE 1024*1024*4
    st_malloc buf(READBUFSIZE);
    int r=0;

    int fd=CONTAINER(esi->fd_);
    if(fd==-1)
    {
        logErr2("invalid case %s %d %s",__FILE__,__LINE__,_DMI().c_str());
        return;
    }
    /// bug fix - cannot receive data while connected not thrown
//    if(esi->inConnection_)
//        return;
    r=::recv(fd,(char*)buf.buf,READBUFSIZE,0);
    if (r==0 )
    {
        XTRY;
        int err=errno;
        closeSocket(esi,"recv 0, resetted by peer",err,MS);
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
            XTRY;
            needClose=true;
            errText=(std::string)"SocketService: case EBADF:ECONNREFUSED:ENOTCONN:ENOTSOCK:ECONNRESET: "+strerror(errno);
            return;
            XPASS;
        }
        default:
            needClose=true;
            errText=(std::string)"SocketService: case default "+strerror(errno);
            return;
        }
        if(needClose)
            closeSocket(esi,errText.c_str(),errno,MS);
        XPASS;
    }
    if(r>=0)
    {
        {
            XTRY;
            esi->inBuffer_.append((char*)buf.buf,r);
            XPASS;
        }
        {
            XTRY;
            passEvent(new socketEvent::StreamRead(esi,esi->m_route));
            XPASS;

        }

    }
    XPASS;

}

void SocketIO::Service::onEPOLLIN(const REF_getter<epoll_socket_info>&esi, const REF_getter<SocketsContainerForSocketIO>& MS)
{
    MUTEX_INSPECTOR;
    S_LOG(__FUNCTION__);
    XTRY;


    if (esi->streamType_==epoll_socket_info::STREAMTYPE_LISTENING)
    {
        XTRY;
        onEPOLLIN_STREAMTYPE_LISTENING(esi,MS);
        XPASS;
    }
    else if (esi->streamType_==epoll_socket_info::STREAMTYPE_CONNECTED)
    {
        onEPOLLIN_STREAMTYPE_CONNECTED_or_STREAMTYPE_ACCEPTED(esi,MS);

    }
    else if(esi->streamType_==epoll_socket_info::STREAMTYPE_ACCEPTED)
    {
        onEPOLLIN_STREAMTYPE_CONNECTED_or_STREAMTYPE_ACCEPTED(esi,MS);
    }

    XPASS

}
void SocketIO::Service::onEPOLLOUT(const REF_getter<epoll_socket_info>&__EV_,const  REF_getter<SocketsContainerForSocketIO>& MS)
{

    MUTEX_INSPECTOR;


    S_LOG(__FUNCTION__);

    XTRY;
    const REF_getter<epoll_socket_info>& esi=__EV_;
    if(esi->socketType_!=SOCK_STREAM)
    {
        throw CommonError("if(esi->m_socketType!=SOCK_STREAM) %s",esi->socketDescription_);
    }
    if(esi->inConnection_)
    {
        S_LOG("esi->inConnection");

        XTRY;
        if(!MS->count(esi->id_))
        {
            logErr2("socket not found %d (%s %d)",CONTAINER(esi->id_),__FILE__,__LINE__);
            return;
        }
        esi->inConnection_=false;
        if(esi->outBuffer_.size()==0)
            MS->multiplexor_->sockStopWrite(esi.get());

        passEvent(new socketEvent::Connected(esi, esi->m_route));

//        if(esi->inBuffer_.size())
//            passEvent(new socketEvent::StreamRead(esi, esi->m_route));
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
            {
                res=esi->outBuffer_.send(esi->fd_,esi.get());
                if(res>0)
                {

                    if(esi->outBuffer_.size()==0 && esi->markedToDestroyOnSend_)
                    {
                        need_to_close=true;
                    }


                }
                if(esi->outBuffer_.size()==0)
                {
                    passEvent(new socketEvent::NotifyOutBufferEmpty(esi, esi->m_route));
                }
            }
        }
        if(need_to_close)
        {
            XTRY;
            closeSocket(esi,"marked to destroy on send",0,MS);
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
                closeSocket(esi,"send result (-1)",0,MS);

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
void SocketIO::Service::onEPOLLERR(const REF_getter<epoll_socket_info>& esi, const  REF_getter<SocketsContainerForSocketIO>& MS)
{
    MUTEX_INSPECTOR;

    S_LOG(__FUNCTION__);

    if(esi->socketType_!=SOCK_STREAM)
    {
        throw CommonError("if(esi->m_socketType!=SOCK_STREAM) %s %s",esi->socketDescription_,__PRETTY_FUNCTION__);
    }

    XTRY;

    int       error = 0;
    socklen_t errlen = sizeof(error);
    if (getsockopt(CONTAINER(esi->fd_), SOL_SOCKET, SO_ERROR,
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
#endif
        }
    }

    closeSocket(esi,"EPOLERR",error,MS);


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
    MUTEX_INSPECTOR;
#if defined __MACH__
    pthread_setname_np("SocketIO");
#else
#if !defined _WIN32 && !defined __FreeBSD__
    pthread_setname_np(pthread_self(),"SocketIO");
#endif
#endif
    REF_getter<SocketsContainerForSocketIO> MS(NULL);
    {
        W_LOCK(m_io_socks_pollers_.lock);
        if(m_io_socks_pollers_.for_threads_.empty())
            throw CommonError("if(m_io_socks_pollers.for_threads.empty())");
        MS=*m_io_socks_pollers_.for_threads_.begin();
        m_io_socks_pollers_.for_threads_.pop_front();
    }
#ifdef HAVE_EPOLL
#define eventsSIZE 1024
    struct epoll_event events[eventsSIZE];
#endif
    while (1)
    {
        MUTEX_INSPECTOR;
        XTRY;
//        handle_accepted(MS);
        try
        {
#ifdef HAVE_SELECT

            if(m_isTerminating) return ;
            std::map<SOCKET_id,REF_getter<epoll_socket_info> > cc=m_socks->getContainer();
            fd_set readfds;
            fd_set writefds;
            fd_set exceptfds;
            FD_ZERO(&readfds);
            FD_ZERO(&writefds);
            FD_ZERO(&exceptfds);
            std::set<int> socklist;
            for(auto i=cc.begin();
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
                MUTEX_INSPECTOR;
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
                            this->onEPOLLIN(esi);
                        if(FD_ISSET(CONTAINER(esi->get_fd()),&writefds))
                            this->onEPOLLOUT(esi);
                        if(FD_ISSET(CONTAINER(esi->get_fd()),&exceptfds))
                        {
                            this->onEPOLLERR(esi);
                            m_socks->remove(esi->m_id);
                        }

                    }
                }
                else
                {
                    MUTEX_INSPECTOR;
                    if(r==-1)
                    {
                        if(errno==EBADF)
                        {

                        }
                        logErr2("select: errno %d",errno);
                    }
                }
            }
#elif defined (HAVE_EPOLL)

            if(isTerminating_) return ;
//            if(iUtils->isTerminating()) return NULL;
            int nfds;
            {
                MUTEX_INSPECTOR;
                nfds=epoll_wait(MS->multiplexor_->m_handle,events,eventsSIZE,epoll_timeout_millisec_);
            }





            for (int i=0; i<nfds; i++)
            {
                MUTEX_INSPECTOR;
                XTRY;
                SOCKET_id id;
                CONTAINER(id)=events[i].data.u64;
                REF_getter<epoll_socket_info> __EV=MS->getOrNull(id);

                if (!__EV.valid())
                {
                    MUTEX_INSPECTOR;
                    continue;
                }
                if(__EV->closed())
                {
                    logErr2("socked closed");
                    continue;
                }
                if (events[i].events&EPOLLERR)
                {
                    MUTEX_INSPECTOR;
                    onEPOLLERR(__EV,MS);
                    continue;
                }
                if (events[i].events&EPOLLIN )
                {
                    MUTEX_INSPECTOR;
//                    logErr2("epolled sock %d",CONTAINER(__EV->m_fd));
                    onEPOLLIN(__EV,MS);
                }
                if (events[i].events&EPOLLOUT)
                {
                    MUTEX_INSPECTOR;
                    onEPOLLOUT(__EV,MS);
                }
                XPASS;
            }

#elif defined (HAVE_KQUEUE)
            if(isTerminating_) return;

            timespec ts;
            ts.tv_sec=0;
            ts.tv_nsec=epoll_timeout_millisec_*1000;


#define LSTSIZ 32
            struct kevent evList[LSTSIZ];

            int nev = kevent(MS->multiplexor_->m_handle, NULL, 0, evList, LSTSIZ, &ts);
            if(isTerminating_) return;
            if (nev < 0)
            {
                logErr2("kevent error: %d %s", errno, strerror(errno));
                continue;

            }
            for (int i=0; i<nev; i++) {
                MUTEX_INSPECTOR;
                SOCKET_id sid;
                CONTAINER(sid)=(long)evList[i].udata;
                REF_getter<epoll_socket_info> esi=MS->getOrNull(sid);
                if(!esi.valid())
                {
                    if(CONTAINER(sid)!=-1)
                    {
                        DBG(logErr2("kevent: esi == NULL sid %d",CONTAINER(sid)));
                    }
                    continue;
                }
                auto& l=evList[i];

                if(l.filter==EVFILT_WRITE)
                {
                    MUTEX_INSPECTOR;

                    S_LOG("EVFILT_WRITE");
//                    if(l.flags&EV_ADD)
                    {
                        S_LOG("EV_ADD");
                        if(l.flags & EV_EOF)
                        {
                            closeSocket(esi,"EV_EOF4",0,MS);
                            MS->remove(esi->id_);
                        }
                        else if(l.flags & EV_ERROR)
                        {
                            closeSocket(esi,"EV_EOF6",l.data,MS);
                            MS->remove(esi->id_);
                        }
                        else
                        {
                            MUTEX_INSPECTOR;
                            onEPOLLOUT(esi,MS);
                            continue;
                        }
                    }
                }
                else if(l.filter==EVFILT_READ)
                {
                    MUTEX_INSPECTOR;
                    S_LOG("EVFILT_READ");
                    {
                        S_LOG("EV_ADD");
                        if(l.flags & EV_EOF)
                        {
                            MUTEX_INSPECTOR;
                            S_LOG("EV_EOF");
                            closeSocket(esi,"EV_EOF3",l.data,MS);
                            MS->remove(esi->id_);
                        }
                        else if(l.flags & EV_ERROR)
                        {
                            MUTEX_INSPECTOR;
                            S_LOG("EV_ERROR");
                            closeSocket(esi,"EV_ERROR11",l.data,MS);
                            MS->remove(esi->id_);
                        }
                        else
                        {
                            onEPOLLIN(esi,MS);
                            continue;
                        }
                    }

                }
                else logErr2("invalid case %s %d",__FILE__,__LINE__);

            }
#else
#error "dfssdfdsfsdfwqerweqrqw"
#endif
        }
        catch(const std::exception &e)
        {
            logErr2("exception: %s %s %d",e.what(),__FILE__,__LINE__);
        }
        XPASS;
    }
}
void SocketIO::Service::closeSocket(const REF_getter<epoll_socket_info>&esi,const char*reason, int errNo, const     REF_getter<SocketsContainerForSocketIO>& MS)
{
    MUTEX_INSPECTOR;
    if(esi->closed())
    {
        logErr2("  socket already closed %s",reason);
        return;
    }

    if(esi->streamType_==epoll_socket_info::STREAMTYPE_CONNECTED)
    {
        MUTEX_INSPECTOR;
        if(esi->inConnection_)
        {
            passEvent(new socketEvent::ConnectFailed(esi,*esi->request_for_connect_,errNo,esi->m_route));
        }
        else
        {
            passEvent(new socketEvent::Disconnected(esi,reason,esi->m_route));
        }
    }
    else if(esi->streamType_==epoll_socket_info::STREAMTYPE_ACCEPTED)
    {
        MUTEX_INSPECTOR;
        passEvent(new socketEvent::Disaccepted(esi,reason,esi->m_route));

    }
    else throw CommonError("invalid case %s %d",__FILE__,__LINE__);



    XTRY;

    MS->remove(esi->id_);
    esi->close(reason);
    XPASS;
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
    if(socketEventEnum::AddToListenTCP==ID)
        return on_AddToListenTCP((const socketEvent::AddToListenTCP*)e.get());
    if(socketEventEnum::AddToConnectTCP==ID)
        return on_AddToConnectTCP((const socketEvent::AddToConnectTCP*)e.get());
    if(webHandlerEventEnum::RequestIncoming==ID)
        return on_RequestIncoming((const webHandlerEvent::RequestIncoming*)e.get());
    if(systemEventEnum::startService==ID)
        return true;

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
bool  SocketIO::Service::on_AddToListenTCP(const socketEvent::AddToListenTCP*ev)
{
//    printf("@@ AddToListenTCP %s\n",ev->addr.dump().c_str());
    MUTEX_INSPECTOR;

    S_LOG(__FUNCTION__);
    REF_getter<SocketsContainerForSocketIO> MS=m_io_socks_pollers_.getPoller(this);

    int fd=-1;
    switch (ev->addr.family())
    {
    case AF_UNIX:
        fd=::socket (ev->addr.family(), SOCK_STREAM, 0);
        break;
    case AF_INET:
        fd=::socket (ev->addr.family(), SOCK_STREAM, IPPROTO_TCP);
        break;
    case AF_INET6:
        fd=::socket (ev->addr.family(), SOCK_STREAM, IPPROTO_TCP);
        break;
    
    default:
        break;
    }
    REF_getter<epoll_socket_info> nesi(new epoll_socket_info(SOCK_STREAM,epoll_socket_info::STREAMTYPE_LISTENING,ev->socketId,fd,poppedFrontRoute(ev->route),
                                       ev->socketDescription,MS->multiplexor_));



    {
        MUTEX_INSPECTOR;
        int i = 1;
        if(setsockopt(CONTAINER(nesi->fd_),SOL_SOCKET,SO_REUSEADDR,(char *)&i,sizeof(i)))
        {
            closeSocket(nesi,"setsockopt",errno,MS);
            return true;
        }
    }

//    printf("@@ bind %s\n",ev->addr.dump().c_str());
    while (bind (CONTAINER(nesi->fd_),ev->addr.addr(), ev->addr.addrLen()) == -1)
    {
#ifdef DEBUG
        logErr2("Server: bind()  failed: errno %d %s %s (%s) %s %d", errno,strerror(errno),ev->addr.dump().c_str(),ev->route.dump().c_str(),__FILE__,__LINE__);
#else
        logErr2("Server: bind()  failed: errno %d %s %s", errno, strerror(errno),ev->addr.dump().c_str());
#endif
        iUtils->setTerminate(1);
        return true;
    }

//    {
    msockaddr_in local_addr;
    socklen_t len=local_addr.maxAddrLen();
    if(getsockname(CONTAINER(nesi->fd_),local_addr.addr(),&len))
    {
        closeSocket(nesi,"getsockname",errno,MS);
        return true;

    }
    nesi->local_name_.emplace(local_addr);
    logErr2("binded to %s (%s)",local_addr.dump().c_str(),ev->route.dump().c_str());
//    }


    if (listen(CONTAINER(nesi->fd_),(int)512)==-1)
    {
        closeSocket(nesi,"listen",errno,MS);
        return true;
    }

    passEvent(new socketEvent::NotifyBindAddress(local_addr,ev->socketDescription,ev->rebind,nesi->m_route));

    MS->multiplexor_->sockAddReadOnNew(nesi.get());
    MS->add(nesi);
    return true;
}

bool  SocketIO::Service::on_AddToConnectTCP(const socketEvent::AddToConnectTCP*ev)
{
    MUTEX_INSPECTOR;

    XTRY;
    S_LOG(__FUNCTION__);
    REF_getter<SocketsContainerForSocketIO> MS=m_io_socks_pollers_.getPoller(this);

    //int PF=ev->addr.family()==AF_INET?PF_INET:PF_INET6;
    SOCKET_fd sock=-1;
    switch (ev->addr.family())
    {
    case AF_UNIX:
        sock=::socket (ev->addr.family(), SOCK_STREAM, 0);
        break;
    case AF_INET:
        sock=::socket (ev->addr.family(), SOCK_STREAM, IPPROTO_TCP);
        break;
    case AF_INET6:
        sock=::socket (ev->addr.family(), SOCK_STREAM, IPPROTO_TCP);
        break;
    
    default:
        throw CommonError("invalid family %s %d",__FILE__,__LINE__);
        break;
    }
    // SOCKET_fd sock=::socket (ev->addr.family(), SOCK_STREAM, IPPROTO_TCP);
    if (CONTAINER(sock)==-1)
    {
        logErr2("cannot create socket");
        return true;
    }
    REF_getter<epoll_socket_info> nesi=new epoll_socket_info(SOCK_STREAM,epoll_socket_info::STREAMTYPE_CONNECTED,ev->socketId,sock,poppedFrontRoute(ev->route),
        ev->socketDescription,MS->multiplexor_);

    nesi->inConnection_=true;

    if(ev->addr.family()!=AF_UNIX){
        MUTEX_INSPECTOR;
        int i = 1;
        if(setsockopt(CONTAINER(sock),SOL_SOCKET,SO_REUSEADDR,(char *)&i,sizeof(i)))
        {
            logErr2("if(setsockopt(CONTAINER(sock),SOL_SOCKET,SO_REUSEADDR,(char *)&i,sizeof(i)))");
            closeSocket(nesi,"setsockopt",errno,MS);
            return true;
        }
    }

//    if (1)
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
            closeSocket(nesi,"ioctl",errno,MS);
            return true;

        }
    }

    {
        MUTEX_INSPECTOR;
        struct linger l_ {};
        l_.l_onoff=1;
        l_.l_linger=(u_short)5;
        if (::setsockopt(CONTAINER(sock),SOL_SOCKET,SO_LINGER,(char*)&l_, sizeof(l_))!=0)
        {
            closeSocket(nesi,"SP_LINGER",errno,MS);
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
    nesi->request_for_connect_.emplace(ev->addr);
    MS->multiplexor_->sockAddRWOnNew(nesi.get());
    MS->add(nesi);

//    nesi->remote_name=new P_msockaddr_in(sa);
    printf("connect to %s\n",sa.dump().c_str());
    int cres=::connect(CONTAINER(sock),sa.addr(), sa.addrLen());
    if(cres)
    {
#ifndef _WIN32
        if(errno != EINPROGRESS)
        {
            closeSocket(nesi,"connect",errno,MS);
            passEvent(new socketEvent::ConnectFailed(nesi,sa,errno,ev->route));
            return true;
        }

#else
        if(errno==0)
        {
            nesi->inConnection=false;
            passEvent(new socketEvent::Connected(nesi,ev->route));
        }
        else {
            logErr2("errno %d %s",errno, strerror(errno));
            logErr2("if(::connect(CONTAINER(sock),sa.addr(), sa.addrLen()))");

        }
#endif

    }
    else {
        passEvent(new socketEvent::Connected(nesi,ev->route));
    }



    XPASS;
    return true;
}
bool SocketIO::Service::on_RequestIncoming(const webHandlerEvent::RequestIncoming* e)
{
    MUTEX_INSPECTOR;

    HTTP::Response cc(iInstance);
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



