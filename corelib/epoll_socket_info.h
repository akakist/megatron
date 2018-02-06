#ifndef __________EPOLL_SOCKET_INFO_____H
#define __________EPOLL_SOCKET_INFO_____H

#include "route_t.h"
#include "msockaddr_in.h"
#include "SOCKET_fd.h"
#include "SOCKET_id.h"
#include "mutexable.h"
#include "REF.h"
#include "IConfigObj.h"
#include "json/json.h"
#include "compat_win32.h"
#include "webDumpable.h"
#include "socketsContainer.h"
#include <set>

/**
* Универсальная обертка для сокет селектора
*/


struct socketBufferOut: public Refcountable, public Mutexable
{
    char *buffer;
    size_t curpos;
    size_t size;
    ~socketBufferOut();
    socketBufferOut(const char* data, size_t sz);
    int sendSocket(const SOCKET_fd &fd);
};

class   socketBuffersOut: public Mutexable
{
    std::deque<REF_getter<socketBufferOut> > container;
public:
    void append(const char* data, size_t sz);
    size_t size();
    int send(const SOCKET_fd &fd);
};
class epoll_socket_info;
/**
* Универсальная обертка для сокета
*/
class epoll_socket_info:public Refcountable, public WebDumpable
{

public:
    enum STREAMTYPE
    {
        _ACCEPTED,
        _CONNECTED,
        _LISTENING,
    };
    enum CloseType
    {
        CONNECT_FAILED,
        DISCONNECTED,
        DIACCEPTED,
    };
    /// тип сокета   _ACCEPTED,  _CONNECTED,  _LISTENING,
    const STREAMTYPE m_streamType;
    /// уникальный ИД
    SOCKET_id m_id;
private:
    /// filedescriptor
    SOCKET_fd m_fd;
public:
    SOCKET_fd get_fd()
    {
        return m_fd;
    }
    bool closed();
private:
    bool m_closed;
public:

    /// маршрут для потребителя
    const route_t m_route;
    /// флаг, означаюший, что сокет следует Close после посылки всех данных
    bool markedToDestroyOnSend;

    struct _socketsContainers: public Mutexable
    {
    private:
        std::map<SERVICE_id,REF_getter<SocketsContainerBase> > container;
    public:
        void add(const SERVICE_id& sid, const REF_getter<SocketsContainerBase> & S);
        REF_getter<SocketsContainerBase> getOrNull(const SERVICE_id& sid);
        std::map<SERVICE_id,REF_getter<SocketsContainerBase> > getContainer();
        void clear();

    };
    _socketsContainers m_socketsContainers;

    socketBuffersOut m_outBuffer;
    /// входной буфер
    struct _inBuffer: public Mutexable
    {
        std::string _mx_data;
        void append(const char* data, size_t size);
        size_t size();
        std::string extract_all();
    };
    _inBuffer m_inBuffer;

    /// флаг, показывающий, что сокет находится в стадии соединения.
    bool inConnection;

    /// удаленный адрес пира
    msockaddr_in remote_name;

    /// локальный адрес пира
    msockaddr_in local_name;
    const unsigned int maxOutBufferSize;
    const std::string socketDescription;
    bool (*bufferVerify)(const std::string& s);


    epoll_socket_info(const STREAMTYPE &_strtype,const SOCKET_id& _id,const SOCKET_fd& _fd, const route_t& _route,
                      const REF_getter<SocketsContainerBase> &__socks,
                      const int64_t& _maxOutBufferSize, const std::string& _socketDescription,         bool (*__bufferVerify)(const std::string&));
    virtual ~epoll_socket_info();
    Json::Value wdump();
    std::string wname() {
        return "epoll_socket_info";
    }
    //time_t lastIO;

    /// записать данные
    void write_(const char *s, const size_t &sz);
    /// записать данные
    void write_(const std::string&s);
    /// Close
    void close(const std::string & reason);
    Json::Value __jdump();
};


#endif
