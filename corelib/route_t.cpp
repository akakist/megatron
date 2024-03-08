#include "route_t.h"
#include "IInstance.h"
#include "objectHandler.h"
#include "mutexInspector.h"
#include "listenerBase.h"

std::string route_t::dump() const
{
    XTRY;
    std::vector<std::string> ret;
    for(size_t i=0; i<m_container.size(); i++)
    {
        ret.push_back(m_container[i]->dump());
    }

    return iUtils->join("|",ret);
    XPASS;

}

void SocketRoute::unpack(inBuffer&o)
{
    o>>CONTAINER(id);
}

void SocketRoute::pack(outBuffer&o) const
{
    o<<(int)type<<CONTAINER(id);
}
std::string SocketRoute::dump() const
{
    return "socket:"+std::to_string(CONTAINER(id));
}

void LocalServiceRoute::pack(outBuffer&o) const
{
    o<<(int)type<<id;
}
void LocalServiceRoute::unpack(inBuffer&o)
{
    o>>id;
}
std::string LocalServiceRoute::dump() const
{
    char s[100];
    snprintf(s,sizeof(s),"local:%s",iUtils->serviceName(id).c_str());
    return s;
}
void ListenerRoute::pack(outBuffer&o) const
{
    throw CommonError("try to pack ListenerRoute");
}
void ListenerRoute::unpack(inBuffer&o)
{
    throw CommonError("try to unpack ListenerRoute");
}
std::string ListenerRoute::dump() const
{
    char s[100];
    snprintf(s,sizeof(s),"listener:%s",id->listenerName.c_str());
    return s;
}
void ThreadRoute::pack(outBuffer&o) const
{
    o<<(int)type<<id;
}
void ThreadRoute::unpack(inBuffer&o)
{
    o>>id;
}
std::string ThreadRoute::dump() const
{
    char s[100];
    snprintf(s,sizeof(s),"thread:%d",id);
    return s;
}


void ObjectHandlerRoutePolled::pack(outBuffer&o) const
{
    o<<(int)type<<addr;
}
void ObjectHandlerRouteThreaded::pack(outBuffer&o) const
{
    o<<(int)type<<addr;
}
void ObjectHandlerRoutePolled::unpack(inBuffer&o)
{
    addr=o.get_PSTR();
}
void ObjectHandlerRouteThreaded::unpack(inBuffer&o)
{
    addr=o.get_PSTR();
}

std::string ObjectHandlerRoutePolled::dump() const
{

    const ObjectHandlerRoutePolled* p=this;
    std::string s((char*)&p,sizeof(p));

    return "objectHandlerPolled:"+iUtils->bin2hex(s);
}
std::string ObjectHandlerRouteThreaded::dump() const
{

    const ObjectHandlerRouteThreaded* p=this;
    std::string s((char*)&p,sizeof(p));

    return "ObjectHandlerThreaded:"+iUtils->bin2hex(s);
}

void RemoteAddrRoute::pack(outBuffer&o) const
{
    o<<(int)type<<CONTAINER(addr);
}
void RemoteAddrRoute::unpack(inBuffer&o)
{
    o>>CONTAINER(addr);
}
std::string RemoteAddrRoute::dump() const
{
    return "remote:"+std::to_string(CONTAINER(addr));
}

inBuffer& operator >> (inBuffer&b,route_t & r)
{

    r.unpack(b);
    return b;
}
outBuffer& operator << (outBuffer& b, const route_t & r)
{

    r.pack(b);
    return b;
}


route_t::route_t(const SERVICE_id& id)
{

    m_container.push_front(new LocalServiceRoute(id));
}
route_t::route_t(ListenerBase* id)
{

    m_container.push_front(new ListenerRoute(id));
}
route_t::route_t(ObjectHandlerPolled* id)
{

    std::string s((char*)&id,sizeof(id));

    m_container.push_front(new ObjectHandlerRoutePolled(s));
}
route_t::route_t(ObjectHandlerThreaded* id)
{

    std::string s((char*)&id,sizeof(id));

    m_container.push_front(new ObjectHandlerRouteThreaded(s));
}
route_t::route_t(const std::string &javaCookie,ObjectHandlerPolled* id)
{
    m_container.push_front(new LocalServiceRoute(javaCookie));
    std::string s((char*)&id,sizeof(id));
    m_container.push_front(new ObjectHandlerRoutePolled(s));

}
route_t::route_t(const std::string &javaCookie,ObjectHandlerThreaded* id)
{
    m_container.push_front(new LocalServiceRoute(javaCookie));
    std::string s((char*)&id,sizeof(id));
    m_container.push_front(new ObjectHandlerRouteThreaded(s));

}


