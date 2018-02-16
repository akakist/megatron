#include "referrerClientService.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#ifndef _WIN32
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#endif
#include "IInstance.h"



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


#include "sqlite3Wrapper.h"
#include "configDB.h"

#include "Events/Tools/webHandler/RequestIncoming.h"
#include "Events/Tools/webHandler/RegisterDirectory.h"
#include "Events/Tools/webHandler/RegisterHandler.h"
#include "Events/Tools/telnet/RegisterCommand.h"
#include "Events/Tools/telnet/Reply.h"
#include "Events/System/Net/rpc/SubscribeNotifications.h"
#include "Events/System/Net/http/GetBindPortsRSP.h"
#include "Events/DFS/Referrer/UpdateConfigREQ.h"
#include "Events/DFS/Referrer/UpdateConfigRSP.h"


#include "events_referrerClient.hpp"

using namespace referrerClient;
using namespace dfsReferrerEvent;



Service::Service(const SERVICE_id &svs, const std::string&  nm, IInstance* ifa):
    UnknownBase(nm),
    ListenerBuffered1Thread(this,nm,ifa->getConfig(),svs,ifa),Broadcaster(ifa),
    Logging(this,
#ifdef DEBUG
            TRACE_log
#else
            ERROR_log
#endif
    ,ifa   ),TimerHelper(ifa),//IReferrer(this),

    uplinkConnectionState(NULL),
    d2_start_time(0),
    m_upnpExecuted(false),
    m_upnpInRequesting(false),
    externalAccessIsPossible(false),
    iInstance(ifa),
    connection_sequence_id(0)


