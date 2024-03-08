#ifndef __________EPOLL_SOCKET_INFO_____H
#define __________EPOLL_SOCKET_INFO_____H

#include "networkMultiplexor.h"

#include <deque>
#include "REF.h"
#include "SOCKET_fd.h"
#include "webDumpable.h"
#include "SOCKET_id.h"
#include "route_t.h"

/**
* Wrapper for socket selector
*/

struct P_msockaddr_in: public Refcountable
{
    msockaddr_in addr;
    P_msockaddr_in(const msockaddr_in& sa)
        : addr(sa)
    {

    }
};
//struct socketBufferOut: public Refcountable, public Mutexable
//{
//    char *buffer;
//    size_t curpos;
//    size_t size;
//    ~socketBufferOut();
//    socketBufferOut(const char* data, size_t sz);
//    int sendSocket(const SOCKET_fd &fd);
//};

class   socketBuffersOut: public Mutexable
{
    std::string container;
public:
    void append(epoll_socket_info *esi, const char* data, size_t sz);
    size_t size();
    int send(const SOCKET_fd &fd, epoll_socket_info *esi);
};
class epoll_socket_info;
/**
* Socket wrapper
*/
struct NetworkMultiplexor;

class epoll_socket_info:public Refcountable, public WebDumpable
{

public:
    const int m_socketType; /// SOCK_STREAM, SOCK_DGRAM
    enum STREAMTYPE
    {
        STREAMTYPE_ACCEPTED,
        STREAMTYPE_CONNECTED,
        STREAMTYPE_LISTENING,
        STREAMTYPE_UDP_ASSOC
    };

    /// socket type   _ACCEPTED,  _CONNECTED,  _LISTENING,
    const STREAMTYPE m_streamType;

    /// unique ID
    SOCKET_id m_id;

private:

public:
    /// filedescriptor
    SOCKET_fd m_fd;
//    SOCKET_fd get_fd()
//    {
//        return m_fd;
//    }

    bool closed();

    /// routing
    const route_t m_route;

    /// if true socket must be closed after flush data
    bool markedToDestroyOnSend;



    /// out buffer
    socketBuffersOut m_outBuffer;

    /// in buffer
    struct _inBuffer: public Mutexable
    {
        std::string _mx_data;
        void append(const char* data, size_t size);
        size_t size();
        std::string extract_all();
    };
    _inBuffer m_inBuffer;

    /// socket in ::connect state
    bool inConnection;

    /// remote peer name
    ///
    ///
    REF_getter<P_msockaddr_in> request_for_connect;
    REF_getter<P_msockaddr_in> local_name;
    REF_getter<P_msockaddr_in> remote_name;

    /// local peer name
//    msockaddr_in local_name;

    /// m_outBuffer max size
//    const unsigned int maxOutBufferSize;

    /// any text of socket for debugging
    const char* socketDescription;

    ///  used to check buffer available for processing

    REF_getter<NetworkMultiplexor> multiplexor;


    epoll_socket_info(const int& socketType, const STREAMTYPE &_streamtype,const SOCKET_id& _id,const SOCKET_fd& _fd, const route_t& _route,
                      const char* _socketDescription,
                      const REF_getter<NetworkMultiplexor>& _multiplexor);

    virtual ~epoll_socket_info();

    /// implementation of webDumpable (display in http)
    Json::Value wdump();
    std::string wname() {
        return "epoll_socket_info";
    }

    /// write buffer
    void write_(const char *s, const size_t &sz);
    void write_(const std::string&s);
    void write_(const REF_getter<refbuffer>&s);

    void close(const std::string & reason);

    Json::Value __jdump();
    std::map<int,REF_getter<Refcountable> > additions;
#ifdef HAVE_KQUEUE
    bool ev_read_added;
    bool ev_write_added;
#elif defined(HAVE_EPOLL)
    bool ev_added;
#elif defined(HAVE_SELECT)

#else
#error "HAVE iundef22"
#endif
};


#endif
