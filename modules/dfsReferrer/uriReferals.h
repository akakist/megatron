#ifndef ______________________UEIREFERERALS___H
#define ______________________UEIREFERERALS___H
#include "REF.h"
#include "webDumpable.h"
#include "linkinfo.h"
#include "mutexInspector.h"

namespace dfsReferrer
{
    struct _uriReferals: public Refcountable,public WebDumpable, public Mutexable//, public Broadcaster
    {

        REF_getter<linkInfoDownReferrer> getDownlinkOrNull(const SOCKET_id& id);
        REF_getter<linkInfoDownReferrer> getDownlinkOrCreate(const REF_getter<epoll_socket_info> &esi, const msockaddr_in& sa, const std::set<msockaddr_in>& internalAddrs, const route_t& backRoute, bool *created);

        Json::Value wdump();
        std::string wname() {
            return "_uriReferals";
        }

        _uriReferals()
        {
        }

        std::map<SOCKET_id, REF_getter<linkInfoDownReferrer> > getDownlinks()
        {
            M_LOCK(this);
            return downlinks_mx;
        }
        void clearDownlinks()
        {
            M_LOCK(this);
            downlinks_mx.clear();
            DBG(logErr2("downlinks_mx clear %s",_DMI().c_str()));

        }
        void eraseFromDownlinks(const SOCKET_id & id)
        {
            M_LOCK(this);
            downlinks_mx.erase(id);
            DBG(logErr2("downlinks_mx erase %d %s",CONTAINER(id),_DMI().c_str()));
        }
        size_t countInDownlinks(const SOCKET_id & id)
        {
            M_LOCK(this);
            return downlinks_mx.count(id);

        }
    private:
        std::map<SOCKET_id, REF_getter<linkInfoDownReferrer> > downlinks_mx;

    };

}
#endif
