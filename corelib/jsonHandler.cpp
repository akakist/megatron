#include "jsonHandler.h"
#include "mutexInspector.h"
#ifdef __ANDROID__
#include "jni.h"
#endif
#include "Events/System/timer/TickAlarm.h"
#include "Events/DFS/Referrer/SubscribeNotifications.h"
#include "Events/System/Net/rpc/IncomingOnConnector.h"
#include "Events/System/Net/rpc/IncomingOnAcceptor.h"
#include "Events/DFS/Referrer/UpdateConfig.h"
#include "Events/DFS/Referrer/UpdateConfig.h"
//void registerReferrerClientService(const char* pn);

JsonHandler::JsonHandler(IInstance *ins):
    TimerHelper(ins),
#ifdef __MOBILE__
    ObjectHandlerThreaded("JsonHandler",ins),
#else
    ObjectHandlerPolled("JsonHandler",ins),

#endif
    isConnected(false)
{
    //setTimerValue(T_START_CONNECT,0.5);

    //setAlarm(T_START_CONNECT,NULL,NULL,this);
    //  registerReferrerClientService(NULL);
    iUtils->registerEvent(jsonRefEvent::JsonREQ::construct);
    iUtils->registerEvent(jsonRefEvent::JsonRSP::construct);
    iUtils->registerEvent(dfsReferrerEvent::UpdateConfigRSP::construct);
    iUtils->registerEvent(dfsReferrerEvent::UpdateConfigREQ::construct);

    sendEvent(ServiceEnum::ReferrerClient,new dfsReferrerEvent::SubscribeNotifications(this));
}

bool JsonHandler::on_NotifyReferrerUplinkIsConnected(const dfsReferrerEvent::NotifyReferrerUplinkIsConnected *)
{
    //logErr2("on_NotifyReferrerUplinkIsConnected");
    MUTEX_INSPECTOR;
    isConnected=true;
    signal_connected();
    return true;
}
bool JsonHandler::on_NotifyReferrerUplinkIsDisconnected(const dfsReferrerEvent::NotifyReferrerUplinkIsDisconnected *)
{
    MUTEX_INSPECTOR;
    isConnected=false;
    signal_disconnected();
    return true;

}
bool JsonHandler::on_ToplinkDeliverRSP(const dfsReferrerEvent::ToplinkDeliverRSP* e)
{
    XTRY;
    MUTEX_INSPECTOR;
    REF_getter<Event::Base> z=e->getEvent();
    passEvent(z);
    XPASS;
    return true;
}
bool JsonHandler::on_TickAlarm(const timerEvent::TickAlarm*e)
{
    return false;
}

bool JsonHandler::OH_handleObjectEvent(const REF_getter<Event::Base>& e)
{
    XTRY;
    MUTEX_INSPECTOR;
    try {
        if(handleEventInDerived(e))
            return true;
        auto& ID=e->id;
        if(jsonRefEventEnum::JsonRSP==ID)
            return on_JsonRSP((const jsonRefEvent::JsonRSP*) e.operator ->());
        if( dfsReferrerEventEnum::ToplinkDeliverRSP==ID)
            return on_ToplinkDeliverRSP((const dfsReferrerEvent::ToplinkDeliverRSP*)e.operator ->());
        if( dfsReferrerEventEnum::NotifyReferrerUplinkIsConnected==ID)
            return on_NotifyReferrerUplinkIsConnected((const dfsReferrerEvent::NotifyReferrerUplinkIsConnected*)e.operator ->());
        if( dfsReferrerEventEnum::NotifyReferrerUplinkIsDisconnected==ID)
            return on_NotifyReferrerUplinkIsDisconnected((const dfsReferrerEvent::NotifyReferrerUplinkIsDisconnected*)e.operator ->());

        if(timerEventEnum::TickAlarm==ID)
            return on_TickAlarm((const timerEvent::TickAlarm*)e.operator ->());

        if(rpcEventEnum::IncomingOnConnector==ID)
        {
            const rpcEvent::IncomingOnConnector*ze=(const rpcEvent::IncomingOnConnector*)e.operator ->();
            auto& IDC=ze->e->id;
            if(dfsReferrerEventEnum::UpdateConfigRSP==IDC)
            {
                const dfsReferrerEvent::UpdateConfigRSP*xe=(const dfsReferrerEvent::UpdateConfigRSP*)ze->e.operator ->();
                config_body=xe->body;
                logErr2("JsonHandler: if(dfsReferrerEventEnum::UpdateConfigRSP==IDC) %s",config_body.c_str());
                return true;
            }

        }

    }
    catch(std::exception &e)
    {

        logErr2("exception %s",e.what());
        JAVACOOKIE_id c;
        CONTAINER(c)="BROADCAST";
        push_err("Common Error",e.what(),c);
    }
    catch(...)
    {

        JAVACOOKIE_id c;
        CONTAINER(c)="BROADCAST";
        logErr2("exception ...");
        push_err("Unknown error","Unknown exception",c);
    }
    XPASS;
    return false;
}







void JsonHandler::push_msg(const Json::Value &s,const std::string& binData, const JAVACOOKIE_id &javaCookie)
{

    M_LOCK(mx);
    Json::Value j;
    j["data"]=s;
    j["errcode"]="0";
    j["javaCookie"]=CONTAINER(javaCookie);
    //logErr2("JsonHandler::push_msg %s",j.toStyledString().c_str());
#ifdef __MOBILE__
    mx.java_msgs.push_back(std::make_pair(j.toStyledString(),binData));
#else
    //logErr2("msg: %s",j.toStyledString().c_str());
#endif

}
void JsonHandler::push_err(const std::string & action,const std::string& s, const JAVACOOKIE_id& javaCookie)
{

    M_LOCK(mx);
    Json::Value j;
    j["data"]=s;
    j["action"]=action;
    j["errcode"]="1";
    j["javaCookie"]=CONTAINER(javaCookie);
#ifdef __MOBILE__
    mx.java_msgs.push_back(std::make_pair(j.toStyledString(),""));
#else
    //logErr2("msg: %s",s.c_str());
#endif

}
#ifdef __MOBILE__
std::pair<std::string,std::string> JsonHandler::getMessage()
{
    M_LOCK(mx);
    if(mx.java_msgs.size())
    {
        auto msg=*mx.java_msgs.begin();
        mx.java_msgs.pop_front();
        return msg;
    }
    return std::make_pair("","");
}

#endif



/// если емейл еще не зареген, то он вставляется в базу в привязке к UID, в ответе прилетает uid/cookie,
/// если зареген, то в ответе прилетает новая uid/cookie.

