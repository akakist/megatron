#ifndef _________dfsReferrerEvent_h123
#define _________dfsReferrerEvent_h123
#include "serviceEnum.h"
#include "eventEnum.h"
#include "SEQ_id.h"
#include "DATA_id.h"
namespace ServiceEnum
{
    const SERVICE_id DFSReferrer("_xs_DFSReferrer");
    const SERVICE_id ReferrerClient("RefferrerClient");
}

namespace dfsReferrerEventEnum
{

        const EVENT_id Ping("_xe_dfsReferrerPing");
        const EVENT_id Hello("_xe_dfsReferrerHello");
        const EVENT_id SubscribeNotifications("_xe_subscribereferrerNotifyReferrerUplinkIsConnected");
        const EVENT_id Noop("_xe_refferrerNoop");

        const EVENT_id ToplinkDeliverREQ("_xe_ToplinkDeliverREQ");
        const EVENT_id ToplinkDeliverRSP("_xe_ToplinkDeliverRSP");

        const EVENT_id ToplinkBroadcastByBackroute("ToplinkBroadcastByBackroute");


        const EVENT_id NotifyDownlink("NotifyDownlink");

        const EVENT_id Pong("_xe_dfsReferrerPong");
        const EVENT_id Elloh("_xe_dfsReferrerElloh");
        const EVENT_id NotifyReferrerUplinkIsConnected("_xe_referrerNotifyReferrerUplinkIsConnected");
        const EVENT_id NotifyReferrerUplinkIsDisconnected("_xe_referrerNotifyReferrerUplinkIsDisconnected");
        const EVENT_id NotifyReferrerDownlinkDisconnected("_xs_NotifyReferrerDownlinkDisconnected");
        const EVENT_id NotifyReferrerDownlinkConnected("_xs_NotifyReferrerDownlinkConnected");
        const EVENT_id InitClient("InitClient:ReferrerClient");

        const EVENT_id UpdateConfigREQ("UpdateConfigREQ:DFSReferrer");
        const EVENT_id UpdateConfigRSP("UpdateConfigRSP:DFSReferrer");

}
namespace dfsReferrer {


    namespace PingType {
        enum
        {
            PT_CACHED,
            PT_MASTER_SHORT,
            PT_MASTER_LONG,
            PT_CAPS_SHORT,
            PT_CAPS_LONG,
        };
    }

}
/// если SOCKET_id==0 значит это файлы с локального стоража.


#endif
