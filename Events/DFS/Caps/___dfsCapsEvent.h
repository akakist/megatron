#ifndef _________dfsCapsEvent_h
#define _________dfsCapsEvent_h

#include <string>
#include "SERVICE_id.h"
#include "EVENT_id.h"
namespace ServiceEnum
{
    const SERVICE_id DFSCaps("DFSCaps");
}

namespace dfsCapsEventEnum
{
    const EVENT_id RegisterMyRefferrerNodeREQ("DFSCaps.RegisterMyRefferrerNodeREQ");
    const EVENT_id RegisterCloudRoot("DFSCaps.RegisterCloudRoot");
    const EVENT_id GetReferrersREQ("DFSCaps.GetReferrersREQ");
    const EVENT_id GetReferrersRSP("DFSCaps.GetReferrersRSP");
    const EVENT_id GetCloudRootsREQ("DFSCaps.GetCloudRootsREQ");
    const EVENT_id GetCloudRootsRSP("DFSCaps.GetCloudRootsRSP");
}
#endif
