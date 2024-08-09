#pragma once

#include "REF.h"
#include "webDumpable.h"
#include "neighbours.h"
#include "epoll_socket_info.h"

namespace dfsReferrer {
    struct _uplinkConnectionState: public Refcountable
#ifdef WEBDUMP
        ,public WebDumpable
#endif
    {
        Json::Value wdump();
        std::string wname() {
            return "_uplinkConnectionState";
        }

        _uplinkConnectionState(bool isTopServer,const std::set<msockaddr_in> & _saCaps):
            m_isTopServer(isTopServer),
//            getServiceSent(false),
            saCapsFromConfig(_saCaps),
            connectionEstablished(false),
            nodeLevelInHierarhy(0)
        {}
        _uplinkConnectionState(bool isTopServer):
            m_isTopServer(isTopServer),
//            getServiceSent(false),
            connectionEstablished(false),
            nodeLevelInHierarhy(0)
        {

            if(!isTopServer)
                throw CommonError("if(!isTopServer)");
        }


        bool m_isTopServer;



//        bool getServiceSent;
        std::set<msockaddr_in> saCapsFromConfig;


        bool connectionEstablished;
        std::set<msockaddr_in> uplink;
        int nodeLevelInHierarhy;
        std::set<REF_getter<epoll_socket_info> >ponged_for_cleanup_sockets;
        std::map<int64_t/*ping time*/,std::set<std::pair<REF_getter<epoll_socket_info>,msockaddr_in > > > ponged_all;



    };

}