{

    MUTEX_INSPECTOR;
    try
    {
        XTRY;
        IConfigObj* config=ifa->getConfig();
        setTimerValue(crefTimer::T_001_common_connect_failed,config->get_real("T_001_common_connect_failed", 20,""));
        setTimerValue(crefTimer::T_002_D3_caps_get_service_request_is_timed_out,config->get_real("T_002_D3_caps_get_service_request_is_timed_out", 15.0,""));
        setTimerValue(crefTimer::T_007_D6_resend_ping_caps_short,config->get_real("T_007_D6_resend_ping_caps_short", 7.0,""));
        setTimerValue(crefTimer::T_008_D6_resend_ping_caps_long,config->get_real("T_008_D6_resend_ping_caps_long", 20.0,""));
        setTimerValue(crefTimer::T_009_pong_timed_out_caps_long,config->get_real("T_009_pong_timed_out_caps_long", 40.0,""));
        setTimerValue(crefTimer::T_020_D31_wait_after_send_PT_CACHE_on_recvd_from_GetReferrers,config->get_real("T_020_D31_wait_after_send_PT_CACHE_on_recvd_from_GetReferrers", 2.0,""));
        setTimerValue(crefTimer::T_040_D2_cache_pong_timed_out_from_neighbours,config->get_real("T_040_D2_cache_pong_timed_out_from_neighbours", 2,""));
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

    return true;
}

Service::~Service()
{
}



UnknownBase* Service::construct(const SERVICE_id& id, const std::string&  nm,IInstance* ifa)
{
    return new Service(id,nm,ifa);
}

void registerReferrerClientService(const char* pn)
{
    XTRY;
    if(pn)
    {
        iUtils->registerPlugingInfo(COREVERSION,pn,IUtils::PLUGIN_TYPE_SERVICE,ServiceEnum::ReferrerClient,"ReferrerClient");
    }
    else
    {
        iUtils->registerService(COREVERSION,ServiceEnum::ReferrerClient,Service::construct,"ReferrerClient");
        regEvents_referrerClient();
    }
    XPASS;

}



bool Service::on_ConnectFailed(rpcEvent::ConnectFailed const* e)
{
    MUTEX_INSPECTOR;
    S_LOG("on_ConnectFailed");
    /**
      Для центрального сервера никаких реконнектов нет
      */

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


bool Service::on_ToplinkDeliverREQ(const dfsReferrerEvent::ToplinkDeliverREQ *e111)
{
    MUTEX_INSPECTOR;
    S_LOG("on_ToplinkDeliverREQ");

    DBG(logErr2("on_ToplinkDeliverREQ"));
    REF_getter<dfsReferrerEvent::ToplinkDeliverREQ> ee=(dfsReferrerEvent::ToplinkDeliverREQ *)e111;



    {
        /// from local service
        if(ee->uuid.size())
            throw CommonError("if(ee->uuid.size()) "+_DMI());

        ee->uuid.insert(iInstance->globalCookie());
    }

    if(hasUL())
    {
        sendToplinkReqClient(getUL(),ee.operator ->());
    }
    else
    {
        throw CommonError("!if(hasUL())");
    }
    return true;

}


bool  Service::on_IncomingOnConnector(const rpcEvent::IncomingOnConnector *evt)
{
    S_LOG("OnConnector "+evt->esi->remote_name.dump());

    auto& IDC=evt->e->id;
    if( dfsReferrerEventEnum::ToplinkDeliverRSP==IDC)
        return on_ToplinkDeliverRSP(static_cast<const dfsReferrerEvent::ToplinkDeliverRSP* > (evt->e.operator ->()));
    if( dfsReferrerEventEnum::Pong==IDC) /// connector
        return on_Pong(static_cast<const dfsReferrerEvent::Pong* > (evt->e.operator ->()),evt->esi);
    if(dfsReferrerEventEnum::UpdateConfigRSP==IDC)
    {
        const dfsReferrerEvent::UpdateConfigRSP*xe=(const dfsReferrerEvent::UpdateConfigRSP*)evt->e.operator ->();
        logErr2("referrerClientService.cpp: if(dfsReferrerEventEnum::UpdateConfigRSP==IDC) %s route=%s",xe->body.c_str(),xe->route.dump().c_str() );
        config_body=xe->body;

        for(auto& z: m_readyNotificationBackroutes)
        {
            passEvent(new dfsReferrerEvent::UpdateConfigRSP(config_body,poppedFrontRoute(z)));
        }
        return true;
    }

    XTRY;
    MUTEX_INSPECTOR;


    log(ERROR_log,"unhandled event t t %s %s %d",evt->e->name,__FILE__,__LINE__);
    XPASS;
    return false;
}



bool  Service::on_SubscribeNotifications(const dfsReferrerEvent::SubscribeNotifications*ev)
{
    m_readyNotificationBackroutes.insert(ev->route);
    if(uplinkConnectionState.valid())
    {
        if(uplinkConnectionState->uplink.size())
        {
            passEvent(new dfsReferrerEvent::NotifyReferrerUplinkIsConnected(getUL(),poppedFrontRoute(ev->route)));
            if(config_body.size())
            {
                passEvent(new dfsReferrerEvent::UpdateConfigRSP(config_body,poppedFrontRoute(ev->route)));
            }

        }
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

    if(dfsReferrerEventEnum::UpdateConfigRSP==ID)
    {
        const dfsReferrerEvent::UpdateConfigRSP*xe=(const dfsReferrerEvent::UpdateConfigRSP*)ev.operator ->();
        config_body=xe->body;
        return true;
    }

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
                                                     now,
                                                     connection_sequence_id,
                                                     ListenerBase::serviceId));
            }
        }
        stage=STAGE_D3_1_SEND_PING_TO_RESPONDED_REFERRERS;
        _reset_alarm(T_020_D31_wait_after_send_PT_CACHE_on_recvd_from_GetReferrers);



        return true;
    }
    if( rpcEventEnum::Disconnected==ID)
        return(this->on_Disconnected((const rpcEvent::Disconnected*)ev.operator->()));
    if( rpcEventEnum::ConnectFailed==ID)
        return(this->on_ConnectFailed((const rpcEvent::ConnectFailed*)ev.operator->()));
    if( rpcEventEnum::Connected==ID)
        return(this->on_Connected((const rpcEvent::Connected*)ev.operator->()));
    if( rpcEventEnum::IncomingOnConnector==ID)
        return(this->on_IncomingOnConnector((const rpcEvent::IncomingOnConnector*)ev.operator->()));
    if( dfsReferrerEventEnum::SubscribeNotifications==ID)
        return on_SubscribeNotifications((const dfsReferrerEvent::SubscribeNotifications*)ev.operator ->());
    if( dfsReferrerEventEnum::ToplinkDeliverRSP==ID)
        return on_ToplinkDeliverRSP(static_cast<const dfsReferrerEvent::ToplinkDeliverRSP* > (ev.operator ->()));
    if( dfsReferrerEventEnum::ToplinkDeliverREQ==ID)       /// forwading
        return on_ToplinkDeliverREQ(static_cast<const dfsReferrerEvent::ToplinkDeliverREQ* > (ev.operator ->()));
    if( dfsReferrerEventEnum::InitClient==ID)       /// forwading
        return on_InitClient(static_cast<const dfsReferrerEvent::InitClient* > (ev.operator ->()));

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
    case crefTimer::T_020_D31_wait_after_send_PT_CACHE_on_recvd_from_GetReferrers:
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



    case T_040_D2_cache_pong_timed_out_from_neighbours:
    {
        S_LOG("T_040_D2_cache_pong_timed_out_from_neighbours");
        if(stage!=STAGE_D2_PING_NEIGHBOURS)
        {
            DBG(log(TRACE_log,"if(stage!=STAGE_D2_PING_NEIGHBOURS)"));
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
                                                 iUtils->getNow().get(),
                                                 connection_sequence_id,
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
    cc.content+="<h1>Referrer client report</h1><p>";


    cc.makeResponse(e->esi);
    return true;

}

msockaddr_in Service::getUL()
{
    MUTEX_INSPECTOR;
    if(!uplinkConnectionState.valid())
        throw CommonError("if(!uplinkConnectionState.valid())");
    if(uplinkConnectionState->uplink.size()==0)
        throw CommonError("if(uplinkConnectionState->uplink.size()==0) %s",_DMI().c_str());
    return *uplinkConnectionState->uplink.begin();

}
bool Service::hasUL()
{
    MUTEX_INSPECTOR;

    if(!uplinkConnectionState.valid())
        return false;
    if(uplinkConnectionState->uplink.size())
        return true;

    return false;

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

void Service::sendToplinkReqClient(const msockaddr_in& uplink,dfsReferrerEvent::ToplinkDeliverREQ *ee)
{
    MUTEX_INSPECTOR;
    S_LOG("sendToplinkReq");
    //DBG(log(TRACE_log,"req %s",ee->dump().c_str()));
    {
        if(ee->uuid.size()==0)
        {

        }
        ee->uuid.insert(iInstance->globalCookie());
        //DBG(log(TRACE_log,"send %s %s",uplink.dump().c_str(),ee->dump().c_str()));
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
                                         iUtils->getNow().get(),
                                         connection_sequence_id,
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
        DBG(log(TRACE_log,"if(e->connection_sequence_id!=connection_sequence_id)"));
        return true;
    }

    msockaddr_in visible_name_of_pinger=e->visible_name_of_pinger;

    //visible_name_of_pinger.setPort(getRpcExternalListenPortMain(iInstance));

    //DBG(log(TRACE_log,"e->visible_name_of_pinger %s visible_name_of_pinger %s",e->visible_name_of_pinger.dump().c_str(),visible_name_of_pinger.dump().c_str()));

    uplinkConnectionState->nodeLevelInHierarhy=e->nodeLevelInHierarhy+1;
    uplinkConnectionState->ponged_for_cleanup_sockets.insert(esi);
    uplinkConnectionState->ponged_all[iUtils->getNow().get()-e->ping_time].insert(std::make_pair(esi,e->visible_name_of_pinger));
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
    case dfsReferrer::PingType::PT_CACHED:
    {
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
//#ifdef KALLREMOVEAFTERDEBUG
                        if(y.second.inaddr()!=e->visible_name_of_pinger.inaddr())
                            throw CommonError("if(y.second!=e->visible_name_of_pinger) %s %s",y.second.dump().c_str(),e->visible_name_of_pinger.dump().c_str());
//#endif
                        _stop_alarm(T_020_D31_wait_after_send_PT_CACHE_on_recvd_from_GetReferrers);
                        d4_uplink_mode(y.first,e->visible_name_of_pinger);
                        return true;
                    }
                }
            }
            for(auto& x: uplinkConnectionState->ponged_all)
            {
                for(auto& y: x.second)
                {
                    if(isCaps(y.first->remote_name))
                    {
//#ifdef KALLREMOVEAFTERDEBUG
                        if(y.second.inaddr()!=e->visible_name_of_pinger.inaddr())
                            throw CommonError("if(y.second!=e->visible_name_of_pinger) %s %s",y.second.dump().c_str(),e->visible_name_of_pinger.dump().c_str());
//#endif
                        _stop_alarm(T_020_D31_wait_after_send_PT_CACHE_on_recvd_from_GetReferrers);
                        d4_uplink_mode(y.first,e->visible_name_of_pinger);
                        return true;
                    }
                }
            }
        }
        {
            DBG(log(TRACE_log,"d2_on_pong_PT_CACHED %s",esi->remote_name.dump().c_str()));
        }

        return true;

    }



    case dfsReferrer::PingType::PT_CAPS_SHORT:
    case dfsReferrer::PingType::PT_MASTER_SHORT:
        return d6_on_pong_PT_CAPS_SHORT(e->visible_name_of_pinger, esi);

    case dfsReferrer::PingType::PT_CAPS_LONG:
    case dfsReferrer::PingType::PT_MASTER_LONG:
        return d6_on_pong_PT_CAPS_LONG(e->visible_name_of_pinger, esi);
    default:
        log(ERROR_log,"invalid pingType %d %s %d",e->pingType,__FILE__,__LINE__);


    }
    return true;

}
bool Service::on_connectionEstablished(const msockaddr_in& remote_addr)
{
    MUTEX_INSPECTOR;
    S_LOG("on_connectionEstablished");
    //DBG(log(TRACE_log,"sa %s",remote_addr.dump().c_str()));
    stopAlarm(crefTimer::T_001_common_connect_failed,NULL,ListenerBase::serviceId);

    DBG(log(TRACE_log,"bool Service::on_connectionEstablished( ) !!!--- %s",remote_addr.dump().c_str()));

    logErr2("--------- connection established %s",remote_addr.dump().c_str());

    uplinkConnectionState->connectionEstablished=true;
    for(auto &i:m_readyNotificationBackroutes)
    {
        REF_getter<Event::Base> z=new dfsReferrerEvent::NotifyReferrerUplinkIsConnected(remote_addr,poppedFrontRoute(i));
        passEvent(z);
        //logErr2("pass %s",z->dump().c_str());
    }
    uplinkConnectionState->uplink.insert(remote_addr);


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
        d5_start(esi,visibleName);
    }
    else
    {
        on_connectionEstablished(esi->remote_name);
    }



}