void route_t::push_front(const REF_getter<Route>& v)
{

    XTRY;
    m_container.push_front(v);
    XPASS;
}
void route_t::push_front(const route_t& r)
{
    if(r.m_container.size()!=1)
        throw CommonError("if(r.m_container.size()!=1)");
    m_container.push_front(r.m_container[0]);
}

REF_getter<Route> route_t::pop_front()
{

    XTRY;
    if(m_container.size()==0) throw CommonError("route_t::pop_front: m_container.size()==0 %s",_DMI().c_str());

    REF_getter<Route> r=*m_container.begin();
    m_container.pop_front();
    return r;
    XPASS;
}
size_t route_t::size()const
{
    return m_container.size();
}
void route_t::pack(outBuffer&o) const
{

    XTRY;
    o<<m_container.size();
    for(size_t i=0; i<m_container.size(); i++)
    {
        m_container[i]->pack(o);
    }
    XPASS;
}
void route_t::unpack(inBuffer&o)
{

    XTRY;
    size_t sz;
    {
        XTRY;
        o>>sz;
        XPASS;

    }
    for(size_t i=0; i<sz; i++)
    {
        XTRY;
        Route::RouteType type;
        int t;
        {
            XTRY;
            o>>t;
            type=(Route::RouteType)t;
            XPASS;
        }
        switch(type)
        {
        case Route::LISTENER:
            throw CommonError("route unpack for listener");
            break;
        case Route::LOCALSERVICE:
        {
            XTRY;
            Route *r=new LocalServiceRoute();
            r->unpack(o);
            m_container.push_back(r);
            XPASS;
        }
        break;
        case Route::THREAD:
        {
            XTRY;
            Route *r=new ThreadRoute();
            r->unpack(o);
            m_container.push_back(r);
            XPASS;
        }
        break;
        case Route::REMOTEADDR:
        {
            XTRY;
            Route *r=new RemoteAddrRoute();
            r->unpack(o);
            m_container.push_back(r);
            XPASS;
        }
        break;
        case Route::OBJECTHANDLER_POLLED:
        {
            XTRY;
            Route *r=new ObjectHandlerRoutePolled();
            r->unpack(o);
            m_container.push_back(r);
            XPASS;
        }
        break;
        case Route::OBJECTHANDLER_THREADED:
        {
            XTRY;
            Route *r=new ObjectHandlerRouteThreaded();
            r->unpack(o);
            m_container.push_back(r);
            XPASS;
        }
        break;
        case Route::SOCKETROUTE:
        {
            XTRY;
            Route *r=new SocketRoute();
            r->unpack(o);
            m_container.push_back(r);
            XPASS;
        }
        break;
        }
        XPASS;
    }
    {
        XTRY;
        XPASS;
    }
    XPASS;
}
int route_t::operator<(const route_t& a) const
{

    XTRY;
    if(a.m_container.size()!=m_container.size()) return a.m_container.size()<m_container.size();
    for(size_t i=0; i<m_container.size(); i++)
    {
        if(a.m_container[i]->type!=m_container[i]->type) return a.m_container[i]->type < m_container[i]->type;
        switch(a.m_container[i]->type)
        {
        case Route::LOCALSERVICE:
        {
            if(static_cast<const LocalServiceRoute*>(a.m_container[i].operator->())->id != static_cast<const LocalServiceRoute*>(m_container[i].operator->())->id)
                return static_cast<const LocalServiceRoute*>(a.m_container[i].operator->())->id < static_cast<const LocalServiceRoute*>(m_container[i].operator->())->id;
        }
        break;
        case Route::THREAD:
        {
            if(static_cast<const ThreadRoute*>(a.m_container[i].operator->())->id != static_cast<const ThreadRoute*>(m_container[i].operator->())->id)
                return static_cast<const ThreadRoute*>(a.m_container[i].operator->())->id < static_cast<const ThreadRoute*>(m_container[i].operator->())->id;
        }
        break;
        case Route::REMOTEADDR:
        {
            if(CONTAINER(static_cast<const RemoteAddrRoute*>(a.m_container[i].operator->())->addr) != CONTAINER(static_cast<const RemoteAddrRoute*>(m_container[i].operator->())->addr))
                return CONTAINER(static_cast<const RemoteAddrRoute*>(a.m_container[i].operator->())->addr) < CONTAINER(static_cast<const RemoteAddrRoute*>(m_container[i].operator->())->addr);
        }
        break;
        case Route::OBJECTHANDLER_POLLED:
        {
            if(static_cast<const ObjectHandlerRoutePolled*>(a.m_container[i].operator->())->addr != static_cast<const ObjectHandlerRoutePolled*>(m_container[i].operator->())->addr)
                return static_cast<const ObjectHandlerRoutePolled*>(a.m_container[i].operator->())->addr < static_cast<const ObjectHandlerRoutePolled*>(m_container[i].operator->())->addr;
        }
        break;
        case Route::OBJECTHANDLER_THREADED:
        {
            if(static_cast<const ObjectHandlerRouteThreaded*>(a.m_container[i].operator->())->addr != static_cast<const ObjectHandlerRouteThreaded*>(m_container[i].operator->())->addr)
                return static_cast<const ObjectHandlerRouteThreaded*>(a.m_container[i].operator->())->addr < static_cast<const ObjectHandlerRouteThreaded*>(m_container[i].operator->())->addr;
        }
        break;
        case Route::SOCKETROUTE:
        {
            if(CONTAINER(static_cast<const SocketRoute*>(a.m_container[i].operator->())->id) != CONTAINER(static_cast<const SocketRoute*>(m_container[i].operator->())->id))
                return CONTAINER(static_cast<const SocketRoute*>(a.m_container[i].operator->())->id) < CONTAINER(static_cast<const SocketRoute*>(m_container[i].operator->())->id);
        }
        break;
        default:
            throw CommonError("invalid route type %d",a.m_container[i]->type);
        }
    }
    XPASS;
    return 0;
}


