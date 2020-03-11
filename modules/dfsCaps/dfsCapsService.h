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
#include "Events/DFS/Caps/GetCloudRoots.h"
#include "Events/DFS/Caps/RegisterMyRefferrerNode.h"
#include "Events/DFS/Caps/RegisterCloudRoot.h"
#include "Events/DFS/Referrer/ToplinkDeliver.h"
#include "Events/DFS/Referrer/ToplinkDeliver.h"
#include "Events/System/Net/rpc/IncomingOnAcceptor.h"
#include "Events/System/Net/rpc/IncomingOnConnector.h"
#include "DBH.h"
#include "CapsAlgorithm.h"
#include "timerHelper.h"
struct ServiceNode;

#define GET_SERVICE_NODE_LIMIT 50



namespace dfsCaps
{


    class Service:
        public UnknownBase,
        public ListenerBuffered1Thread,
        public Broadcaster,
        public Mutexable,
        public TimerHelper
    {
        bool handleEvent(const REF_getter<Event::Base>& e);
        bool on_startService(const systemEvent::startService*);
        real Distance(const std::pair<real,real> & c1,const std::pair<real,real> & c2);
        bool on_ToplinkDeliverREQ(const dfsReferrerEvent::ToplinkDeliverREQ* e);
        bool on_TickAlarm(const timerEvent::TickAlarm*);
        bool on_RequestIncoming(const webHandlerEvent::RequestIncoming*);

        bool on_GetReferrersREQ(const dfsCapsEvent::GetReferrersREQ* e, const dfsReferrerEvent::ToplinkDeliverREQ *e_toplink);
        bool on_RegisterMyRefferrerNodeREQ(const dfsCapsEvent::RegisterMyRefferrerNodeREQ* e);

        bool on_GetCloudRootsREQ(const dfsCapsEvent::GetCloudRootsREQ* e, const dfsReferrerEvent::ToplinkDeliverREQ *e_toplink);
        bool on_RegisterCloudRoot(const dfsCapsEvent::RegisterCloudRoot* e);

        IInstance* iInstance;

        CapsAlgorithm referrer_nodes;
        CapsAlgorithm cloud_roots;
    public:
        void deinit()
        {
            ListenerBuffered1Thread::denit();
        }

        Service(const SERVICE_id &svs, const std::string&  nm,IInstance* _ifa);
        static UnknownBase* construct(const SERVICE_id& id, const std::string&  nm, IInstance* _ifa);
        ~Service();

//        std::string dbname;
    };
}


#endif
