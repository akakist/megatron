#ifndef ___________RPC__SERVER__H_OUT_PROXY__ZZ
#define ___________RPC__SERVER__H_OUT_PROXY__ZZ
#include "broadcaster.h"
#include "REF.h"




#include "listenerBuffered.h"


#include "objectHandler.h"



#include "tools_mt.h"
#include "ISSL.h"

#include <map>
#include "listenerBuffered1Thread.h"
#include "logging.h"
#include "_bitStream.h"
#include <algorithm>
#include "Events/System/Run/startService.h"
#include "Events/Tools/webHandler/RequestIncoming.h"
#include "Events/System/timer/TickAlarm.h"
#include "Events/DFS/Caps/GetRefferrers.h"
#include "Events/DFS/Caps/RegisterMyRefferrer.h"
#include "Events/DFS/Referrer/ToplinkDeliver.h"
#include "Events/System/Net/rpc/IncomingOnAcceptor.h"
#include "Events/System/Net/rpc/IncomingOnConnector.h"

#include "timerHelper.h"
struct ServiceNode;

#define GET_SERVICE_NODE_LIMIT 50
namespace dfsCaps
{


    class Service:
        private UnknownBase,
        private ListenerBuffered1Thread,
        private Broadcaster,
        public Mutexable,
        public Logging,
        public TimerHelper
    {
        bool handleEvent(const REF_getter<Event::Base>& e);
        bool on_startService(const systemEvent::startService*);
        real Distance(const std::pair<real,real> & c1,const std::pair<real,real> & c2);
        bool on_ToplinkDeliverREQ(const dfsReferrerEvent::ToplinkDeliverREQ* e);
        bool on_TickAlarm(const timerEvent::TickAlarm*);
        bool on_RequestIncoming(const webHandlerEvent::RequestIncoming*);

        int pack(outBitStream& o,int val,int maxval);


        IInstance* iInstance;


        NetTreeRoute calcTreeRoute(const int32_t &ip);
        std::string m_GeoLiteCity_Location;
        std::string m_GeoLiteCity_Blocks;
        std::string m_pn_netBlocks;
        bool m_createNetBlocks;
        NetTreeRouteGenerator netgen;

    public:
        Service(const SERVICE_id &svs, const std::string&  nm,IInstance* _ifa);
        static UnknownBase* construct(const SERVICE_id& id, const std::string&  nm, IInstance* _ifa);
        ~Service();

        std::map<int,NetTreeNode> rootTreeNode;
    };
}


#endif
