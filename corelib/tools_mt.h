#ifndef _______TOOOLS___H
#define _______TOOOLS___H
#include "ioBuffer.h"
#include "route_t.h"
#include "IInstance.h"
#include "IRPC.h"
#include <stdarg.h>

#include "msockaddr_in.h"
#include "Events/System/Net/rpc/Accepted.h"
#include "Events/System/timer/SetAlarm.h"
#include "Events/System/timer/SetTimer.h"
#include "Events/System/timer/ResetAlarm.h"
#include "Events/System/timer/StopAlarm.h"
#include "Events/Tools/errorDispatcher/SendMessage.h"
#include "broadcaster.h"

/// some tools used in DFS

inline unsigned short getRpcExternalListenPortMain(IInstance* iInstance)
{

    UnknownBase* unk=iInstance->getServiceOrCreate(ServiceEnum::RPC);
    if(!unk) throw CommonError("!if(!unk)"+_DMI());
    IRPC* rpc=static_cast<IRPC*>(unk->cast(UnknownCast::IRPC));
    if(!rpc) throw CommonError("!rpc"+_DMI());
    unsigned short port=rpc->getExternalListenPortMain();
#ifndef __MOBILE__
    if(port==0)
        throw CommonError("port==0: rpc->getExternalListenPortMain() return 0"+_DMI());
#endif

    return port;
}

inline std::set<msockaddr_in> getRpcInternalListenAddrs(IInstance* iInstance)
{
    UnknownBase* unk=iInstance->getServiceOrCreate(ServiceEnum::RPC);
    if(!unk) throw CommonError("!if(!unk)"+_DMI());
    IRPC* irpc=static_cast<IRPC*>(unk->cast(UnknownCast::IRPC));
    if(!irpc) throw CommonError("if(!irpc) "+_DMI());

    return irpc->getInternalListenAddrs();
}
inline std::string dump(const std::set<unsigned short> listenPort)
{
    std::vector<std::string> v;
    for(std::set<unsigned short>::const_iterator i=listenPort.begin(); i!=listenPort.end(); i++)
    {
        v.push_back(iUtils->toString(ntohs(*i)));
    }
    return iUtils->join(",",v);
}
inline std::string dump(const std::set<msockaddr_in> listenPort)
{
    std::vector<std::string> v;
    for(std::set<msockaddr_in>::const_iterator i=listenPort.begin(); i!=listenPort.end(); i++)
    {
        v.push_back(i->dump());
    }
    return iUtils->join(",",v);
}


inline std::vector<std::string> conv_s2v(const std::set<std::string>&v)
{
    std::vector<std::string> out;
    for(std::set<std::string>::const_iterator i=v.begin(); i!=v.end(); i++)
    {
        out.push_back(*i);
    }
    return out;
}
inline bool UuidIsCyclic(const std::vector<msockaddr_in>&uuid)
{
    std::map<msockaddr_in,int> m;
    for(size_t i=0; i<uuid.size(); i++)
    {
        if(!m.count(uuid[i]))
            m[uuid[i]]=1;
        else
            return true;
    }
    return false;
}

inline route_t poppedFrontRoute(const route_t&r)
{
    route_t rt=r;
    rt.pop_front();
    return rt;
}
template <class T>
T mabs(T a)
{
    if(a<0) return -a;
    return a;
}

template <class T>
REF_getter<refbuffer> outV(T a)
{
    outBuffer o;
    o<<a;
    return o.asString();

}
template <class T1,class T2>
REF_getter<refbuffer> outV(const T1& a, const T2& b)
{
    outBuffer o;
    o<<a<<b;
    return o.asString();

}
template <class T1,class T2,class T3>
REF_getter<refbuffer> outV(const T1& a, const T2& b, const T3& c)
{
    outBuffer o;
    o<<a<<b<<c;
    return o.asString();

}
template <class T1,class T2,class T3,class T4>
REF_getter<refbuffer> outV(const T1& a, const T2& b, const T3& c,const T4& d)
{
    outBuffer o;
    o<<a<<b<<c<<d;
    return o.asString();

}
template <class T1,class T2,class T3,class T4,class T5>
REF_getter<refbuffer> outV(const T1& a, const T2& b, const T3& c,const T4& d,const T5& e)
{
    outBuffer o;
    o<<a<<b<<c<<d<<e;
    return o.asString();
}
template <class T1,class T2,class T3,class T4,class T5,class T6>
REF_getter<refbuffer> outV(const T1& a, const T2& b, const T3& c,const T4& d,const T5& e,const T6& f)
{
    outBuffer o;
    o<<a<<b<<c<<d<<e<<f;
    return o.asString();
}

inline Json::Value jparse(const std::string& buf)
{
    Json::Reader r;
    Json::Value v;
    r.parse(buf,v);
    return v;
}
inline std::string jFastWrite(const Json::Value& j)
{
    Json::FastWriter w;
    return w.write(j);
}

#endif
