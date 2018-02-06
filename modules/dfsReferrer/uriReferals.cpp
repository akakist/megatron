#include "uriReferals.h"
#include "mutexInspector.h"

Json::Value dfsReferrer::_uriReferals::wdump()
{
    Json::Value v;
    return v;
}
REF_getter<dfsReferrer::linkInfoDownReferrer> dfsReferrer::_uriReferals::getDownlinkOrNull(const SOCKET_id& id)
{
    M_LOCK(this);
    std::map<SOCKET_id, REF_getter<linkInfoDownReferrer> > ::iterator i=downlinks_mx.find(id);
    if(i!=downlinks_mx.end()) return i->second;
    return NULL;
}
REF_getter<dfsReferrer::linkInfoDownReferrer> dfsReferrer::_uriReferals::getDownlinkOrCreate(const REF_getter<epoll_socket_info>& esi, const msockaddr_in& externalAddr,const std::set<msockaddr_in>& internalAddrs, const route_t& backRoute, bool* created)
{
    if(!esi.valid())
        throw CommonError("if(!esi.valid())");

    REF_getter<linkInfoDownReferrer> l(NULL);
    {
        M_LOCK(this);
        std::map<SOCKET_id, REF_getter<linkInfoDownReferrer> > ::iterator i=downlinks_mx.find(esi->m_id);
        if(i!=downlinks_mx.end())
        {
            l=i->second;
        }
    }
    if(l.valid())
    {

        M_LOCK(l.operator->());
        if(!(l->backRoute_mx==backRoute) || l->externalAddr_mx!=externalAddr || l->internalAddrs_mx!=internalAddrs)
        {
            *created=true;
        }
        else
        {
            *created=false;
        }
        l->backRoute_mx=backRoute;
        l->externalAddr_mx=externalAddr;
        l->internalAddrs_mx=internalAddrs;
        return l;

    }
    else
    {
        l=new linkInfoDownReferrer(esi,externalAddr,internalAddrs,backRoute);
        {
            M_LOCK(this);
            downlinks_mx.insert(std::make_pair(esi->m_id,l));
        }
        *created=true;
        return l;

    }
    return NULL;
}
