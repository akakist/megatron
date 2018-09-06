#ifndef ______________UPLINKCONNECTIONSTATE_H
#define ______________UPLINKCONNECTIONSTATE_H

#include "REF.h"
#include "webDumpable.h"
#include "neighbours.h"
#include "epoll_socket_info.h"

namespace dfsReferrer {
    struct _uplinkConnectionState: public Refcountable,public WebDumpable
    {
        Json::Value wdump();
        std::string wname() {
            return "_uplinkConnectionState";
        }

        _uplinkConnectionState(bool isCapsServer,const std::set<msockaddr_in> & _saCaps):
            m_isCapsServer(isCapsServer),
//            getServiceSent(false),
            saCapsFromConfig(_saCaps),
            connectionEstablished(false),
            nodeLevelInHierarhy(0)
        {}
        _uplinkConnectionState(bool isCapsServer):
            m_isCapsServer(isCapsServer),
//            getServiceSent(false),
            connectionEstablished(false),
            nodeLevelInHierarhy(0)
        {

            if(!isCapsServer)
                throw CommonError("if(!isCapsServer)");
        }


        bool m_isCapsServer;



//        bool getServiceSent;
        std::set<msockaddr_in> saCapsFromConfig;


        bool connectionEstablished;
        std::set<msockaddr_in> uplink;
        int nodeLevelInHierarhy;
        std::set<REF_getter<epoll_socket_info> >ponged_for_cleanup_sockets;
        std::map<int64_t/*ping time*/,std::set<std::pair<REF_getter<epoll_socket_info>,msockaddr_in > > > ponged_all;



    };

}
#endif // UPLINKCONNECTIONSTATE_H
