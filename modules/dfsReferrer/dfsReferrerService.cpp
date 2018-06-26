#include "dfsReferrerService.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#ifndef _WIN32
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#endif
#include "IInstance.h"
#include "megatron_config.h"


#include "route_t.h"
#include "CHUNK_id.h"

#include "IRPC.h"
#include "mutexInspector.h"


#include "tools_mt.h"
#include "version_mega.h"
#include "VERSION_id.h"



#include "tools_mt.h"
#include "unknownCastDef.h"

#include "st_rsa.h"

//#ifndef __ANDROID__
//#include "sqlite3Wrapper.h"
//#endif
#include "configDB.h"

#include "Events/Tools/webHandler/RequestIncoming.h"
#include "Events/Tools/webHandler/RegisterDirectory.h"
#include "Events/Tools/webHandler/RegisterHandler.h"
#include "Events/Tools/telnet/RegisterCommand.h"
#include "Events/Tools/telnet/Reply.h"
#include "Events/System/Net/rpc/SubscribeNotifications.h"
#include "Events/System/Net/http/GetBindPorts.h"


#include "events_dfsReferrer.hpp"

using namespace dfsReferrer;
using namespace dfsReferrerEvent;



Service::Service(const SERVICE_id &svs, const std::string&  nm, IInstance* ifa):
    UnknownBase(nm),
    ListenerBuffered1Thread(this,nm,ifa->getConfig(),svs,ifa),
    Broadcaster(ifa),
    Logging(this,
#ifdef DEBUG
            TRACE_log
#else
            ERROR_log
#endif
    ,ifa   ),TimerHelper(ifa),

    uplinkConnectionState(NULL),
    urirefs(new _uriReferals()),
    d2_start_time(0),
#if !defined(WITHOUT_UPNP)
    m_upnpExecuted(false),
    m_upnpInRequesting(false),
 #endif
    externalAccessIsPossible(false),
    iInstance(ifa),
    connection_sequence_id(0)


{

    MUTEX_INSPECTOR;
    try
    {
        
        XTRY;

        bool iscaps=ifa->getConfig()->get_bool("IsCapsServer",false,
                                               "Is caps mode. Do not forward caps requests to uplink");
        if(!iscaps)
        {
            std::vector<std::string> hline;
            {
                for(int j=0; j<5; j++)
                {
                    char s[100];
                    int port=atoi(CAPS_PORT);
                    snprintf(s,sizeof(s),CAPS_IP ":%d",port+j);
                    hline.push_back(s);
                }
            }
            uplinkConnectionState=new _uplinkConnectionState(
                false,
                ifa->getConfig()->get_tcpaddr("CapsServerUrl",iUtils->join(",",hline),""));
        }
        else
        {
            uplinkConnectionState=new _uplinkConnectionState(true);
        }







        IConfigObj* config=ifa->getConfig();
        setTimerValue(refTimer::T_001_common_connect_failed,config->get_real("T_001_common_connect_failed", 20,""));
        setTimerValue(refTimer::T_002_D3_caps_get_service_request_is_timed_out,config->get_real("T_002_D3_caps_get_service_request_is_timed_out", 15.0,""));
        //setTimerValue(refTimer::T_003_master_short_pong_timed_out,config->get_real("T_003_master_short_pong_timed_out", 8.0,""));
        //setTimerValue(refTimer::T_004_master_long_pong_timed_out,config->get_real("T_004_master_long_pong_timed_out", 30.0,""));
        //setTimerValue(refTimer::T_005_master_long_resend_ping_timeout,config->get_real("T_005_master_long_resend_ping_timeout", 10.0,""));
        //setTimerValue(refTimer::T_006_cleanup_master_not_allowed,config->get_real("T_006_cleanup_master_not_allowed_", 15.0,""));
        setTimerValue(refTimer::T_007_D6_resend_ping_caps_short,config->get_real("T_007_D6_resend_ping_caps_short", 7.0,""));
        setTimerValue(refTimer::T_008_D6_resend_ping_caps_long,config->get_real("T_008_D6_resend_ping_caps_long", 20.0,""));
        setTimerValue(refTimer::T_009_pong_timed_out_caps_long,config->get_real("T_009_pong_timed_out_caps_long", 40.0,""));
        //setTimerValue(refTimer::T_010_request_cap_for_service_timed_out,config->get_real("T_010_request_cap_for_service_timed_out", 2, ""));
        setTimerValue(refTimer::T_011_downlink_ping_timed_out,config->get_real("T_011_downlink_ping_timed_out", 60, ""));
        setTimerValue(refTimer::T_012_reregister_referrer,config->get_real("T_012_reregister_referrer", 3500, ""));
        setTimerValue(refTimer::T_019_D5_external_connection_check_timeout,config->get_real("T_019_D5_external_connection_check_timeout", 7, ""));
        setTimerValue(refTimer::T_020_D31_wait_after_send_PT_CACHE_on_recvd_from_GetReferrers,config->get_real("T_020_D3_1_wait_after_send_PT_CACHE_on_recvd_from_GetService", 2.0,""));
        setTimerValue(refTimer::T_040_D2_cache_pong_timed_out_from_neighbours,config->get_real("T_004_cache_pong_timed_out_", 2,""));


        ConfigDB_private c;
        config_body=c.get_string("refferer_config","");



        XPASS;
    }
    catch(std::exception &e)
    {
        logErr2("exception: %s %s %d",e.what(),__FILE__,__LINE__);
        throw;
    }
}
bool  Service::on_startService(const systemEvent::startService* )
{
    MUTEX_INSPECTOR;

    S_LOG("on_startService");

    sendEvent(ServiceEnum::RPC,new rpcEvent::SubscribeNotifications(ListenerBase::serviceId));
    if(iUtils->isServiceRegistered(ServiceEnum::Telnet))
    {
        sendEvent(ServiceEnum::Telnet, new telnetEvent::RegisterCommand("services/referrer", "dumpFiles", "show files info","", ListenerBase::serviceId));
        sendEvent(ServiceEnum::Telnet, new telnetEvent::RegisterCommand("services/referrer", "dumpUsers", "show users info","", ListenerBase::serviceId));
        sendEvent(ServiceEnum::Telnet, new telnetEvent::RegisterCommand("services/referrer", "dumpPipes", "show pipes info","", ListenerBase::serviceId));
    }


    if(iUtils->isServiceRegistered(ServiceEnum::WebHandler))
    {
        sendEvent(ServiceEnum::WebHandler, new webHandlerEvent::RegisterDirectory("dfs","DFS"));
        sendEvent(ServiceEnum::WebHandler, new webHandlerEvent::RegisterHandler("dfs/referrer","Referrer",ListenerBase::serviceId));
    }



    if(!uplinkConnectionState->m_isCapsServer)
    {
        STAGE_D2_PING_NEIGHBOURS_start();
        return true;
    }
    return true;
}

Service::~Service()
{
}



UnknownBase* Service::construct(const SERVICE_id& id, const std::string&  nm,IInstance* ifa)
{
    return new Service(id,nm,ifa);
}

void registerDFSReferrerService(const char* pn)
{
    XTRY;
    if(pn)
    {
        iUtils->registerPlugingInfo(COREVERSION,pn,IUtils::PLUGIN_TYPE_SERVICE,ServiceEnum::DFSReferrer,"DFSReferrer");
    }
    else
    {
        iUtils->registerService(COREVERSION,ServiceEnum::DFSReferrer,Service::construct,"DFSReferrer");
        regEvents_dfsReferrer();
    }
    XPASS;

}


