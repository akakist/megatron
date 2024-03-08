#ifdef _WIN32
#include <Winsock2.h>
#endif
#include "epoll_socket_info.h"
#include "IInstance.h"
#ifdef __linux__
#include <sys/epoll.h>
#endif
#ifdef __APPLE__

#endif
#include "json/json.h"
#ifdef _WIN32
#include "compat_win32.h"
#include <winsock2.h>
#endif
#include "mutexInspector.h"
#include "colorOutput.h"
Json::Value epoll_socket_info::__jdump()
{
    Json::Value v;
    XTRY;

    v["id"]=(int)CONTAINER(m_id);
    v["fd"]=CONTAINER(m_fd);
    switch(m_streamType)
    {
    case STREAMTYPE_ACCEPTED:
        v["streamType"]="ACCEPTED";
        break;
    case STREAMTYPE_CONNECTED:
        v["streamType"]="CONNECTED";
        break;
    case STREAMTYPE_LISTENING:
        v["streamType"]="LISTENING";
        break;
    default:
        throw CommonError("invalid streamtype %s %d",__FILE__,__LINE__);
    }
    {
        v["outBufferSize"]=(int)m_outBuffer.size();
        {
            M_LOCK(m_inBuffer);
            v["inBufferSize"]=(int)m_inBuffer._mx_data.size();
        }
    }
    v["in connection"]=inConnection;
//    v["peer name"]=remote_name->addr.dump();
//    v["local name"]=local_name.dump();
    v["dst route"]=m_route.dump();
    v["closed"]=closed();
    v["outbuffer size"]=(int)m_outBuffer.size();
    v["inbuffer size"]=(int)m_inBuffer.size();
    v["socketDescription"]=socketDescription;
    XPASS;
    return v;

}

Json::Value epoll_socket_info::wdump()
{
    return __jdump();
}


void epoll_socket_info::write_(const std::string&s)
{
    MUTEX_INSPECTOR;
    XTRY;
    if(!closed())
    {
        XTRY;
        {
            XTRY;
//            if(s.size()+m_outBuffer.size()>maxOutBufferSize)
//                throw CommonError("socket outBuffer size overflow %ld",maxOutBufferSize);
            m_outBuffer.append(this,s.data(),s.size());
            XPASS;
        }
        XPASS;
    }
    XPASS;
}
void epoll_socket_info::write_(const char *s, const size_t &sz)
{
    MUTEX_INSPECTOR;
    XTRY;
    write_(std::string(s,sz));

    XPASS;
}
void epoll_socket_info::write_(const REF_getter<refbuffer> &s)
{
    MUTEX_INSPECTOR;
    XTRY;
    write_(std::string((char*)s->buffer,s->size_));

    XPASS;
}

void epoll_socket_info::close(const std::string & reason)
{
    MUTEX_INSPECTOR;

    DBG(logErr2("epoll_socket_info::close %s",reason.c_str()));
    XTRY;
    std::string sType;
    if(CONTAINER(m_fd)==-1)
    {
        return;
    }

    if(this->m_streamType==epoll_socket_info::STREAMTYPE_ACCEPTED)
    {
        sType="_ACCEPTED";
    }
    else if(this->m_streamType==epoll_socket_info::STREAMTYPE_CONNECTED)
    {
        sType="_CONNECTED";
    }
    else if(this->m_streamType==epoll_socket_info::STREAMTYPE_LISTENING)
    {
        sType="_LISTENING";
    }
    DBG(logErr2("close reason: %s remote name %s type %s fd %d",reason.c_str(),remote_name->addr.dump().c_str(),sType.c_str(),CONTAINER(m_fd)));
    if(closed())
    {
        DBG(logErr2("socket already closed %s",_DMI().c_str()));
        return;
    }
    {
        XTRY;
//#ifdef _WIN32
//        shutdown(CONTAINER(get_fd()),SD_BOTH);
//#else
//        shutdown(CONTAINER(get_fd()),SHUT_RDWR);
//#endif
        XPASS;
    }
    XPASS;

    if(CONTAINER(m_fd)!=-1)
    {
#ifdef _WIN32
        if(!::closesocket(CONTAINER(m_fd)))
#else
        if(!::close(CONTAINER(m_fd)))
#endif
        {
        }
        CONTAINER(m_id)=-1;
        CONTAINER(m_fd)=-1;

    }

}