void Service::d5_start(const REF_getter<epoll_socket_info>&esi,const msockaddr_in& visibleName)
{
    if(stage==STAGE_D5_CHECK_EXTERNAL)
        return;
    stage=STAGE_D5_CHECK_EXTERNAL;

    //logErr2("d5_start_upnp_check esi %s",esi->remote_name.dump().c_str());
    S_LOG("d5_start");
    MUTEX_INSPECTOR;
    DBG(log(TRACE_log,"sa %s ",esi->remote_name.dump().c_str()));


    on_connectionEstablished(esi->remote_name);
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
                                         iUtils->getNow().get(),
                                         connection_sequence_id,
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
    resetAlarm(crefTimer::T_001_common_connect_failed,NULL,ocookie.asString(),ListenerBase::serviceId);
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
        auto gcid=iInstance->globalCookie();
        sendEvent(caps,ServiceEnum::DFSReferrer,
                  new dfsReferrerEvent::Ping(dfsReferrer::PingType::PT_CACHED, gcid,iUtils->getNow().get(),
                                             connection_sequence_id,
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
    uplinkConnectionState=new _uplinkConnectionState(uplinkConnectionState->saCapsFromConfig);
    MUTEX_INSPECTOR;
    S_LOG("d2_start");
    DBG(log(TRACE_log,"d2_start"));

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
                                                 iUtils->getNow().get(),
                                                 connection_sequence_id,
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



    {
        std::set<msockaddr_in> sas=iInstance->myExternalAddressesGet();
        if(sas.size()==0)
            throw CommonError("if(sas.size()==0)");

        DBG(log(TRACE_log,"dfsCapsEvent::GetReferrersREQ "));

        REF_getter<Event::Base> ze=new dfsCapsEvent::GetReferrersREQ(sas,ListenerBase::serviceId);
        sendToplinkReqClient(sa,new ToplinkDeliverREQ(ServiceEnum::DFSCaps,ze.operator ->(),ListenerBase::serviceId));
        _reset_alarm(T_002_D3_caps_get_service_request_is_timed_out);
    }
    reset_T_001_common_connect_failed();

}

bool Service::on_InitClient(const dfsReferrerEvent::InitClient *e)
{
    uplinkConnectionState=new _uplinkConnectionState(e->caps);
    STAGE_D2_PING_NEIGHBOURS_start();
    return true;
}