#if !defined(WITHOUT_UPNP)
bool Service::on_UpnpResult(const rpcEvent::UpnpResult*e)
{
    MUTEX_INSPECTOR;
    S_LOG("on_UpnpResult");
    m_upnpExecuted=true;
    m_upnpInRequesting=false;
    inBuffer in(e->cookie);
    std::string cmd=in.get_PSTR();
    DBG(log(TRACE_log,"upnp result %s",cmd.c_str()));

    if(cmd=="d5_start")
    {
        msockaddr_in sa;
        in>>sa;
        sendEvent(sa,ServiceEnum::DFSReferrer,
                  new dfsReferrerEvent::Hello(
                      getRpcExternalListenPortMain(iInstance),
                      getRpcInternalListenAddrs(iInstance),
                      ListenerBase::serviceId));

    }
    else
        log(ERROR_log,"unhandled upnp_result command %s",cmd.c_str());

    return true;
}
#endif
bool Service::on_ConnectFailed(rpcEvent::ConnectFailed const* e)
{
    MUTEX_INSPECTOR;
    S_LOG("on_ConnectFailed");

    msockaddr_in destination_addr=e->destination_addr;
    DBG(log(TRACE_log,"DFSREferrer::Service::on_ConnectFailed %s (%s %d)",destination_addr.dump().c_str(),__FILE__,__LINE__));

    d4_on_connect_failed(e->destination_addr);
    d6_on_connect_failed(e->destination_addr);


    return true;
}
bool Service::on_Disconnected(const rpcEvent::Disconnected* e)
{
    MUTEX_INSPECTOR;
    S_LOG("on_Disconnected");

    msockaddr_in addr=e->destination_addr;
    XTRY;

    d4_on_disconnected(e->destination_addr);
    d6_on_disconnected(e->destination_addr);

    XPASS;
    return true;
}
void Service::on_deleteDownlink(const SOCKET_id& sock, const msockaddr_in& )
{
    MUTEX_INSPECTOR;

    REF_getter<linkInfoDownReferrer> di=urirefs->getDownlinkOrNull(sock);
    {
        MUTEX_INSPECTOR;
        if(di.valid())
        {
            M_LOCK(di.operator ->());
            DBG(log(TRACE_log,"disaccepted valid user %s, do smth for notify",di->esi_mx->remote_name.dump().c_str()));
        }

        if(di.valid())
        {
            DBG(log(TRACE_log,"urirefs->downlinks.erase %d", CONTAINER(sock)));
            urirefs->eraseFromDownlinks(sock);
            for(auto& z:m_readyNotificationBackroutes)
            {
                passEvent(new dfsReferrerEvent::NotifyReferrerDownlinkDisconnected(sock,poppedFrontRoute(z)));
            }
        }
        else
        {
            DBG(log(TRACE_log,"invalid di because host is not downlink"));
        }
    }
}
bool Service::on_Disaccepted(const rpcEvent::Disaccepted* e)
{
    MUTEX_INSPECTOR;

    XTRY;
    S_LOG("on_Disaccepted sock "+iUtils->toString(CONTAINER(e->esi->m_id)));
    on_deleteDownlink(e->esi->m_id,e->esi->remote_name);
    XPASS;
    return true;
}

bool Service::on_Accepted(const rpcEvent::Accepted* )
{
    return true;
}

bool  Service::on_Connected(const rpcEvent::Connected* )
{
    return true;
}


bool Service::on_ToplinkDeliverRSP(const dfsReferrerEvent::ToplinkDeliverRSP *e)
{
    MUTEX_INSPECTOR;
    S_LOG("on_ToplinkDeliverRSP");
    if(e->route.size())
    {
        passEvent(e);
    }
    else
    {
        REF_getter<Event::Base> e_extr=e->getEvent();
        passEvent(e_extr);
        /// TODO smth
        //log(ERROR_log,"ERR on_ToplinkDeliverRSP route size =0 %s",__FUNCTION__);
    }
    return true;

}


bool Service::on_ToplinkDeliverREQ(const dfsReferrerEvent::ToplinkDeliverREQ *e111, const rpcEvent::IncomingOnAcceptor* acc)
{
    MUTEX_INSPECTOR;
    S_LOG("on_ToplinkDeliverREQ");

    DBG(logErr2("on_ToplinkDeliverREQ"));
    REF_getter<dfsReferrerEvent::ToplinkDeliverREQ> e_toplink=(dfsReferrerEvent::ToplinkDeliverREQ *)e111;


    if(uplinkConnectionState->m_isCapsServer)
    {
        MUTEX_INSPECTOR;
        S_LOG("m_isCapsServer");
        if(e_toplink->destinationService==ServiceEnum::DFSCaps)
        {
            MUTEX_INSPECTOR;
            sendEvent(ServiceEnum::DFSCaps,e_toplink.operator ->());
            return true;
        }
        if(e_toplink->destinationService==ServiceEnum::DFSReferrer)
        {
            MUTEX_INSPECTOR;
            REF_getter<Event::Base> z=e_toplink->getEvent();
            return handleEvent(z);
        }
        //iInstance->initService(e_toplink->destinationService);
        sendEvent(e_toplink->destinationService,e_toplink.operator ->());
        DBG(log(TRACE_log,"sent to q %s",e_toplink->destinationService.dump().c_str()));
        return true;
    }

    if(acc)
    {


        if(e_toplink->uuid.count(iInstance->globalCookie()))
        {
            DBG(logErr2("ciclic uuid %s",e_toplink->route.dump().c_str()));
            acc->esi->close("ciclic uuid");
            return true;
        }
        e_toplink->uuid.insert(iInstance->globalCookie());
    }
    else
    {
        /// from local service
        if(e_toplink->uuid.size())
            throw CommonError("if(ee->uuid.size()) "+_DMI());

        e_toplink->uuid.insert(iInstance->globalCookie());
    }

    if(hasUL())
    {
        MUTEX_INSPECTOR;
        sendToplinkReq(getUL(),e_toplink.operator ->());
    }
    else
    {
        throw CommonError("!if(hasUL())");
    }
    return true;

}


bool  Service::on_IncomingOnConnector(const rpcEvent::IncomingOnConnector *evt)
{
    MUTEX_INSPECTOR;
    S_LOG("OnConnector "+evt->esi->remote_name.dump());

    auto& IDC=evt->e->id;
    if( dfsReferrerEventEnum::ToplinkDeliverRSP==IDC)
        return on_ToplinkDeliverRSP(static_cast<const dfsReferrerEvent::ToplinkDeliverRSP* > (evt->e.operator ->()));
    if( dfsReferrerEventEnum::Pong==IDC) /// connector
        return on_Pong(static_cast<const dfsReferrerEvent::Pong* > (evt->e.operator ->()),evt->esi);
    if( dfsReferrerEventEnum::UpdateConfigRSP==IDC)
        return on_UpdateConfigRSP(static_cast<const dfsReferrerEvent::UpdateConfigRSP* > (evt->e.operator ->()));


    XTRY;


    log(ERROR_log,"unhandled event t t %s %s %d",evt->e->name,__FILE__,__LINE__);
    XPASS;
    return false;
}

