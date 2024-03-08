#include "appHandler.h"
#include <math.h>
#include <megatron_config.h>
#include "tools_mt.h"
#include "colorOutput.h"
#include "__crc64.h"
#include "__crc16.h"
#include "__crc32.h"
#include "ISSL.h"
#include "st_rsa.h"
#include "SQLiteCpp/Database.h"
#include "_QUERY.h"
#include "megatronClient.h"
#include "Events/DFS/Referrer/InitClient.h"
#include "Events/DFS/Referrer/ToplinkDeliver.h"

//const char emitent[]="c4393323dc2cc32a4b9e30aea0b91878a78693d3";/// shuvalow
void AppHandler::init(int argc, char ** argv, const std::string & filesDir, IInstance* instance)
{
//    IUtils *iu=Megatron::initMegatron(argc,argv,filesDir);
//    auto instance=Megatron::createInstance(""
//                                           ""
//                                           "\nRPC.ConnectionActivity=600.000000"
//                                           "\nRPC.IterateTimeoutSec=60.000000"
//                                           "\nRPC.ListenerBuffered.MaxThreadsCount=10"
//                                           "\nStart=RPC,ReferrerClient"
//                                           "\nOscarSecure.ListenerBuffered.MaxThreadsCount=10"
//                                           "\nOscarSecure.RSA_keysize=256"
//                                           "\nOscarSecure.maxPacketSize=33554432"
//                                           "\nRPC.BindAddr_MAIN=INADDR_ANY:0"
//                                           "\nRPC.BindAddr_RESERVE=NONE"
//                                           "\nSocketIO.ListenerBuffered.MaxThreadsCount=10"
//                                           "\nSocketIO.listen_backlog=128"
//                                           "\nSocketIO.maxOutBufferSize=8388608"
//                                           "\nSocketIO.size=1024"
//                                           "\nSocketIO.timeout_millisec=10"
//                                           "\nWebHandler.bindAddr=NONE"
//                                           "\nTelnet.BindAddr=NONE"
//                                           "\nimsfTestService1.ListenerBuffered.MaxThreadsCount=10"
//                                           "\nReferrerClient.T_001_common_connect_failed=20.000000"
//                                           "\nReferrerClient.T_002_D3_caps_get_service_request_is_timed_out=15.000000"
//                                           "\nReferrerClient.T_007_D6_resend_ping_caps_short=7.000000"
//                                           "\nReferrerClient.T_008_D6_resend_ping_caps_long=20.000000"
//                                           "\nReferrerClient.T_009_pong_timed_out_caps_long=40.000000"
//                                           "\nReferrerClient.T_020_D31_wait_after_send_PT_CACHE_on_recvd_from_GetReferrers=2.000000"
//                                           "\nReferrerClient.T_040_D2_cache_pong_timed_out_from_neighbours=2.000000");


    {

        std::set<msockaddr_in> caps;
        caps=instance->getConfig()->get_tcpaddr("CapsIP","x.testquant.com:10100","Root cloud address");

//        msockaddr_in sa;
//        sa.initFromUrl(CLIENT_ADDRESS);
//        caps.insert(sa);
        for(auto& z:caps)
        {
            logErr2("caps %s",z.dump().c_str());
        }
        instance->sendEvent(ServiceEnum::ReferrerClient,new dfsReferrerEvent::InitClient(caps));
    }
//    return instance;

}
AppHandler::AppHandler(IInstance* ins)
    :
    TimerHelper(ins),
    ObjectHandlerThreaded("BlockchainHandler",ins),
//#else
//    ObjectHandlerPolled("BlockchainHandler",ins),

//#endif

    isConnected(false),instance(ins)
{
    MUTEX_INSPECTOR;
    try {
        MUTEX_INSPECTOR;

        iUtils->registerEvent(
            (event_static_constructor) dfsReferrerEvent::UpdateConfigRSP::construct);
        iUtils->registerEvent(
            (event_static_constructor) dfsReferrerEvent::UpdateConfigREQ::construct);


        iUtils->registerEvent(
            (event_static_constructor) ErrorEvent::ErrorRSP::construct);
        iUtils->registerEvent(
            (event_static_constructor) dfsReferrerEvent::ToplinkDeliverRSP::construct);

        sendEvent(ServiceEnum::ReferrerClient,new dfsReferrerEvent::SubscribeNotifications(this));

    } catch(std::exception &e)
    {
        logErr2("BlockchainHandler::BlockchainHandler: %s",e.what());
    }

}




bool AppHandler::on_NotifyReferrerUplinkIsConnected(const dfsReferrerEvent::NotifyReferrerUplinkIsConnected *e)
{
    logErr2(GREEN("@@ %s"),__PRETTY_FUNCTION__);
    XTRY;
    isConnected=true;
    referrer.insert(e->sa);


    return true;
    XPASS;
}


bool AppHandler::on_NotifyReferrerUplinkIsDisconnected(const dfsReferrerEvent::NotifyReferrerUplinkIsDisconnected *e)
{
    MUTEX_INSPECTOR;
    isConnected=false;
    referrer.erase(e->sa);
    return true;

}
bool AppHandler::on_ToplinkDeliverRSP(const dfsReferrerEvent::ToplinkDeliverRSP* e_tl)
{
    MUTEX_INSPECTOR;
    XTRY;
    REF_getter<Event::Base> z=e_tl->getEvent();
    auto &ID=z->id;
    if(ErrorEventEnum::ErrorRSP==ID)
    {
        ErrorEvent::ErrorRSP*er=(ErrorEvent::ErrorRSP*)z.operator->();
        logErr2("Error %s",er->errText.c_str());
        std::string javaCookie=e_tl->route.getLastJavaCookie();

        return true;
    }

    XPASS;
    return false;
}

bool AppHandler::OH_handleObjectEvent(const REF_getter<Event::Base>& e)
{
    MUTEX_INSPECTOR;
    try {

//        logErr2("OH_handleObjectEvent %s",e->dump().toStyledString().c_str());
        auto& ID=e->id;
        if(dfsReferrerEventEnum::UpdateConfigRSP==ID)
        {
            MUTEX_INSPECTOR;
            const dfsReferrerEvent::UpdateConfigRSP*xe=(const dfsReferrerEvent::UpdateConfigRSP*)e.operator ->();
            config_bod=xe->bod;
            return true;
        }
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
            MUTEX_INSPECTOR;
            const rpcEvent::IncomingOnConnector*ze=(const rpcEvent::IncomingOnConnector*)e.operator ->();
            auto& IDC=ze->e->id;
            if(dfsReferrerEventEnum::UpdateConfigRSP==IDC)
            {
                MUTEX_INSPECTOR;
                const dfsReferrerEvent::UpdateConfigRSP*xe=(const dfsReferrerEvent::UpdateConfigRSP*)ze->e.operator ->();
                config_bod=xe->bod;
                return true;
            }


        }

    }
    catch(std::exception &e)
    {
        logErr2("exception %s",e.what());
    }
    catch(...)
    {
        logErr2("exception ...");
    }
    return false;
}
bool AppHandler::on_TickAlarm(const timerEvent::TickAlarm*)
{
    MUTEX_INSPECTOR;
    return false;
}

