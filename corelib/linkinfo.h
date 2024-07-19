#pragma once
#include <string>
#include "epoll_socket_info.h"

#include "webDumpable.h"
#include "IUtils.h"
struct linkInfoDownReferrer: public Refcountable
#ifdef WEBDUMP
    ,public WebDumpable
#endif
    , public Mutexable
{
    REF_getter<epoll_socket_info> esi_mx_;
    msockaddr_in externalAddr_mx_;
    std::set<msockaddr_in> internalAddrs_mx;
    route_t backRoute_mx_;
    time_t lastReplaceByDownlink_mx_;

    linkInfoDownReferrer(const REF_getter<epoll_socket_info> &_esi,const msockaddr_in& _externalAddr, const std::set<msockaddr_in> &_internalAddrs, const route_t &_backRoute)
        :esi_mx_(_esi),externalAddr_mx_(_externalAddr), internalAddrs_mx(_internalAddrs),backRoute_mx_(_backRoute), lastReplaceByDownlink_mx_(time(NULL))
    {}
    Json::Value wdump()
    {
        M_LOCK(this);
        Json::Value v;
        v["externalAddr"]=externalAddr_mx_.dump();
        v["internalAddrs"]=iUtils->dump(internalAddrs_mx);
        return v;
    }
    std::string wname() {
        return "linkInfoDown";
    }


};
