#include "corelib/mutexInspector.h"
#include <time.h>
#include <time.h>
#include <map>
#include "testTimerService.h"
#include "___testEvent.h"




#include "Events/System/Net/httpEvent.h"
#include "version_mega.h"
#include "Events/System/timerEvent.h"
#include "refstring.h"



std::set<std::string> samples;
bool testTimer::Service::on_startService(const systemEvent::startService*)
{
    MUTEX_INSPECTOR;

    for(int i=0;i<10;i++)
    {
        std::string ss((char*)&i,sizeof(i));
        sendEvent(ServiceEnum::Timer,new timerEvent::SetAlarm(1,toRef(std::to_string(i)),toRef("any additional data "+std::to_string(i)),0.1*i,ListenerBase::serviceId));
        samples.insert(ss);
    }

    return true;
}


bool testTimer::Service::handleEvent(const REF_getter<Event::Base>& e)
{
    XTRY;
    try {
        MUTEX_INSPECTOR;
        auto& ID=e->id;
        if(systemEventEnum::startService==ID)
            return on_startService((const systemEvent::startService*)e.get());
        if(timerEventEnum::TickAlarm==ID)
        {
            auto E=static_cast<timerEvent::TickAlarm*>(e.get());

            if(E->tid==1) ///
            {
                auto data=E->data->asString();
                auto cookie=E->cookie->asString();
                logErr2("-> %d %s %s",E->tid,data.c_str(),cookie.c_str());
                samples.erase(data);
                if(samples.empty())
                    iUtils->setTerminate();
            }

            /// say engine we handled event
            return true;
        }


    }
    catch(CommonError& e)
    {
        logErr2("testTimer CommonError  %s",e.what());

    }
    catch(std::exception &e)
    {
        logErr2("testTimer std::exception  %s",e.what());
    }
    XPASS;
    /// we not handled event, engine will log it
    return false;
}

testTimer::Service::~Service()
{
}


testTimer::Service::Service(const SERVICE_id& id, const std::string& nm,IInstance* ins):
    UnknownBase(nm),
    ListenerBuffered1Thread(this,nm,ins->getConfig(),id,ins),
    Broadcaster(ins), iInstance(ins)
{


}

void registerTestTimer(const char* pn)
{
    MUTEX_INSPECTOR;

    XTRY;
    if(pn)
    {
        std::set<EVENT_id> es;
        iUtils->registerPlugingInfo(COREVERSION,pn,IUtils::PLUGIN_TYPE_SERVICE,ServiceEnum::testTimer,"testTimer",es);
    }
    else
    {
        iUtils->registerService(COREVERSION,ServiceEnum::testTimer,testTimer::Service::construct,"testTimer");
//        regEvents_prodtestTimer();
    }
    XPASS;
}