bool  Service::on_IncomingOnAcceptor(const rpcEvent::IncomingOnAcceptor*evt)
{
    MUTEX_INSPECTOR;
    S_LOG("OnAcceptor "+evt->esi->remote_name.dump());
    XTRY;
    auto& IDA=evt->e->id;
    if(dfsReferrerEventEnum::Ping==IDA)/// acceptor
    {
        if(urirefs->countInDownlinks(evt->esi->m_id))
        {
            outBuffer o;
            o<<CONTAINER(evt->esi->m_id)<<evt->esi->remote_name;
            outBuffer oc;
            oc<<connection_sequence_id;

            resetAlarm(refTimer::T_011_downlink_ping_timed_out,o.asString(),oc.asString(),ListenerBase::serviceId);
        }
    }
    if(dfsReferrerEventEnum::ToplinkDeliverRSP==IDA)
    {
        MUTEX_INSPECTOR;
        passEvent(evt->e);
        return true;
    }
    if( dfsReferrerEventEnum::SubscribeNotifications==IDA)
        return on_SubscribeNotifications((const dfsReferrerEvent::SubscribeNotifications*)evt->e.operator ->());
    if( dfsReferrerEventEnum::ToplinkDeliverREQ==IDA)        /// forwading
        return on_ToplinkDeliverREQ(static_cast<const dfsReferrerEvent::ToplinkDeliverREQ* > (evt->e.operator ->()),evt);
    if( dfsReferrerEventEnum::Elloh==IDA)/// acceptor
        return on_acceptor_Elloh(static_cast<const dfsReferrerEvent::Elloh* > (evt->e.operator ->()),evt->esi);
    if( dfsReferrerEventEnum::Hello==IDA)/// acceptor
        return on_acceptor_Hello(static_cast<const dfsReferrerEvent::Hello* > (evt->e.operator ->()),evt->esi);
    if( dfsReferrerEventEnum::Ping==IDA)/// acceptor
        return on_Ping(static_cast<const dfsReferrerEvent::Ping* > (evt->e.operator ->()),evt->esi);
    if( dfsReferrerEventEnum::UpdateConfigRSP==IDA)
        return on_UpdateConfigRSP(static_cast<const dfsReferrerEvent::UpdateConfigRSP* > (evt->e.operator ->()));


    log(ERROR_log,"unhandled event %s %s %d",evt->e->name,__FILE__,__LINE__);
    XPASS;
    return false;
}


bool  Service::on_SubscribeNotifications(const dfsReferrerEvent::SubscribeNotifications*ev)
{
    m_readyNotificationBackroutes.insert(ev->route);
    if(hasUL())
    {
        passEvent(new dfsReferrerEvent::NotifyReferrerUplinkIsConnected(getUL(),poppedFrontRoute(ev->route)));
    }
    auto downlinks=urirefs->getDownlinks();
    for(auto z: downlinks)
    {
        REF_getter<linkInfoDownReferrer> l=z.second;
        passEvent(new dfsReferrerEvent::NotifyReferrerDownlinkConnected(l,poppedFrontRoute(ev->route)));
    }
    return true;

}


bool Service::handleEvent(const REF_getter<Event::Base>& ev)
{
    MUTEX_INSPECTOR;
    S_LOG("handleEvent");

    auto& ID=ev->id;
    XTRY;
    if( systemEventEnum::startService==ID)
        return on_startService((const systemEvent::startService*)ev.operator->());
    if( timerEventEnum::TickTimer==ID)
        return on_TickTimer((const timerEvent::TickTimer*)ev.operator->());
    if( timerEventEnum::TickAlarm==ID)
        return on_TickAlarm((const timerEvent::TickAlarm*)ev.operator->());
    if( webHandlerEventEnum::RequestIncoming==ID)
        return(this->on_RequestIncoming((const webHandlerEvent::RequestIncoming*)ev.operator->()));
    if( telnetEventEnum::CommandEntered==ID)
        return on_CommandEntered((const telnetEvent::CommandEntered*)ev.operator->());

    if(dfsCapsEventEnum::GetReferrersRSP==ID)
    {
        S_LOG("dfsCapsEventEnum::GetReferrersRSP==ID");
        if(stage!=STAGE_D3_GET_GEFERRERS)
        {
            DBG(log(TRACE_log,"if(stage!=STAGE_D3_GET_GEFERRERS)"));
            return true;
        }

        const dfsCapsEvent::GetReferrersRSP *e=(const dfsCapsEvent::GetReferrersRSP *)ev.operator ->();

        _stop_alarm(T_002_D3_caps_get_service_request_is_timed_out);

        for(auto i:e->referrer_addresses)
        {
            DBG(log(TRACE_log,"dfsReferrer: on dfsCaps::_getReferrers %s",i.dump().c_str()));
        }


        //stopAlarm(refTimer::T_010_request_cap_for_service_timed_out,NULL,ListenerBase::serviceId);

        //outBuffer o;
        //o<<connection_sequence_id;

        reset_T_001_common_connect_failed();

        int64_t now=iUtils->getNow().get();
        if(e->referrer_addresses.size()==0)
        {
            /// TODO
            //uplinkConnectionState->
        }
        else
        {
            for(auto i:e->referrer_addresses)
            {
                sendEvent(i,ServiceEnum::DFSReferrer,
                          new dfsReferrerEvent::Ping(dfsReferrer::PingType::PT_CACHED, iInstance->globalCookie(),
                                                     getRpcExternalListenPortMain(iInstance),
                                                     getRpcInternalListenAddrs(iInstance),
                                                     now,
                                                     connection_sequence_id,dfsReferrerEvent::Ping::CT_NODE,
                                                     ListenerBase::serviceId));
            }
        }
        stage=STAGE_D3_1_SEND_PING_TO_RESPONDED_REFERRERS;
        _reset_alarm(T_020_D31_wait_after_send_PT_CACHE_on_recvd_from_GetReferrers);



        return true;
    }
    if( rpcEventEnum::Disconnected==ID)
        return(this->on_Disconnected((const rpcEvent::Disconnected*)ev.operator->()));
    if( rpcEventEnum::Disaccepted==ID)
        return(this->on_Disaccepted((const rpcEvent::Disaccepted*)ev.operator->()));
    if( rpcEventEnum::ConnectFailed==ID)
        return(this->on_ConnectFailed((const rpcEvent::ConnectFailed*)ev.operator->()));
    if( rpcEventEnum::Connected==ID)
        return(this->on_Connected((const rpcEvent::Connected*)ev.operator->()));
    if( rpcEventEnum::Accepted==ID)
        return(this->on_Accepted((const rpcEvent::Accepted*)ev.operator->()));
    if( rpcEventEnum::Binded==ID)
        return(this->on_Binded((const rpcEvent::Binded*)ev.operator->()));
    if( rpcEventEnum::IncomingOnAcceptor==ID)
        return(this->on_IncomingOnAcceptor((const rpcEvent::IncomingOnAcceptor*)ev.operator->()));
    if( rpcEventEnum::IncomingOnConnector==ID)
        return(this->on_IncomingOnConnector((const rpcEvent::IncomingOnConnector*)ev.operator->()));
#if !defined(WITHOUT_UPNP)
    if( rpcEventEnum::UpnpResult==ID)
        return(this->on_UpnpResult((const rpcEvent::UpnpResult*)ev.operator->()));
#endif
    if( dfsReferrerEventEnum::SubscribeNotifications==ID)
        return on_SubscribeNotifications((const dfsReferrerEvent::SubscribeNotifications*)ev.operator ->());
    if( dfsReferrerEventEnum::ToplinkDeliverRSP==ID)
        return on_ToplinkDeliverRSP(static_cast<const dfsReferrerEvent::ToplinkDeliverRSP* > (ev.operator ->()));
    if( dfsReferrerEventEnum::ToplinkDeliverREQ==ID)       /// forwading
        return on_ToplinkDeliverREQ(static_cast<const dfsReferrerEvent::ToplinkDeliverREQ* > (ev.operator ->()),NULL);

    if( dfsReferrerEventEnum::UpdateConfigRSP==ID)
        return on_UpdateConfigRSP(static_cast<const dfsReferrerEvent::UpdateConfigRSP* > (ev.operator ->()));
    if( dfsReferrerEventEnum::UpdateConfigREQ==ID)
        return on_UpdateConfigREQ(static_cast<const dfsReferrerEvent::UpdateConfigREQ* > (ev.operator ->()));


    log(ERROR_log,"unhandled event t t %s %s %d",ev->name,__FILE__,__LINE__);
    XPASS;
    return false;
}

