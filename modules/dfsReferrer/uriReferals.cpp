#include "uriReferals.h"
#include "mutexInspector.h"

Json::Value dfsReferrer::_uriReferals::wdump()
{
    Json::Value v;
    return v;
}
REF_getter<dfsReferrer::linkInfoDownReferrer> dfsReferrer::_uriReferals::getDownlinkOrNull(const SOCKET_id& id)
{
    std::map<SOCKET_id, REF_getter<linkInfoDownReferrer> > ::iterator i=m_downlinks.find(id);
    if(i!=m_downlinks.end()) return i->second;
    return NULL;
}
REF_getter<dfsReferrer::linkInfoDownReferrer> dfsReferrer::_uriReferals::getDownlinkOrCreate(const REF_getter<epoll_socket_info>& esi, const msockaddr_in& externalAddr,const std::set<msockaddr_in>& internalAddrs, const route_t& backRoute, bool* created)
{
    if(!esi.valid())
        throw CommonError("if(!esi.valid())");

    REF_getter<linkInfoDownReferrer> l(NULL);
    {
        std::map<SOCKET_id, REF_getter<linkInfoDownReferrer> > ::iterator i=m_downlinks.find(esi->m_id);
        if(i!=m_downlinks.end())
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
            m_downlinks.insert(std::make_pair(esi->m_id,l));
        }
        *created=true;
        return l;

    }
    return NULL;
}