bool route_t::operator==(const route_t& a) const
{

    XTRY;
    if(a.m_container.size()!=m_container.size()) return false;
    for(size_t i=0; i<m_container.size(); i++)
    {
        auto at=a.m_container[i]->type;
        if(at!=m_container[i]->type) return false;
        switch(at)
        {
        case Route::LOCALSERVICE:
        {
            if(static_cast<const LocalServiceRoute*>(a.m_container[i].operator->())->id != static_cast<const LocalServiceRoute*>(m_container[i].operator->())->id)
                return false;
        }
        break;
        case Route::THREAD:
        {
            if(static_cast<const ThreadRoute*>(a.m_container[i].operator->())->id != static_cast<const ThreadRoute*>(m_container[i].operator->())->id)
                return false;
        }
        break;
        case Route::REMOTEADDR:
        {
            if(CONTAINER(static_cast<const RemoteAddrRoute*>(a.m_container[i].operator->())->addr) != CONTAINER(static_cast<const RemoteAddrRoute*>(m_container[i].operator->())->addr))
                return false;
        }
        break;
        case Route::OBJECTHANDLER_POLLED:
        {
            if(static_cast<const ObjectHandlerRoutePolled*>(a.m_container[i].operator->())->addr != static_cast<const ObjectHandlerRoutePolled*>(m_container[i].operator->())->addr)
                return false;
        }
        break;
        case Route::OBJECTHANDLER_THREADED:
        {
            if(static_cast<const ObjectHandlerRouteThreaded*>(a.m_container[i].operator->())->addr != static_cast<const ObjectHandlerRouteThreaded*>(m_container[i].operator->())->addr)
                return false;
        }
        break;
        case Route::SOCKETROUTE:
        {
            if(CONTAINER(static_cast<const SocketRoute*>(a.m_container[i].operator->())->id) != CONTAINER(static_cast<const SocketRoute*>(m_container[i].operator->())->id))
                return false;
        }
        break;
        default:
            throw CommonError("invalid route type %d",at);
        }
    }
    XPASS;
    return true;
}
std::string route_t::getLastJavaCookie() const
{
    if(m_container.size()!=1)
        throw CommonError("route_t: if(m_container.size()!=1)");
    auto r=m_container[0];
    if(r.valid())
    {
        if(r->type==Route::LOCALSERVICE)
        {
            LocalServiceRoute* lrt=(LocalServiceRoute* )r.operator ->();
            return lrt->id.getSid();
        }

    }
    throw CommonError("invalid case %s %d",__FILE__,__LINE__);

}