bool Service::on_CommandEntered(const telnetEvent::CommandEntered*e)
{
    MUTEX_INSPECTOR;
    if(e->tokens.size())
    {
        if(e->tokens[0]=="dumpFiles")
        {
            Json::StyledWriter w;
            Json::Value v;
            sendEvent(ServiceEnum::Telnet,new telnetEvent::Reply(e->socketId,w.write(v),ListenerBase::serviceId));
        }
        else if(e->tokens[0]=="dumpPipes")
        {
            Json::StyledWriter w;
            Json::Value v;
            sendEvent(ServiceEnum::Telnet,new telnetEvent::Reply(e->socketId,w.write(v),ListenerBase::serviceId));
        }
        else if(e->tokens[0]=="dumpUsers")
        {
            Json::StyledWriter w;
            Json::Value v;
            sendEvent(ServiceEnum::Telnet,new telnetEvent::Reply(e->socketId,w.write(v),ListenerBase::serviceId));
        }
        else throw CommonError("invalid command %s %d",__FILE__,__LINE__);
    }
    return true;
}



bool Service::on_TickTimer(const timerEvent::TickTimer*e)
{
    switch(e->tid)
    {
    }
    return false;
}

bool Service::on_TickAlarm(const timerEvent::TickAlarm* e)
{
    MUTEX_INSPECTOR;
    S_LOG("on_TickAlarm");
    inBuffer cookieBuf(e->cookie);
    int connsecid=cookieBuf.get_PN();
    if(connsecid!=connection_sequence_id)
    {
        DBG(logErr2("if(connsecid!=connection_sequence_id)"));
        return true;
    }
    switch(e->tid)
    {
    case refTimer::T_020_D31_wait_after_send_PT_CACHE_on_recvd_from_GetReferrers:
    {
        S_LOG("T_020_D31_wait_after_send_PT_CACHE_on_recvd_from_GetReferrers:");
        for(auto &x: uplinkConnectionState->ponged_all)
        {
            for(auto& y: x.second)
            {
                if(!isCaps(y.second))
                {
                    d4_uplink_mode(y.first,y.second);
                    return true;
                }
            }

        }
        for(auto &x: uplinkConnectionState->ponged_all)
        {
            for(auto& y: x.second)
            {
                if(isCaps(y.second))
                {
                    d4_uplink_mode(y.first,y.second);
                    return true;
                }
            }

        }

        reset_T_001_common_connect_failed();
    }
    return true;
    case T_001_common_connect_failed:
    {
        S_LOG("case T_001_common_connect_failed:");
        STAGE_D2_PING_NEIGHBOURS_start();
        return true;
    }
    case T_019_D5_external_connection_check_timeout:
    {
        S_LOG("T_019_D5_external_connection_check_timeout");
        //inBuffer in(e->cookie);
        msockaddr_in remote_addr;
        cookieBuf>>remote_addr;

#if defined(WITHOUT_UPNP)
        logErr2("External connection check failed, terminating app...");
        STAGE_D2_PING_NEIGHBOURS_start();
        //iUtils->setTerminate();
        return true;
#else
        if(m_upnpExecuted)
        {
            logErr2("External connection check failed, terminating app...");
            iUtils->setTerminate();
            return true;
        }
        else
        {
            if(!m_upnpExecuted && !m_upnpInRequesting)
            {
                m_upnpInRequesting=true;
                log(INFO_log,"UPNP check %s %d",__FILE__,__LINE__);
                outBuffer o;
                o<<(std::string)"d5_start"<<remote_addr;
                //if(!iInstance->no_bind())
                {
                    sendEvent(ServiceEnum::RPC,new rpcEvent::UpnpPortMap(o.asString()->asString(),ListenerBase::serviceId));
                    return true;
                }
            }

        }
#endif
        logErr2("INVALID CASE %s %d",__FILE__,__LINE__);
        return true;
    }


    case T_011_downlink_ping_timed_out:
    {
        MUTEX_INSPECTORS("T_011_downlink_ping_timed_out");
        inBuffer in(e->data);
        SOCKET_id sid;
        CONTAINER(sid)=in.get_PN();
        msockaddr_in sa;
        in>>sa;
        on_deleteDownlink(sid,sa);
        return true;
    }
    case T_012_reregister_referrer:
    {
        S_LOG("T_012_reregister_referrer");
        registerReferrer(getUL());
    }
    return true;
    case T_040_D2_cache_pong_timed_out_from_neighbours:
    {
        S_LOG("T_040_D2_cache_pong_timed_out_from_neighbours");
        if(stage!=STAGE_D2_PING_NEIGHBOURS)
        {
            DBG(log(TRACE_log,"if(stage!=STAGE_D2_PING_NEIGHBOURS)"));
            //STAGE_D2_PING_NEIGHBOURS_start();
            return true;
        }
        {
            STAGE_D21_PING_CAPS_start();
        }
        return true;
    }
    case T_002_D3_caps_get_service_request_is_timed_out:
    {
        S_LOG("T_002_D3_caps_get_service_request_is_timed_out");
        if(stage==STAGE_D3_GET_GEFERRERS)
        {
            for(auto &x: uplinkConnectionState->ponged_all)
            {
                for(auto& y: x.second)
                {
                    if(!isCaps(y.second))
                    {
                        d4_uplink_mode(y.first,y.second);
                        return true;
                    }
                }

            }
            for(auto &x: uplinkConnectionState->ponged_all)
            {
                for(auto& y: x.second)
                {
                    if(isCaps(y.second))
                    {
                        d4_uplink_mode(y.first,y.second);
                        return true;
                    }
                }

            }

        }
        return true;
    }
    case T_007_D6_resend_ping_caps_short:
    case T_008_D6_resend_ping_caps_long:
    case T_009_pong_timed_out_caps_long:
    {
        msockaddr_in remote_addr;
        inBuffer in(e->data);
        in>>remote_addr;
        switch(e->tid)
        {
        case T_007_D6_resend_ping_caps_short:
        {
            S_LOG("T_007_D6_resend_ping_caps_short");
            sendEvent(remote_addr,ServiceEnum::DFSReferrer,
                      new dfsReferrerEvent::Ping(isCaps(remote_addr)?dfsReferrer::PingType::PT_CAPS_SHORT:dfsReferrer::PingType::PT_MASTER_SHORT,
                                                 iInstance->globalCookie(),
                                                 getRpcExternalListenPortMain(iInstance),
                                                 getRpcInternalListenAddrs(iInstance),
                                                 iUtils->getNow().get(),
                                                 connection_sequence_id,
                                                 dfsReferrerEvent::Ping::CT_NODE,
                                                 ListenerBase::serviceId));
            _reset_alarm(T_007_D6_resend_ping_caps_short,remote_addr);

        }
        return true;
        case T_008_D6_resend_ping_caps_long:
        {
            S_LOG("T_008_D6_resend_ping_caps_long");
            if(stage==STAGE_D6_MAINTAIN_CONNECTION)
            {
                d6_on_T_011_resend_ping_PT_CAPS_LONG(remote_addr);

            }
            else
            {
                DBG(log(TRACE_log,"invalid stage"));
            }
        }

        return true;
        case T_009_pong_timed_out_caps_long:
        {
            S_LOG("T_009_pong_timed_out_caps_long");

            d6_on_T_012_pong_timed_out_PT_CAPS_LONG(remote_addr);
        }
        return true;
        default:
            log(ERROR_log,"invalid timer %d %s %d",e->tid,__FILE__,__LINE__);
        }
    }
    return true;
    default:
        log(ERROR_log,"unhandled timer %d %s %d",e->tid,__FILE__,__LINE__);

    }
    return false;
}

