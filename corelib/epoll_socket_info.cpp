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

#include "Events/System/Net/socket/Accepted.h"
Json::Value epoll_socket_info::__jdump()
{
    Json::Value v;
    XTRY;

    v["id"]=(int)CONTAINER(m_id);
    v["fd"]=CONTAINER(m_fd);
    switch(m_streamType)
    {
    case _ACCEPTED:
        v["streamType"]="ACCEPTED";
        break;
    case _CONNECTED:
        v["streamType"]="CONNECTED";
        break;
    case _LISTENING:
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
    v["peer name"]=remote_name.dump();
    v["local name"]=local_name.dump();
    v["dst route"]=m_route.dump();
    v["closed"]=closed();
    v["outbuffer size"]=(int)m_outBuffer.size();
    v["inbuffer size"]=(int)m_inBuffer.size();
    v["socketDescription"]=socketDescription;
    //v["lastIO"]=int(time(NULL)-lastIO);
    XPASS;
    return v;

}

Json::Value epoll_socket_info::wdump()
{
    return __jdump();
}


void epoll_socket_info::write_(const std::string&s)
{
    XTRY;
    if(!closed())
    {
        XTRY;
        {
            XTRY;
            if(s.size()+m_outBuffer.size()>maxOutBufferSize)
                throw CommonError("socket outBuffer size overflow %ld",maxOutBufferSize);
            m_outBuffer.append(s.data(),s.size());
            XPASS;
        }
        REF_getter<SocketsContainerBase> sockets=m_socketsContainers.getOrNull(ServiceEnum::Socket);

        if(!sockets.valid()) throw CommonError("if(!sockets.valid())  "+_DMI());
        sockets->mod_write(m_id);
        XPASS;
    }
    XPASS;
}
void epoll_socket_info::write_(const char *s, const size_t &sz)
{
    XTRY;
    write_(std::string(s,sz));
    XPASS;
}

void epoll_socket_info::close(const std::string & reason)
{

    XTRY;
    std::string sType;
    if(this->m_streamType==epoll_socket_info::_ACCEPTED)
    {
        sType="_ACCEPTED";
    }
    else if(this->m_streamType==epoll_socket_info::_CONNECTED)
    {
        sType="_CONNECTED";
    }
    else if(this->m_streamType==epoll_socket_info::_LISTENING)
    {
        sType="_LISTENING";
    }
    DBG(logErr2("close reason: %s remote name %s type %s",reason.c_str(),remote_name.dump().c_str(),sType.c_str()));
    if(closed())
    {
        DBG(logErr2("socket already closed %s",_DMI().c_str()));
        return;
    }
    m_closed=true;
    {
        XTRY;
#ifdef _WIN32
        shutdown(CONTAINER(get_fd()),SD_BOTH);
#else
        shutdown(CONTAINER(get_fd()),SHUT_RDWR);
#endif
        XPASS;
    }
    XPASS;

    std::map<SERVICE_id,REF_getter<SocketsContainerBase> > ss=m_socketsContainers.getContainer();
    m_socketsContainers.clear();
    for(auto &i:ss)
    {
        REF_getter<SocketsContainerBase> cc=i.second;

        if(cc.valid())
        {
            cc->remove(this->m_id,reason);
        }
    }
    if(CONTAINER(get_fd())!=-1)
    {
#ifdef _WIN32
        if(!::closesocket(CONTAINER(m_fd)))
#else
        if(!::close(CONTAINER(m_fd)))
#endif
        {
        }
        else
        {
        }
        iUtils->ungetSocketId(m_id);
        CONTAINER(m_id)=-1;

    }
}

epoll_socket_info::~epoll_socket_info()
{
    XTRY;
    if(CONTAINER(m_fd)==-1)
    {
        DBG(logErr2("inval behaviour if(CONTAINER(m_fd)==-1) %s",_DMI().c_str()));
        return;
    }
    /// TOCLOSE
    DBG(logErr2("::close int epoll_socket_info destructor %d",CONTAINER(m_fd)));
#ifdef _WIN32
    if(!::closesocket(CONTAINER(m_fd)))
#else
    if(!::close(CONTAINER(m_fd)))
#endif
    {
    }
    else
    {
    }
    iUtils->ungetSocketId(m_id);
    XPASS;
}





socketBufferOut::~socketBufferOut()
{
    if(buffer)
    {
        free(buffer);
        buffer=NULL;
    }
}
socketBufferOut::socketBufferOut(const char* data, size_t sz)
{
    buffer=(char*) malloc(sz);
    if(!buffer) throw CommonError("malloc: errno %d",errno);
    memcpy(buffer,data,sz);
    size=sz;
    curpos=0;
}
int socketBufferOut::sendSocket(const SOCKET_fd &fd)
{
    M_LOCK(this);
    int res=::send(CONTAINER(fd),buffer+curpos,size-curpos,0);
    if(res>0) curpos+=res;
    if(curpos==size)
    {
        free(buffer);
        buffer=NULL;
        curpos=0;
        size=0;
    }
    return res;
}

void socketBuffersOut::append(const char* data, size_t sz)
{
    M_LOCK(this);
    container.push_back(new socketBufferOut(data,sz));
}
size_t socketBuffersOut::size()
{
    M_LOCK(this);
    return container.size();
}
int socketBuffersOut::send(const SOCKET_fd &fd)
{
    M_LOCK(this);
    REF_getter<socketBufferOut> p(NULL);
    {
        if(container.size())
        {
            p=container[0];
        }
    }
    if(p.valid())
    {
        int res=p->sendSocket(fd);
        if(p->buffer==NULL)
        {
            container.pop_front();
        }
        return res;
    }
    return 0;
}

bool epoll_socket_info::closed()
{
    return m_closed;
}

epoll_socket_info::epoll_socket_info(const STREAMTYPE &_strtype,const SOCKET_id& _id,const SOCKET_fd& _fd,
                                     const route_t& _route,
                                     const REF_getter<SocketsContainerBase> &__socks,
                                     const int64_t& _maxOutBufferSize, const std::string& _socketDescription,bool (*__bufferVerify)(const std::string&)):
    m_streamType(_strtype),m_id(_id),m_fd(_fd),m_closed(false),m_route(_route),markedToDestroyOnSend(false),
    inConnection(false),maxOutBufferSize(_maxOutBufferSize), socketDescription(_socketDescription), bufferVerify(__bufferVerify)
{
    m_socketsContainers.add(ServiceEnum::Socket,__socks);
}

void epoll_socket_info::_socketsContainers::add(const SERVICE_id& sid, const REF_getter<SocketsContainerBase> & S)
{
    M_LOCK(this);
    container.insert(std::make_pair(sid,S));
}
REF_getter<SocketsContainerBase> epoll_socket_info::_socketsContainers::getOrNull(const SERVICE_id& sid)
{
    M_LOCK(this);
    std::map<SERVICE_id,REF_getter<SocketsContainerBase> > ::iterator i=container.find(sid);
    if(i!=container.end())
        return i->second;

    return NULL;
}
std::map<SERVICE_id,REF_getter<SocketsContainerBase> > epoll_socket_info::_socketsContainers::getContainer()
{
    M_LOCK(this);
    return container;
}
void epoll_socket_info::_socketsContainers::clear()
{
    M_LOCK(this);
    container.clear();
}
void epoll_socket_info::_inBuffer::append(const char* data, size_t size)
{
    M_LOCK(this);
    if(_mx_data.capacity()<_mx_data.size()+10000)
    {
        _mx_data.reserve(100000+_mx_data.size());
    }

    _mx_data.append(data,size);
}
size_t epoll_socket_info::_inBuffer::size()
{
    M_LOCK(this);
    return _mx_data.size();
}
std::string epoll_socket_info::_inBuffer::extract_all()
{
    M_LOCK(this);
    std::string ret=_mx_data;
    _mx_data.clear();
    return ret;
}
