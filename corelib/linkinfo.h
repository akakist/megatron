#ifndef ________________LINKINFO_H
#define ________________LINKINFO_H
#include <string>
#include "epoll_socket_info.h"

#include "webDumpable.h"
#include "IUtils.h"
namespace dfsReferrer
{
    struct linkInfoDownReferrer: public Refcountable,public WebDumpable, public Mutexable
    {
        REF_getter<epoll_socket_info> esi_mx;
        msockaddr_in externalAddr_mx;
        std::set<msockaddr_in> internalAddrs_mx;
        route_t backRoute_mx;
        time_t lastReplaceByDownlink_mx;

        linkInfoDownReferrer(const REF_getter<epoll_socket_info> &_esi,const msockaddr_in& _externalAddr, const std::set<msockaddr_in> &_internalAddrs, const route_t &_backRoute)
            :esi_mx(_esi),externalAddr_mx(_externalAddr), internalAddrs_mx(_internalAddrs),backRoute_mx(_backRoute), lastReplaceByDownlink_mx(time(NULL))
        {}
        Json::Value wdump()
        {
            M_LOCK(this);
            Json::Value v;
            //v["_linkInfo"]=_linkInfo::wdump();
            v["externalAddr"]=externalAddr_mx.dump();
            v["internalAddrs"]=iUtils->dump(internalAddrs_mx);
            return v;
        }
        std::string wname() {
            return "linkInfoDown";
        }


    };

}
#endif // LINKINFO_H