bool Service::on_RequestIncoming(const webHandlerEvent::RequestIncoming* e)
{
    MUTEX_INSPECTOR;
    HTTP::Response cc;
    cc.content+="<h1>DFS Referrer report</h1><p>";

    cc.content+=uplinkConnectionState->getWebDumpableLink("uplinkConnectionState")+"<p>";
    cc.content+=urirefs->getWebDumpableLink("urirefs")+"<p>";

    cc.makeResponse(e->esi);
    return true;

}

bool Service::on_Binded(const rpcEvent::Binded*e)
{
    MUTEX_INSPECTOR;
    if(e->esi->socketDescription=="RPC")
    {
    }
    return true;
}
void Service::kill_downlinks()
{
    auto downlinks=urirefs->getDownlinks();
    for(auto &i:downlinks)
    {
        REF_getter<linkInfoDownReferrer> &di=i.second;
        {
            M_LOCK(di.operator ->());
            di->esi_mx->close("uplink disconnected");
        }
    }
}
msockaddr_in Service::getUL()
{
    MUTEX_INSPECTOR;
    if(uplinkConnectionState->uplink.size()==0)
        throw CommonError("if(uplinkConnectionState->uplink.size()==0) %s",_DMI().c_str());
    return *uplinkConnectionState->uplink.begin();

}
bool Service::hasUL()
{
    MUTEX_INSPECTOR;
    if(uplinkConnectionState->uplink.size())
        return true;

    return false;

}
void Service::registerReferrer(const msockaddr_in& destination)
{
    MUTEX_INSPECTOR;
    S_LOG("void dfsReferrer::Service::registerReferrer(const msockaddr_in&)");
    if(externalAccessIsPossible)
    {

        DBG(log(TRACE_log,"send dfsCapsEvent::RegisterMyRefferrerREQ"));
        REF_getter<Event::Base> cr=
            new dfsCapsEvent::RegisterMyRefferrerREQ(iInstance->myExternalAddressesGet(),ListenerBase::serviceId);
        sendToplinkReq(destination,new ToplinkDeliverREQ(ServiceEnum::DFSCaps,cr.operator ->(),ListenerBase::serviceId));

    }
    else throw CommonError("!externalAccessIsPossible");
    outBuffer o;
    o<<connection_sequence_id;
    resetAlarm(refTimer::T_012_reregister_referrer,NULL,o.asString(),ListenerBase::serviceId);

}



void Service::_reset_alarm(int alarm_id)
{
    outBuffer o;
    o<<connection_sequence_id;
    resetAlarm(alarm_id,NULL,o.asString(),ListenerBase::serviceId);
}
void Service::_reset_alarm(int alarm_id, const msockaddr_in& sa)
{
    outBuffer o;
    o<<connection_sequence_id;
    resetAlarm(alarm_id,toRef(sa.asString()),o.asString(),ListenerBase::serviceId);
}
void Service::_reset_alarm(int alarm_id1,int alarm_id2, const msockaddr_in& sa)
{
    _reset_alarm(alarm_id1,sa);
    _reset_alarm(alarm_id2,sa);
}


void Service::_stop_alarm(int alarm_id)
{
    stopAlarm(alarm_id,NULL,ListenerBase::serviceId);
}
void Service::_stop_alarm(int alarm_id, const msockaddr_in& sa)
{
    stopAlarm(alarm_id,toRef(sa.asString()),ListenerBase::serviceId);
}
void Service::_stop_alarm(int alarm_id1, int alarm_id2, const msockaddr_in& sa)
{
    _stop_alarm(alarm_id1,sa);
    _stop_alarm(alarm_id2,sa);
}
void Service::_stop_alarm(int alarm_id1, int alarm_id2, int alarm_id3, const msockaddr_in& sa)
{
    _stop_alarm(alarm_id1,sa);
    _stop_alarm(alarm_id2,sa);
    _stop_alarm(alarm_id3,sa);
}

