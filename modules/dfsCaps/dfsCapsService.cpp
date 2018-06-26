
#include "dfsCapsService.h"
#include <stdarg.h>
#include "mutexInspector.h"

#include "version_mega.h"
#include "VERSION_id.h"

#include <math.h>
#include "dfsErrors.h"
#include <algorithm>
#include <cmath>

#include <sys/types.h>
#include <sys/stat.h>
#include "st_malloc.h"
#include "st_FILE.h"
#include "Events/Tools/webHandler/RegisterHandler.h"
#include "Events/Tools/webHandler/RegisterDirectory.h"
#include "Events/System/Net/rpc/IncomingOnConnector.h"
#include "Events/System/timer/TickTimer.h"

#include "events_dfsCaps.hpp"
#include "megatron_config.h"

#define CAPS_DBNAME "caps"
//using namespace std;
dfsCaps::Service::Service(const SERVICE_id &svs, const std::string&  nm, IInstance *_ifa):
    UnknownBase(nm),
    ListenerBuffered1Thread(this,nm,_ifa->getConfig(),svs,_ifa),
    Broadcaster(_ifa),

    Logging(this,
#ifdef DEBUG
            TRACE_log
#else
            INFO_log
#endif
    ,_ifa       ),TimerHelper(_ifa),iInstance(_ifa)
{
    MUTEX_INSPECTOR;
}
dfsCaps::Service::~Service() {

}
UnknownBase* dfsCaps::Service::construct(const SERVICE_id& id, const std::string&  nm, IInstance *_ifa)
{
    return new Service(id,nm,_ifa);
}
void registerDFSCapsService(const char* pn)
{
    if(pn)
    {
        iUtils->registerPlugingInfo(COREVERSION,pn,IUtils::PLUGIN_TYPE_SERVICE,ServiceEnum::DFSCaps,"DFSCaps");
    }
    else
    {
        
        iUtils->registerService(COREVERSION,ServiceEnum::DFSCaps,dfsCaps::Service::construct,"DFSCaps");
        regEvents_dfsCaps();
    }
}
bool dfsCaps::Service::on_ToplinkDeliverREQ(const dfsReferrerEvent::ToplinkDeliverREQ* e_toplink)
{
    S_LOG("on_ToplinkDeliverREQ");
    REF_getter<Event::Base> z=e_toplink->getEvent();
    if(z->id==dfsCapsEventEnum::GetReferrersREQ)
    {
        S_LOG("dfsCapsEventEnum::GetReferrersREQ");
        const dfsCapsEvent::GetReferrersREQ * E_getReferrersReq=(const dfsCapsEvent::GetReferrersREQ *)z.operator ->();
//        int32_t ip=*ips.begin();
        if(E_getReferrersReq->externalListenAddr.size()==0)
        {
            logErr2("if(E->externalListenAddr.size()==0) %s",__func__);
            return false;
        }
        passEvent(new dfsReferrerEvent::ToplinkDeliverRSP(new dfsCapsEvent::GetReferrersRSP(nodes[time(NULL)/3600],E_getReferrersReq->route),poppedFrontRoute(e_toplink->route)));
        return true;

        return true;
    }
    if(z->id==dfsCapsEventEnum::RegisterMyRefferrerREQ)
    {

        S_LOG("dfsCapsEventEnum::RegisterMyRefferrerREQ");

        const dfsCapsEvent::RegisterMyRefferrerREQ * e_RegisterMyRefferrerREQ=(const dfsCapsEvent::RegisterMyRefferrerREQ *)z.operator ->();
        if(e_RegisterMyRefferrerREQ->externalListenAddr.size())
        {
            if(e_RegisterMyRefferrerREQ->externalListenAddr.begin()->isNAT())
            {
                logErr2("TODO !!!!!!!!!!!!!!skip registerHost %s %s",e_RegisterMyRefferrerREQ->externalListenAddr.begin()->dump().c_str(),__func__);
                return true;
            }
            for(auto z:e_RegisterMyRefferrerREQ->externalListenAddr)
            {
                nodes[time(NULL)/3600].insert(z);
                nodes[time(NULL)/3600+1].insert(z);
            }
            if(nodes.size()>3)
                nodes.erase(nodes.begin());
        }


        return true;
    }

    return true;
}

