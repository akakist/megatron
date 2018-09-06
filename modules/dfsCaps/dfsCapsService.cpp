
#include "dfsCapsService.h"
#include <stdarg.h>
#include <algorithm>
#include "IGEOIP.h"
#include "mutexInspector.h"
#include <iostream>
#include <fstream>
#include <ios>
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
#include "sqlite3Wrapper.h"

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
    dbname=_ifa->getConfig()->get_string("dbname","capsDb","db caps name");
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
        const dfsCapsEvent::GetReferrersREQ * E_getReferrersReq=(const dfsCapsEvent::GetReferrersREQ *)z.operator ->();
        if(E_getReferrersReq->externalListenAddr.size()==0)
        {
            logErr2("if(E->externalListenAddr.size()==0) %s",__func__);
            return false;
        }
        geoNetRec res;
        I_GEOIP *geoip=(I_GEOIP*)iUtils->queryIface(Ifaces::GEOIP);
        std::vector<msockaddr_in> sv;
        for(auto& z: E_getReferrersReq->externalListenAddr)
        {
            if(geoip->findNetRec(iInstance,z.getStringAddr(),z.family()==AF_INET,res))
            {
                auto ret=algo.findReferrers(res.lat,res.lon);
                for(auto &x:ret)
                {
                    sv.push_back(x->sa);
                }

                passEvent(new dfsReferrerEvent::ToplinkDeliverRSP(new dfsCapsEvent::GetReferrersRSP(sv,E_getReferrersReq->route),poppedFrontRoute(e_toplink->route)));
                return true;

                return true;
            }

        }
        return false;
    }
    if(z->id==dfsCapsEventEnum::RegisterMyRefferrerREQ)
    {

//        S_LOG("dfsCapsEventEnum::RegisterMyRefferrerREQ");

        const dfsCapsEvent::RegisterMyRefferrerREQ * e_RegisterMyRefferrerREQ=(const dfsCapsEvent::RegisterMyRefferrerREQ *)z.operator ->();
        if(e_RegisterMyRefferrerREQ->externalListenAddr.size())
        {
            for(auto z:e_RegisterMyRefferrerREQ->externalListenAddr)
            {
                I_GEOIP *geoip=(I_GEOIP*)iUtils->queryIface(Ifaces::GEOIP);
                geoNetRec res;
                if(geoip->findNetRec(iInstance,z.getStringAddr(),z.family()==AF_INET,res))
                {
                    algo.addReferrer(res.lat,res.lon,z,e_RegisterMyRefferrerREQ->downlinkCount);
                }
            }

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
//    Sqlite3Wrapper d(dbname);
//    std::vector<std::string> v=d.select_1_column((QUERY)"SELECT * FROM sqlite_master");
//    std::set<std::string> s;
//    for(auto z:v)s.insert(z);
//    if(!s.count("city_block4"))
//    {
//        d.execSimple((QUERY)""
//                     "create table if not exists city_block4"
//                     "("
//                         "network blob,"
//                         "geoname_id blob,"
//                        " registered_country_geoname_id blob,"
//                         "represented_country_geoname_id blob,"
//                         "is_anonymous_proxy blob,"
//                         "is_satellite_provider blob,"
//                         "postal_code blob,"
//                         "latitude blob,"
//                         "longitude blob,"
//                         "accuracy_radius blob"
//                     ")"
//                     );
//    }
//    logErr2(".tables ->");
//    for(auto z:v)
//    {
//        logErr2(".tables -> %s",z.c_str());

//    }
/*
drop table if exists city_block4;
create table if not exists city_block4
(
    network blob,
    geoname_id blob,
    registered_country_geoname_id blob,
    represented_country_geoname_id blob,
    is_anonymous_proxy blob,
    is_satellite_provider blob,
    postal_code blob,
    latitude blob,
    longitude blob,
    accuracy_radius blob
);
.mode csv
.separator ','
.import /Users/s.belyalov/Downloads/GeoLite2-City-CSV_20180605/GeoLite2-City-Blocks-IPv4.csv city_block4;
alter table city_block4 add column start_ip blob;
alter table city_block4 add column end_ip blob;
CREATE INDEX IF NOT EXISTS start_ip_idx ON city_block4(start_ip) ;
CREATE INDEX IF NOT EXISTS end_ip_idx ON city_block4(end_ip) ;
*/

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