bool Service::on_Ping(const dfsReferrerEvent::Ping* e, const REF_getter<epoll_socket_info>& _esi)
{
    MUTEX_INSPECTOR;
    S_LOG("on_Ping");
    if(uplinkConnectionState->m_isCapsServer && e->clientType==dfsReferrerEvent::Ping::CT_CLIENT)
    {
        logErr2("caps cannot connect client type");
        return true;
    }

    bool valid=uplinkConnectionState->connectionEstablished && uplinkConnectionState->uplink.size() ;
    if(uplinkConnectionState->m_isCapsServer) valid=true;
    if(!valid)
    {
        /// if no uplink dont reply pong
        _esi->close("no uplink");
        return true;
    }
    if(CONTAINER(e->globalCookieOfSender).size())
    {
        if(e->globalCookieOfSender==iInstance->globalCookie())
        {
            DBG(log(TRACE_log,"on ping globalCookie matched %s",e->dump().toStyledString().c_str()));
            _esi->close("same globalCookie");
            return true;
        }
    }
    switch(e->pingType)
    {
    case dfsReferrer::PingType::PT_CAPS_LONG:
    case dfsReferrer::PingType::PT_CAPS_SHORT:
    case dfsReferrer::PingType::PT_MASTER_LONG:
    case dfsReferrer::PingType::PT_MASTER_SHORT:
    {
        if(_esi.valid())
        {
            msockaddr_in sa=_esi->remote_name;
            sa.setPort(e->externalListenPort);
            bool created=false;
            REF_getter<dfsReferrer::linkInfoDownReferrer>l=urirefs->getDownlinkOrCreate(_esi,sa,e->internalListenAddr,e->route,&created);
            if(created)
            {
                for(auto z: m_readyNotificationBackroutes)
                {
                    passEvent(new dfsReferrerEvent::NotifyReferrerDownlinkConnected(l,poppedFrontRoute(z)));
                }
                route_t br;
                {
                    M_LOCK(l.operator ->());
                    br=l->backRoute_mx;
                }
                passEvent(new dfsReferrerEvent::UpdateConfigRSP(config_body,poppedFrontRoute(br)));

            }
        }
    }
    break;
    }

    {
        passEvent(new dfsReferrerEvent::Pong(e->pingType,_esi->remote_name,
                                             iInstance->globalCookie(),
                                             uplinkConnectionState->nodeLevelInHierarhy,
                                             e->ping_time,
                                             e->connection_sequence_id,
                                             poppedFrontRoute(e->route)));
        return true;
    }
    return true;

}
void Service::sendToplinkReq(const msockaddr_in& uplink,dfsReferrerEvent::ToplinkDeliverREQ *ee)
{
    MUTEX_INSPECTOR;
    S_LOG("sendToplinkReq");
    //DBG(log(TRACE_log,"req %s",ee->dump().c_str()));
    if(uplinkConnectionState->m_isCapsServer)
    {
        S_LOG("m_isCapsServer");
        if(ee->destinationService==ServiceEnum::DFSReferrer)
        {

            sendEvent(ee->destinationService,ee);
        }
        if(ee->destinationService==ServiceEnum::DFSCaps)
        {
            sendEvent(ServiceEnum::DFSCaps,ee);
            return;
        }
        sendEvent(ee->destinationService,ee);
    }
    else
    {
        if(ee->uuid.size()==0)
        {

        }
        ee->uuid.insert(iInstance->globalCookie());
        sendEvent(uplink,ServiceEnum::DFSReferrer,ee);
    }


}

bool Service::d6_on_pong_PT_CAPS_LONG(const msockaddr_in& visible_name_of_pinger, const REF_getter<epoll_socket_info>& esi)
{
    MUTEX_INSPECTOR;
    S_LOG("d6_on_pong_PT_CAPS_LONG");
    DBG(log(TRACE_log,"d6_on_pong_PT_CAPS_LONG %s",esi->remote_name.dump().c_str()));
    _reset_alarm(T_008_D6_resend_ping_caps_long,T_009_pong_timed_out_caps_long,esi->remote_name);
    return true;
}
void Service::d6_on_T_011_resend_ping_PT_CAPS_LONG(const msockaddr_in& remote_addr)
{
    S_LOG("d6_on_T_008_D6_resend_ping_caps_long");
    MUTEX_INSPECTOR;
    DBG(logErr2("send PT_CAPS_LONG %s @%s",remote_addr.dump().c_str(),__PRETTY_FUNCTION__));

    sendEvent(remote_addr,ServiceEnum::DFSReferrer,
              new dfsReferrerEvent::Ping(isCaps(remote_addr)?dfsReferrer::PingType::PT_CAPS_LONG:dfsReferrer::PingType::PT_MASTER_LONG,
                                         iInstance->globalCookie(),
                                         getRpcExternalListenPortMain(iInstance),
                                         getRpcInternalListenAddrs(iInstance),
                                         iUtils->getNow().get(),
                                         connection_sequence_id,
                                         dfsReferrerEvent::Ping::CT_NODE,
                                         ListenerBase::serviceId));
}
void Service::d6_on_T_012_pong_timed_out_PT_CAPS_LONG(const msockaddr_in& remote_addr)
{
    S_LOG("d6_on_T_009_pong_timed_out_caps_long");
    MUTEX_INSPECTOR;
    d6_on_disconnected(remote_addr);
}
void Service::d6_on_disconnected(const msockaddr_in& remote_addr)
{
    S_LOG("d6_on_disconnected");
    MUTEX_INSPECTOR;
    if(uplinkConnectionState->uplink.count(remote_addr))
    {
        DBG(logErr2("Caps connect broken, try to reconnect..."));

        DBG(log(TRACE_log,"%s ",remote_addr.dump().c_str()));
        //uplinkConnectionState->isCapsConnected.clear();

        DBG(log(TRACE_log,"isCapsRequested.clear %s %d",__FILE__,__LINE__));
        _stop_alarm(T_007_D6_resend_ping_caps_short,T_008_D6_resend_ping_caps_long,T_009_pong_timed_out_caps_long,remote_addr);
        STAGE_D2_PING_NEIGHBOURS_start();
    }
}
void Service::d6_on_connect_failed(const msockaddr_in& remote_addr)
{
    S_LOG("d6_on_connect_failed");
    d6_on_disconnected(remote_addr);
}



