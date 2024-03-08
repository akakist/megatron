#ifndef _________ROUTE_T__________H
#define _________ROUTE_T__________H
#include "commonError.h"
#include "ioBuffer.h"
#include "SERVICE_id.h"
#include "REF.h"
#include "msockaddr_in.h"
#include "CONTAINER.h"
#include "SOCKET_id.h"
#include <set>
//#include "listenerBase.h"

/// event routing

class Route:public Refcountable
{
public:
    enum RouteType
    {
        LOCALSERVICE            =1,
        REMOTEADDR              =2,
        OBJECTHANDLER_POLLED    =3,
        SOCKETROUTE             =4,
        OBJECTHANDLER_THREADED  =5,
        THREAD                  =6,
        LISTENER                =7
    };
    RouteType type;
    Route(RouteType t):type(t) {}
    virtual void pack(outBuffer&o) const =0;
    virtual void unpack(inBuffer&o)=0;
    virtual std::string dump()const=0;
    virtual ~Route() {}
};
class ObjectHandlerPolled;
class ObjectHandlerThreaded;
class ListenerBase;
class route_t
{

    std::deque<REF_getter<Route> > m_container;
public:
    route_t() {}
    route_t(const SERVICE_id& id);
    route_t(ObjectHandlerPolled* id);
    route_t(ObjectHandlerThreaded* id);
    route_t(ListenerBase* id);
    route_t(const std::string &javaCookie,ObjectHandlerPolled* id);
    route_t(const std::string &javaCookie,ObjectHandlerThreaded* id);

    std::string dump() const;
    void push_front(const REF_getter<Route>& v);
    void push_front(const route_t& r);
    REF_getter<Route> pop_front();
    size_t size()const ;
    void pack(outBuffer&o) const;
    void unpack(inBuffer&o);
    int operator<(const route_t& a) const;
    bool operator==(const route_t& a) const;
    std::string getLastJavaCookie() const;

};
outBuffer& operator << (outBuffer& b, const route_t & r);
inBuffer& operator >> (inBuffer&b,route_t & r);

class LocalServiceRoute:public Route
{
public:
    SERVICE_id id;
    LocalServiceRoute(const SERVICE_id& _id): Route(Route::LOCALSERVICE),id(_id) {}
    LocalServiceRoute():Route(Route::LOCALSERVICE) {}
    void unpack(inBuffer&o) final;
    void pack(outBuffer&o) const final;
    std::string dump()const final;
    ~LocalServiceRoute() {}
};
class ListenerRoute:public Route
{
public:
    ListenerBase* id;
    ListenerRoute(ListenerBase* _id): Route(Route::LISTENER),id(_id) {}
    ListenerRoute():Route(Route::LISTENER),id(NULL) {}
    void unpack(inBuffer&o) final;
    void pack(outBuffer&o) const final;
    std::string dump()const final;
    ~ListenerRoute() {}
};
class ThreadRoute:public Route
{
public:
    int id;
    ThreadRoute(const int& _id): Route(Route::THREAD),id(_id) {}
    ThreadRoute():Route(Route::THREAD) {}
    void unpack(inBuffer&o) final;
    void pack(outBuffer&o) const final;
    std::string dump()const final;
    ~ThreadRoute() {}
};

class SocketRoute:public Route
{
public:
    SOCKET_id id;
    SocketRoute(const SOCKET_id& _id): Route(Route::SOCKETROUTE),id(_id) {}
    SocketRoute():Route(Route::SOCKETROUTE)
    {
        CONTAINER(id)=0L;
    }
    void unpack(inBuffer&o) final;
    void pack(outBuffer&o) const final;
    std::string dump() const final;
    ~SocketRoute() {}
};
class RemoteAddrRoute:public Route
{
public:
    SOCKET_id addr;
    RemoteAddrRoute(const SOCKET_id & _addr): Route(Route::REMOTEADDR),addr(_addr) {}
    RemoteAddrRoute(): Route(Route::REMOTEADDR)
    {
        CONTAINER(addr)=0L;
    }
    std::string dump() const final;
    void unpack(inBuffer&o) final;
    void pack(outBuffer&o) const final;
    ~RemoteAddrRoute() {}
};

class ObjectHandlerRoutePolled:public Route
{
public:
    std::string  addr;
    ObjectHandlerRoutePolled(const std::string&  _addr): Route(Route::OBJECTHANDLER_POLLED),addr(_addr) {}
    ObjectHandlerRoutePolled(): Route(Route::OBJECTHANDLER_POLLED) {}
    std::string dump() const final;
    void unpack(inBuffer&o) final;
    void pack(outBuffer&o) const final;
    ~ObjectHandlerRoutePolled() {}
};
class ObjectHandlerRouteThreaded:public Route
{
public:
    std::string  addr;
    ObjectHandlerRouteThreaded(const std::string&  _addr): Route(Route::OBJECTHANDLER_THREADED),addr(_addr) {}
    ObjectHandlerRouteThreaded(): Route(Route::OBJECTHANDLER_THREADED) {}
    std::string dump() const final;
    void unpack(inBuffer&o) final;
    void pack(outBuffer&o) const final;
    ~ObjectHandlerRouteThreaded() {}
};

#endif