epoll_socket_info::~epoll_socket_info()
{
    try {
        if(CONTAINER(m_fd)!=-1)
        {
            logErr2(" %s on nonclosed sock",__PRETTY_FUNCTION__);
#ifdef _WIN32
            if(!::closesocket(CONTAINER(m_fd)))
#else
            if(!::close(CONTAINER(m_fd)))
#endif
            {
                printf(RED("::closeB %d %s"),errno,strerror(errno));
            }
            CONTAINER(m_id)=-1;
            CONTAINER(m_fd)=-1;
        }


    } catch(...) {}
}





//socketBufferOut::~socketBufferOut()
//{
//    if(buffer)
//    {
//        free(buffer);
//        buffer=NULL;
//    }
//}
//socketBufferOut::socketBufferOut(const char* data, size_t sz)
//{
//    buffer=(char*) malloc(sz);
//    if(!buffer) throw CommonError("malloc: errno %d",errno);
//    memcpy(buffer,data,sz);
//    size=sz;
//    curpos=0;
//}
//int socketBufferOut::sendSocket(const SOCKET_fd &fd)
//{
//    M_LOCK(this);

//    int res=::send(CONTAINER(fd),buffer+curpos,size-curpos,0);
//    if(res>0) curpos+=res;
//    if(curpos==size)
//    {
//        free(buffer);
//        buffer=NULL;
//        curpos=0;
//        size=0;
//    }
//    return res;
//}

void socketBuffersOut::append(epoll_socket_info* esi,const char* data, size_t sz)
{
    MUTEX_INSPECTOR;
    M_LOCK(this);
    container+=std::string(data,sz);
    esi->multiplexor->sockStartWrite(esi);

}
size_t socketBuffersOut::size()
{
    MUTEX_INSPECTOR;
    M_LOCK(this);
    return container.size();
}
int socketBuffersOut::send(const SOCKET_fd &fd, epoll_socket_info* esi)
{
    MUTEX_INSPECTOR;
    int res=0;
    int restSize=0;
    {
        M_LOCK(this);
        if(container.size())
        {
            res=::send(CONTAINER(fd),container.data(),container.size(),0);
            if(res>0)
            {
                if(res==container.size())
                {
                    container.clear();
                }
                else
                {
                    container=container.substr(res,container.size()-res);
                }
            }
        }

        if(container.size()==0)
            esi->multiplexor->sockStopWrite(esi);

    }
    return res;
}

bool epoll_socket_info::closed()
{
    return CONTAINER(m_fd)==-1;
}

epoll_socket_info::epoll_socket_info(const int &_socketType, const STREAMTYPE &_streamtype, const SOCKET_id& _id, const SOCKET_fd& _fd,
                                     const route_t& _route, const char *_socketDescription, const REF_getter<NetworkMultiplexor> &_multiplexor):
    m_socketType(_socketType),
    m_streamType(_streamtype),
    m_id(_id),
    m_fd(_fd),
    m_route(_route),
    markedToDestroyOnSend(false),
    inConnection(false),
//    maxOutBufferSize(_maxOutBufferSize),
    socketDescription(_socketDescription),
    request_for_connect(NULL),
    remote_name(NULL),
    local_name(NULL),
    multiplexor(_multiplexor)
#ifdef HAVE_KQUEUE
    ,
    ev_read_added(false),
    ev_write_added(false)
#elif defined(HAVE_EPOLL)
    ,
    ev_added(false)
#elif defined(HAVE_SELECT)

    // ev_added(false)
#else
#error "HAVE UNDEF"
#endif

{
}

void epoll_socket_info::_inBuffer::append(const char* data, size_t size)
{
    M_LOCK(this);
//    if(_mx_data.capacity()<_mx_data.size()+10000)
//    {
//        _mx_data.reserve(100000+_mx_data.size());
//    }

    _mx_data.append(data,size);
}
size_t epoll_socket_info::_inBuffer::size()
{
    MUTEX_INSPECTOR;
    M_LOCK(this);
    return _mx_data.size();
}
std::string epoll_socket_info::_inBuffer::extract_all()
{
    MUTEX_INSPECTOR;
    M_LOCK(this);
    std::string ret=std::move(_mx_data);
    _mx_data.clear();
    return ret;
}
