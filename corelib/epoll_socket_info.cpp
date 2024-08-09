#include "IUtils.h"
#include <unistd.h>
#ifdef _WIN32
#include <Winsock2.h>
#endif
#include "epoll_socket_info.h"
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
Json::Value epoll_socket_info::__jdump()
{
    Json::Value v;
    XTRY;

    v["id"]=(int)CONTAINER(id_);
    v["fd"]=CONTAINER(fd_);
    switch(streamType_)
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

    v["outBufferSize"]=(int)outBuffer_.size();
    v["inBufferSize"]=(int)inBuffer_.size();
    if(outBuffer_.size())
    {
        auto s=outBuffer_.getAll();
        v["outBuffer"]=iUtils->bin2hex(s);
    }
    if(inBuffer_.size())
    {
        std::string s;
        {
            R_LOCK(inBuffer_.lk);
            s=inBuffer_._mx_data;
        }
        v["inBuffer"]=iUtils->bin2hex(s);

    }


    v["in connection"]=inConnection_;
    v["dst route"]=m_route.dump();
    v["closed"]=closed();
    v["socketDescription"]=socketDescription_;
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
            outBuffer_.append(this,s);
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
#ifdef KALL
void epoll_socket_info::write_(const REF_getter<refbuffer> &s)
{
    MUTEX_INSPECTOR;
    XTRY;
    write_(std::string((char*)s->buffer,s->size_));

    XPASS;
}
#endif
void epoll_socket_info::close(const char* reason)
{
    MUTEX_INSPECTOR;

    DBG(logErr2("epoll_socket_info::close %s",reason));
    XTRY;
    std::string sType;
    if(CONTAINER(fd_)==-1)
    {
        return;
    }

    if(this->streamType_==epoll_socket_info::STREAMTYPE_ACCEPTED)
    {
        sType="_ACCEPTED";
    }
    else if(this->streamType_==epoll_socket_info::STREAMTYPE_CONNECTED)
    {
        sType="_CONNECTED";
    }
    else if(this->streamType_==epoll_socket_info::STREAMTYPE_LISTENING)
    {
        sType="_LISTENING";
    }
    DBG(logErr2("close reason: %s remote name %s type %s fd %d",reason.c_str(),remote_name().dump().c_str(),sType.c_str(),CONTAINER(fd_)));
    if(closed())
    {
        DBG(logErr2("socket already closed %s",_DMI().c_str()));
        return;
    }
    XPASS;

    if(CONTAINER(fd_)!=-1)
    {
#ifdef _WIN32
        if(!::closesocket(CONTAINER(m_fd)))
#else
        if(!::close(CONTAINER(fd_)))
#endif
        {
        }
        CONTAINER(id_)=-1;
        CONTAINER(fd_)=-1;

    }

}

epoll_socket_info::~epoll_socket_info()
{
    try {
        if(CONTAINER(fd_)!=-1)
        {
            DBG(logErr2(" %s on nonclosed sock %s",__PRETTY_FUNCTION__,socketDescription_));
#ifdef _WIN32
            if(!::closesocket(CONTAINER(m_fd)))
#else
            if(!::close(CONTAINER(fd_)))
#endif
            {
//                printf(RED("::close() error %d %s"),errno,strerror(errno));
            }
            CONTAINER(id_)=-1;
            CONTAINER(fd_)=-1;
        }


    } catch(...) {}
}






void socketBuffersOut::append(epoll_socket_info* esi,const std::string&s)
{
    MUTEX_INSPECTOR;
    {
        W_LOCK(lk);
        container_+=s;
    }
    esi->multiplexor_->sockStartWrite(esi);

}
size_t socketBuffersOut::size()
{
    MUTEX_INSPECTOR;
    R_LOCK(lk);
    return container_.size();
}
int socketBuffersOut::send(const SOCKET_fd &fd, epoll_socket_info* esi)
{
    MUTEX_INSPECTOR;
    int res=0;
    int restSize=0;
    {
        W_LOCK(lk);
        if(container_.size())
        {
            res=::send(CONTAINER(fd),container_.data(),container_.size(),0);
            if(res>0)
            {
                if(res==container_.size())
                {
                    container_.clear();
                }
                else
                {
                    container_=container_.substr(res,container_.size()-res);
                }
            }
        }

        if(container_.size()==0)
            esi->multiplexor_->sockStopWrite(esi);

    }
    return res;
}

bool epoll_socket_info::closed()
{
    return CONTAINER(fd_)==-1;
}

epoll_socket_info::epoll_socket_info(const int &_socketType, const STREAMTYPE &_streamtype, const SOCKET_id& _id, const SOCKET_fd& _fd,
                                     const route_t& _route, const char *_socketDescription, const REF_getter<NetworkMultiplexor> &_multiplexor):
    socketType_(_socketType),
    streamType_(_streamtype),
    id_(_id),
    fd_(_fd),
    m_route(_route),
    markedToDestroyOnSend_(false),
    inConnection_(false),
    socketDescription_(_socketDescription),
    multiplexor_(_multiplexor)
#ifdef HAVE_KQUEUE
    ,
    ev_read_added_(false),
    ev_write_added_(false)
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
    W_LOCK(lk);
    _mx_data.append(data,size);
}
size_t epoll_socket_info::_inBuffer::size()
{
    MUTEX_INSPECTOR;
    R_LOCK(lk);
    return _mx_data.size();
}
std::string epoll_socket_info::_inBuffer::extract_all()
{
    MUTEX_INSPECTOR;
    W_LOCK(lk);
//    return std::move(_mx_data);
//    std::string ret=std::move(_mx_data);
    std::string ret=_mx_data;
    _mx_data.clear();
    return ret;

}