#define IGNORE_NAT
bool Service::on_Pong(const dfsReferrerEvent::Pong* e, const REF_getter<epoll_socket_info>& esi)
{
    MUTEX_INSPECTOR;
    S_LOG("on_Pong");
    if(e->globalCookieOfResponder==iInstance->globalCookie())
    {
        DBG(log(TRACE_log,"if(e->globalCookieOfResponder==iInstance->globalCookie())"));
        return true;
    }
    if(e->connection_sequence_id!=connection_sequence_id)
    {
        DBG(log(TRACE_log,"if(e->connection_sequence_id!=connection_sequence_id) %d %d",e->connection_sequence_id,connection_sequence_id));
        return true;
    }

    msockaddr_in visible_name_of_pinger=e->visible_name_of_pinger;

    visible_name_of_pinger.setPort(getRpcExternalListenPortMain(iInstance));


    uplinkConnectionState->nodeLevelInHierarhy=e->nodeLevelInHierarhy+1;
    uplinkConnectionState->ponged_for_cleanup_sockets.insert(esi);
    uplinkConnectionState->ponged_all[iUtils->getNow().get()-e->ping_time].insert(std::make_pair(esi,e->visible_name_of_pinger));
    if(uplinkConnectionState->m_isCapsServer && uplinkConnectionState->nodeLevelInHierarhy!=0)
    {
        throw CommonError("if(uplinkConnectionState->m_isCapsServer && uplinkConnectionState->nodeLevelInHierarhy!=0)");
    }
    neighbours.add(esi->remote_name,e->ping_time);
#ifndef IGNORE_NAT
    if(sa.isNAT())
    {
        DBG(logErr2("isNAT %s",sa.dump().c_str()));
    }
    else
#endif
    {
        if(!iInstance->myExternalAddressesCount(visible_name_of_pinger))
        {
            iInstance->myExternalAddressesInsert(visible_name_of_pinger);
        }
    }

    switch(e->pingType)
    {
    case PingType::PT_CACHED:
    {
        //logErr2("case PingType::PT_CACHED:");
        if(stage==STAGE_D2_PING_NEIGHBOURS||stage==STAGE_D21_PING_CAPS)
        {
            bool ok_send=false;

            if(stage==STAGE_D2_PING_NEIGHBOURS)
            {
                if(!isCaps(esi->remote_name))
                {
                    ok_send=true;
                }
            }
            if(stage==STAGE_D21_PING_CAPS)
            {
                if(isCaps(esi->remote_name))
                {
                    ok_send=true;
                }
            }
            if(ok_send)
            {
                STAGE_D3_GET_GEFERRERS_start(esi->remote_name);
            }
        }
        if(stage==STAGE_D3_1_SEND_PING_TO_RESPONDED_REFERRERS)
        {

            for(auto& x: uplinkConnectionState->ponged_all)
            {
                for(auto& y: x.second)
                {
                    if(!isCaps(y.first->remote_name))
                    {
                        if(y.second==e->visible_name_of_pinger)
                        {
//                            throw CommonError("if(y.second!=e->visible_name_of_pinger) %s %s",y.second.dump().c_str(),e->visible_name_of_pinger.dump().c_str());
                            _stop_alarm(T_020_D31_wait_after_send_PT_CACHE_on_recvd_from_GetReferrers);
                            d4_uplink_mode(y.first,e->visible_name_of_pinger);
                            return true;
                        }
                    }
                }
            }
        }
        {
            DBG(log(TRACE_log,"d2_on_pong_PT_CACHED %s",esi->remote_name.dump().c_str()));
        }

        return true;

    }
    case PingType::PT_CAPS_SHORT:
    case PingType::PT_MASTER_SHORT:
        return d6_on_pong_PT_CAPS_SHORT(e->visible_name_of_pinger, esi);

    case PingType::PT_CAPS_LONG:
    case PingType::PT_MASTER_LONG:
        return d6_on_pong_PT_CAPS_LONG(e->visible_name_of_pinger, esi);
    default:
        log(ERROR_log,"invalid pingType %d %s %d",e->pingType,__FILE__,__LINE__);


    }
    return true;

}
bool Service::on_acceptor_Elloh(const dfsReferrerEvent::Elloh* e, const REF_getter<epoll_socket_info>& esi)
{
    MUTEX_INSPECTOR;
    S_LOG("on_acceptor_Elloh");
    {
        msockaddr_in ms=esi->remote_name;
        ms.setPort(e->externalListenPort);

        externalAccessIsPossible=true;

        logErr2("--------- external connection test successful");
        stopAlarm(refTimer::T_019_D5_external_connection_check_timeout,NULL,ListenerBase::serviceId);
        on_connectionEstablished(ms);

    }
    return true;

}
bool Service::on_acceptor_Hello(const dfsReferrerEvent::Hello* e, const REF_getter<epoll_socket_info>& esi)
{
    MUTEX_INSPECTOR;
    S_LOG("on_acceptor_Hello");

    DBG(logErr2("on_acceptor_Hello %s",e->dump().toStyledString().c_str()));
    DBG(log(TRACE_log,"sa %s",esi->remote_name.dump().c_str()));
    msockaddr_in ms=esi->remote_name;
    ms.setPort(e->externalListenPort);
    DBG(log(TRACE_log,"senEvent Elloh::CHECK_FOR_EXTERNAL_CONNECTION %s",ms.dump().c_str()));
    sendEvent(ms,ServiceEnum::DFSReferrer,
              new dfsReferrerEvent::Elloh(
                  getRpcExternalListenPortMain(iInstance),
                  ListenerBase::serviceId));
    return true;


}

bool Service::on_connectionEstablished(const msockaddr_in& remote_addr)
{
    MUTEX_INSPECTOR;
    S_LOG("on_connectionEstablished");
    stopAlarm(refTimer::T_001_common_connect_failed,NULL,ListenerBase::serviceId);

    DBG(log(TRACE_log,"bool Service::on_connectionEstablished( ) !!!--- %s",remote_addr.dump().c_str()));

    logErr2("--------- connection established %s",remote_addr.dump().c_str());

    uplinkConnectionState->connectionEstablished=true;
    for(auto &i:m_readyNotificationBackroutes)
    {
        passEvent(new dfsReferrerEvent::NotifyReferrerUplinkIsConnected(remote_addr,poppedFrontRoute(i)));
    }
    uplinkConnectionState->uplink.insert(remote_addr);

    registerReferrer(remote_addr);

    for(auto &z:uplinkConnectionState->ponged_for_cleanup_sockets)
    {
        if(z->remote_name!=remote_addr)
        {
            DBG(logErr2("close %s",z->remote_name.dump().c_str()));
            z->close("cleanup unused connections");

        }
    }

    uplinkConnectionState->ponged_for_cleanup_sockets.clear();

    d6_start(remote_addr);
    return true;
}
void Service::d4_on_disconnected(const msockaddr_in& sa)
{
    S_LOG("d4_on_disconnected "+sa.dump());
    MUTEX_INSPECTOR;
    DBG(log(TRACE_log,"addr is uplink %d",uplinkConnectionState->uplink.count(sa)));
    if(uplinkConnectionState->uplink.count(sa))
    {
        for(auto& z: m_readyNotificationBackroutes)
        {
            passEvent(new dfsReferrerEvent::NotifyReferrerUplinkIsDisconnected(sa,poppedFrontRoute(z)));
        }
#ifdef CLOSE_DOWNLINKS
        auto downlinks=urirefs->getDownlinks();
        for(auto &z: downlinks)
        {
            REF_getter<linkInfoDownReferrer> ld=z.second;
            {
                M_LOCK(ld.operator ->());
                ld->esi_mx->close("uplink disconnected");
                for(auto& r: m_readyNotificationBackroutes)
                {
                    passEvent(new dfsReferrerEvent::NotifyReferrerDownlinkDisconnected(z.first,poppedFrontRoute(r)));
                }
            }

        }
        urirefs->clearDownlinks();;
#endif

        STAGE_D2_PING_NEIGHBOURS_start();
    }
}
void Service::d4_on_connect_failed(const msockaddr_in& sa)
{
    S_LOG("d4_on_connect_failed");
    MUTEX_INSPECTOR;
    DBG(log(TRACE_log,"d4_on_connect_failed %s",sa.dump().c_str()));
    d4_on_disconnected(sa);

}

void Service::d4_uplink_mode(const REF_getter<epoll_socket_info>& esi, const msockaddr_in &visibleName)
{
    //logErr2("d4_uplink_mode %s", esi->remote_name.dump().c_str());
    S_LOG("d4_uplink_mode");
    MUTEX_INSPECTOR;
    DBG(log(TRACE_log,"switch mode"));

    stage=STAGE_D4_MAINTAIN_CONNECTION;

    if(!externalAccessIsPossible)
    {
        d5_start_external_connection_check(esi,visibleName);
    }
    else
    {
        on_connectionEstablished(esi->remote_name);
    }



}

void Service::d5_start_external_connection_check(const REF_getter<epoll_socket_info>&esi,const msockaddr_in& visibleName)
{
    if(stage==STAGE_D5_CHECK_EXTERNAL)
        return;
    stage=STAGE_D5_CHECK_EXTERNAL;

    S_LOG("d5_start_upnp_check");
    MUTEX_INSPECTOR;
    DBG(log(TRACE_log,"sa %s ",esi->remote_name.dump().c_str()));

    {
        DBG(logErr2("SEND_ME_CHECK_REQUEST"));
        sendEvent(esi->remote_name,ServiceEnum::DFSReferrer,
                  new dfsReferrerEvent::Hello(
                      getRpcExternalListenPortMain(iInstance),
                      getRpcInternalListenAddrs(iInstance),
                      ListenerBase::serviceId));
        outBuffer o;
        o<<connection_sequence_id<<visibleName;
        resetAlarm(refTimer::T_019_D5_external_connection_check_timeout,NULL,o.asString(),ListenerBase::serviceId);
    }
}


