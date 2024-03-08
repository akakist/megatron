#ifndef _________dfsReferrerEvent_h123Z1
#define _________dfsReferrerEvent_h123Z1
#include "serviceEnum.h"

#include "SEQ_id.h"
#include "DATA_id.h"
namespace ServiceEnum
{
    const SERVICE_id prodtestServer("prodtestServer");
    const SERVICE_id prodtestServerWeb("prodtestServerWeb");
}

namespace prodtestEventEnum
{

    const EVENT_id AddTaskREQ("pt.AddTaskREQ");
    const EVENT_id AddTaskRSP("pt.AddTaskRSP");

}


#endif