bool dfsCaps::Service::handleEvent(const REF_getter<Event::Base>& e)
{
    S_LOG("dfsCaps::handleEvent");
    MUTEX_INSPECTOR;
    XTRY;
    auto & ID=e->id;
    if(timerEventEnum::TickAlarm==ID)
        return on_TickAlarm((const timerEvent::TickAlarm*)e.operator->());
    if( webHandlerEventEnum::RequestIncoming==ID)
        return on_RequestIncoming((const webHandlerEvent::RequestIncoming*)e.operator->());
    if(systemEventEnum::startService==ID)
        return on_startService((const systemEvent::startService*)e.operator->());
    if( rpcEventEnum::IncomingOnConnector==ID)
    {
        S_LOG("rpcEventEnum::IncomingOnConnector");
        const rpcEvent::IncomingOnConnector* E=(const rpcEvent::IncomingOnConnector*)e.operator ->();
        auto &IDC=E->e->id;
        if(dfsReferrerEventEnum::ToplinkDeliverREQ==IDC)
            return on_ToplinkDeliverREQ((const dfsReferrerEvent::ToplinkDeliverREQ*)E->e.operator ->());

        log(ERROR_log,"unhandled event %s %s %d",E->e->name,__func__,__LINE__);
    }
    if( rpcEventEnum::IncomingOnAcceptor==ID)
    {
        S_LOG("rpcEventEnum::IncomingOnAcceptor");
        const rpcEvent::IncomingOnAcceptor* E=(const rpcEvent::IncomingOnAcceptor*)e.operator ->();
        auto & IDA=E->e->id;
        if(dfsReferrerEventEnum::ToplinkDeliverREQ==IDA)
            return on_ToplinkDeliverREQ((const dfsReferrerEvent::ToplinkDeliverREQ*)E->e.operator ->());

        log(ERROR_log,"unhandled event %s %s %d",E->e->name,__func__,__LINE__);
    }

    if(dfsReferrerEventEnum::ToplinkDeliverREQ==ID)
        return on_ToplinkDeliverREQ((const dfsReferrerEvent::ToplinkDeliverREQ*)e.operator->());

    log(ERROR_log,"unhandled event %s %s %d",e->name,__FILE__,__LINE__);
    XPASS;
    return false;
}

static std::string cvt(const std::string& s)
{
    std::string out;
    for(size_t i=0; i<s.size()-1; i++)
    {
        if(s[i]==',' && (s[i+1]==','||s[i+1]=='\n'||s[i+1]=='\r'))
        {
            out+=",0";
            continue;
        }
        else
            out+=s[i];
    }
    return out;
}
bool  dfsCaps::Service::on_startService(const systemEvent::startService*)
{
    S_LOG("on_startService");
    MUTEX_INSPECTOR;
    XTRY;


    if(iUtils->isServiceRegistered(ServiceEnum::WebHandler))
    {
        sendEvent(ServiceEnum::WebHandler, new webHandlerEvent::RegisterDirectory("dfs","DFS"));
        sendEvent(ServiceEnum::WebHandler, new webHandlerEvent::RegisterHandler("dfs/Caps","Capabilities",ListenerBase::serviceId));
    }



    XPASS;
    return true;
}


bool dfsCaps::Service::on_RequestIncoming(const webHandlerEvent::RequestIncoming* e)
{
    MUTEX_INSPECTOR;
    HTTP::Response cc;
    cc.content+="<h1>DFS Caps report</h1><p>";

    Json::Value v;
    Json::StyledWriter w;
    cc.content+="<pre>\n"+w.write(v)+"\n</pre>";

    cc.makeResponse(e->esi);
    return true;
}
double dfsCaps::Service::Distance(const std::pair<double,double> & c1,const std::pair<double,double> & c2)
{
    MUTEX_INSPECTOR;
    double	radius = 6372795;
    double lat1=c1.first*M_PI/180;
    double lon1=c1.second*M_PI/180;
    double lat2=c2.first*M_PI/180;
    double lon2=c2.second*M_PI/180;

    double acos_rng = sin(lat1) * sin(lat2) + cos(lat1) * cos(lat2) * cos(lon2 - lon1);
    if (acos_rng >= 1. || std::isnan(acos_rng))
        return 0.;
    if (acos_rng <= -1.)
        return M_PI;
    return radius*(acos(acos_rng));
}


bool dfsCaps::Service::on_TickAlarm(const timerEvent::TickAlarm*e)
{

    logErr2("invalid alarm %d",e->tid);

    return true;
}