void Service::d6_start(const msockaddr_in& sa)
{
    stage=STAGE_D6_MAINTAIN_CONNECTION;
    S_LOG("d6_start "+sa.dump());
    MUTEX_INSPECTOR;
    DBG(log(TRACE_log,"d6_start send PT_CAPS_SHORT"));
    sendEvent(sa,ServiceEnum::DFSReferrer,
              new dfsReferrerEvent::Ping(dfsReferrer::PingType::PT_CAPS_SHORT,
                                         iInstance->globalCookie(),
                                         getRpcExternalListenPortMain(iInstance),
                                         getRpcInternalListenAddrs(iInstance),
                                         iUtils->getNow().get(),
                                         connection_sequence_id,
                                         dfsReferrerEvent::Ping::CT_NODE,
                                         ListenerBase::serviceId));
    _reset_alarm(T_007_D6_resend_ping_caps_short,sa);
}
bool Service::d6_on_pong_PT_CAPS_SHORT(const msockaddr_in& visible_name_of_pinger, const REF_getter<epoll_socket_info>& esi)
{
    MUTEX_INSPECTOR;
    S_LOG("d6_on_pong_PT_CAPS_SHORT");
    {

        _stop_alarm(T_007_D6_resend_ping_caps_short,T_008_D6_resend_ping_caps_long,T_009_pong_timed_out_caps_long,esi->remote_name);
        _reset_alarm(T_008_D6_resend_ping_caps_long,T_009_pong_timed_out_caps_long,esi->remote_name);

    }
    return true;

}
void Service::reset_T_001_common_connect_failed()
{
    outBuffer ocookie;
    ocookie<<connection_sequence_id;
    resetAlarm(refTimer::T_001_common_connect_failed,NULL,ocookie.asString(),ListenerBase::serviceId);
}
bool Service::isCaps(const msockaddr_in& sa)
{
    return uplinkConnectionState->saCapsFromConfig.count(sa);
}

//void Service::
void Service::STAGE_D21_PING_CAPS_start()
{
    stage=STAGE_D21_PING_CAPS;
    for(auto &caps:uplinkConnectionState->saCapsFromConfig)
    {
        auto external=getRpcExternalListenPortMain(iInstance);
        auto internal=getRpcInternalListenAddrs(iInstance);
        auto gcid=iInstance->globalCookie();
        sendEvent(caps,ServiceEnum::DFSReferrer,
                  new dfsReferrerEvent::Ping(dfsReferrer::PingType::PT_CACHED, gcid,external,internal,iUtils->getNow().get(),
                                             connection_sequence_id,
                                             dfsReferrerEvent::Ping::CT_NODE,
                                             ListenerBase::serviceId));
    }
    reset_T_001_common_connect_failed();

}
void Service::STAGE_D2_PING_NEIGHBOURS_start()
{
    connection_sequence_id++;

    stage=STAGE_D2_PING_NEIGHBOURS;


    I_ssl *ssl=(I_ssl*)iUtils->queryIface(Ifaces::SSL);
    if(!uplinkConnectionState.valid())
        throw CommonError("if(!uplinkConnectionState.valid())");
    uplinkConnectionState=new _uplinkConnectionState(uplinkConnectionState->m_isCapsServer,uplinkConnectionState->saCapsFromConfig);
    MUTEX_INSPECTOR;
    S_LOG("d2_start");
    DBG(log(TRACE_log,"d2_start"));
    if(uplinkConnectionState->m_isCapsServer)
    {
        DBG(log(TRACE_log,"uplinkConnectionState->m_isCapsServer NO d2 start"));
        return;
    }

    if(time(NULL) - d2_start_time<2)
    {
        DBG(log(TRACE_log,"if(time(NULL) - d2_start_time<2)"));
        sleep(1);
    }
    d2_start_time=time(NULL);
    reset_T_001_common_connect_failed();



    std::vector<msockaddr_in> s=neighbours.getAllAndClear();
    bool sent=false;
    if(s.size())
    {
        for(auto& i:s)
        {
            if(isCaps(i)) continue;
            sendEvent(i,ServiceEnum::DFSReferrer,
                      new dfsReferrerEvent::Ping(dfsReferrer::PingType::PT_CACHED, iInstance->globalCookie(),
                                                 getRpcExternalListenPortMain(iInstance),getRpcInternalListenAddrs(iInstance),
                                                 iUtils->getNow().get(),
                                                 connection_sequence_id,
                                                 dfsReferrerEvent::Ping::CT_NODE,
                                                 ListenerBase::serviceId));
        }
        _reset_alarm(T_040_D2_cache_pong_timed_out_from_neighbours);
    }
    else
    {
        STAGE_D21_PING_CAPS_start();
    }

}

void Service::STAGE_D3_GET_GEFERRERS_start(const msockaddr_in& sa)
{
    stage=STAGE_D3_GET_GEFERRERS;

    S_LOG("d3_start "+sa.dump());
    MUTEX_INSPECTOR;
    DBG(log(TRACE_log,"d3_start"));

    //if(!iInstance)
    //  throw CommonError("!iInstance "+_DMI());
    //if(iInstance->isTerminating()) return;

    //{
    {
        std::set<msockaddr_in> sas=iInstance->myExternalAddressesGet();
        if(sas.size()==0)
            throw CommonError("if(sas.size()==0)");

        DBG(log(TRACE_log,"dfsCapsEvent::GetReferrersREQ "));

        REF_getter<Event::Base> ze=new dfsCapsEvent::GetReferrersREQ(sas,ListenerBase::serviceId);
        sendToplinkReq(sa,new ToplinkDeliverREQ(ServiceEnum::DFSCaps,ze.operator ->(),ListenerBase::serviceId));
        _reset_alarm(T_002_D3_caps_get_service_request_is_timed_out);
    }
    //}
    reset_T_001_common_connect_failed();

}
bool Service::on_UpdateConfigREQ(const dfsReferrerEvent::UpdateConfigREQ*e)
{
    //logErr2("bool Service::on_UpdateConfigREQ(const dfsReferrerEvent::UpdateConfigREQ*e) %s",e->body.c_str());
    std::map<SOCKET_id, REF_getter<linkInfoDownReferrer> >ls=urirefs->getDownlinks();
    for(auto& x: ls)
    {
        route_t br;
        {
            M_LOCK(x.second.operator ->());
            br=x.second->backRoute_mx;
        }
        passEvent(new dfsReferrerEvent::UpdateConfigRSP(e->body,poppedFrontRoute(br)));
    }
    config_body=e->body;
    ConfigDB_private c;
    c.set("refferer_config",config_body);

    return true;
}
bool Service::on_UpdateConfigRSP(const dfsReferrerEvent::UpdateConfigRSP*e)
{
    DBG(logErr2("bool Service::on_UpdateConfigRSP(const dfsReferrerEvent::UpdateConfigRSP*e) %s",e->body.c_str()));
    config_body=e->body;
    std::map<SOCKET_id, REF_getter<linkInfoDownReferrer> >ls=urirefs->getDownlinks();
    for(auto& x: ls)
    {
        route_t br;
        {
            M_LOCK(x.second.operator ->());
            br=x.second->backRoute_mx;
        }
        passEvent(new dfsReferrerEvent::UpdateConfigRSP(e->body,poppedFrontRoute(br)));
    }
    return true;

}

