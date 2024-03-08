#include "corelib/mutexInspector.h"
#include <time.h>
#include <time.h>

#include "tools_mt.h"
#include "st_FILE.h"

#include <map>
#include "js_utils.h"
#include "configDB.h"
#include "prodtestServerService.h"
#include "../Event/Ping.h"
#include "events_prodtestServerService.hpp"

#include "st_malloc.h"


bool prodtestServer::Service::on_startService(const systemEvent::startService*)
{
    MUTEX_INSPECTOR;
    try {

    }  catch (CommonError& e) {

        logErr2("catch %s",e.what());
    }

    return true;
}


bool prodtestServer::Service::handleEvent(const REF_getter<Event::Base>& e)
{
    XTRY;
    try {
        MUTEX_INSPECTOR;
        auto& ID=e->id;
        if(timerEventEnum::TickTimer==ID)
        {
            const timerEvent::TickTimer*ev=static_cast<const timerEvent::TickTimer*>(e.operator ->());
            return true;
        }
        if(systemEventEnum::startService==ID)
            return on_startService((const systemEvent::startService*)e.operator->());

        if(prodtestEventEnum::AddTaskREQ==ID)
            return on_AddTaskREQ((const prodtestEvent::AddTaskREQ*)e.operator->());


        if(rpcEventEnum::IncomingOnAcceptor==ID)
        {
            auto E=(rpcEvent::IncomingOnAcceptor*)e.operator->();
            auto &IDA=E->e->id;
            if(prodtestEventEnum::AddTaskREQ==IDA)
                return on_AddTaskREQ((const prodtestEvent::AddTaskREQ*)E->e.operator->());
        }

    } catch(std::exception &e)
    {
        logErr2("BlockchainTop std::exception  %s",e.what());
    }
    XPASS;
    return false;
}



prodtestServer::Service::~Service()
{
}


prodtestServer::Service::Service(const SERVICE_id& id, const std::string& nm,IInstance* ins):
    UnknownBase(nm),
    ListenerBuffered1Thread(this,nm,ins->getConfig(),id,ins),
    Broadcaster(ins)
{
}

void registerprodtestServerService(const char* pn)
{
    MUTEX_INSPECTOR;

    XTRY;
    if(pn)
    {
        iUtils->registerPlugingInfo(COREVERSION,pn,IUtils::PLUGIN_TYPE_SERVICE,ServiceEnum::prodtestServer,"prodtestServer",getEvents_prodtestServerService());
    }
    else
    {
        iUtils->registerService(COREVERSION,ServiceEnum::prodtestServer,prodtestServer::Service::construct,"prodtestServer");
        regEvents_prodtestServerService();
    }
    XPASS;
}

int cnt=0;


bool prodtestServer::Service::on_AddTaskREQ(const prodtestEvent::AddTaskREQ* e)
{
    cnt++;
    std::string xored=e->sampleString+"123";


    passEvent(new prodtestEvent::AddTaskRSP(e->session,xored,e->count,poppedFrontRoute(e->route)));
    return true;
}

