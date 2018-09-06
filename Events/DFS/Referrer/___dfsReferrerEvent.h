#ifndef _________dfsReferrerEvent_h123
#define _________dfsReferrerEvent_h123
#include "serviceEnum.h"
#include "eventEnum.h"
#include "SEQ_id.h"
#include "DATA_id.h"
namespace ServiceEnum
{
    const SERVICE_id DFSReferrer("DFSReferrer");
    const SERVICE_id ReferrerClient("RefferrerClient");
}

namespace dfsReferrerEventEnum
{

        const EVENT_id Ping("DFSReferrer.Ping");
        const EVENT_id Hello("DFSReferrer.Hello");
        const EVENT_id SubscribeNotifications("DFSReferrer.SubscribeNotifications");
        const EVENT_id Noop("DFSReferrer.Noop");

        const EVENT_id ToplinkDeliverREQ("DFSReferrer.ToplinkDeliverREQ");
        const EVENT_id ToplinkDeliverRSP("DFSReferrer.ToplinkDeliverRSP");
        const EVENT_id ToplinkDeliverRSP2Node("DFSReferrer.ToplinkDeliverRSP2Node");



        const EVENT_id Pong("DFSReferrer.Pong");
        const EVENT_id Elloh("DFSReferrer.Elloh");
        const EVENT_id NotifyReferrerUplinkIsConnected("DFSReferrer.NotifyReferrerUplinkIsConnected");
        const EVENT_id NotifyReferrerUplinkIsDisconnected("DFSReferrer.NotifyReferrerUplinkIsDisconnected");
        const EVENT_id NotifyReferrerDownlinkDisconnected("DFSReferrer.NotifyReferrerDownlinkDisconnected");
        const EVENT_id InitClient("DFSReferrer.InitClient");

        const EVENT_id UpdateConfigREQ("DFSReferrer.UpdateConfigREQ");
        const EVENT_id UpdateConfigRSP("DFSReferrer.UpdateConfigRSP");

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
/// if SOCKET_id==0 then socket is local storage


#endif
