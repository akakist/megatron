#include "UCHandler.h"
#include "tools_mt.h"
#include "Events/System/timer/TickTimer.h"
#include "Events/DFS/Referrer/UpdateConfigRSP.h"
#include "ISSL.h"


UCHandler::UCHandler(IInstance *ins)
    :
    JsonHandler(ins)
{

//    sendEvent(ServiceEnum::Timer,new timerEvent::SetTimer(1,NULL,NULL,10,this));
}

bool UCHandler::handleEventInDerived(const REF_getter<Event::Base> &e)
{
    auto &ID=e->id;
    if(dfsReferrerEventEnum::UpdateConfigRSP==ID)
    {
        const dfsReferrerEvent::UpdateConfigRSP* z=(const dfsReferrerEvent::UpdateConfigRSP*)e.operator ->();
        logErr2("UCHandler: UpdateConfigRSP %s",z->body.c_str());
        return true;
    }
    return false;
}



