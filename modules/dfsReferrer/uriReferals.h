#ifndef ______________________UEIREFERERALS___H
#define ______________________UEIREFERERALS___H
#include "REF.h"
#include "webDumpable.h"
#include "linkinfo.h"
#include "mutexInspector.h"

namespace dfsReferrer
{
    struct _uriReferals: public Refcountable
#ifdef WEBDUMP
            ,public WebDumpable
#endif
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
            return m_downlinks;
        }
        void clearDownlinks()
        {
            m_downlinks.clear();
            DBG(logErr2("downlinks_mx clear %s",_DMI().c_str()));

        }
        void eraseFromDownlinks(const SOCKET_id & id)
        {
            m_downlinks.erase(id);
//            DBG(logErr2("downlinks_mx erase %d %s",CONTAINER(id),_DMI().c_str()));
        }
        size_t countInDownlinks(const SOCKET_id & id)
        {
            return m_downlinks.count(id);

        }
        size_t sizeOfDownlinks()
        {
            return m_downlinks.size();

        }

    private:
        std::map<SOCKET_id, REF_getter<linkInfoDownReferrer> > m_downlinks;

    };

}
#endif
