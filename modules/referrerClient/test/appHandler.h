#ifndef ____DS_HANDLER__________h
#define ____DS_HANDLER__________h
#include "configDB.h"



#include "Events/DFS/Referrer/NotifyReferrer.h"
#include "Events/DFS/Referrer/SubscribeNotifications.h"
#include "Events/DFS/Referrer/ToplinkDeliver.h"
#include "Events/DFS/Referrer/UpdateConfig.h"
#include "Events/System/Net/rpc/IncomingOnAcceptor.h"
#include "Events/System/Net/rpc/IncomingOnConnector.h"
#include "Events/System/timer/TickAlarm.h"
#include "objectHandler.h"
#include "../Events/System/Error/Error.h"
#include "timerHelper.h"
#include "SQLiteCpp/Transaction.h"

#define HANDLER_DB "h_db"

class AppHandler:
    public TimerHelper,
    public
//#ifdef __MOBILE__
    ObjectHandlerThreaded
//#else
//    ObjectHandlerPolled
//#endif


{
public:
    AppHandler(IInstance *ins);
    IInstance *instance;

    bool OH_handleObjectEvent(const REF_getter<Event::Base>& e);



    bool on_NotifyReferrerUplinkIsConnected(const dfsReferrerEvent::NotifyReferrerUplinkIsConnected *);
    bool on_NotifyReferrerUplinkIsDisconnected(const dfsReferrerEvent::NotifyReferrerUplinkIsDisconnected *);
    bool on_ToplinkDeliverRSP(const dfsReferrerEvent::ToplinkDeliverRSP* e);
    bool on_TickAlarm(const timerEvent::TickAlarm*e);





    /// ACCOUNTS





    std::set<msockaddr_in> referrer;
    bool isConnected;
    std::string config_bod;








    static void init(int argc, char **argv, const std::string &filesDir, IInstance *instance);











};
#endif
