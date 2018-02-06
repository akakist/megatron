
#include "dfsCapsService.h"
#include <stdarg.h>
#include "mutexInspector.h"

#include "version_mega.h"
#include "VERSION_id.h"

#include <math.h>
#include "dfsErrors.h"
#include <algorithm>

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
    ,_ifa       ),TimerHelper(_ifa),iInstance(_ifa),
    m_createNetBlocks(_ifa->getConfig()->get_bool("createNetBlocks",false,"create netblocks file from GeoLiteCity")),
    m_pn_netBlocks(_ifa->getConfig()->get_string("netblocks",
                   iUtils->expand_homedir(CACHE_TARGET_DIR) + "/netblocks"

                   ,"netblocks pathname"))
{
    MUTEX_INSPECTOR;
    try {
        if(m_createNetBlocks)
        {
            m_GeoLiteCity_Location=_ifa->getConfig()->get_string("GeoLiteCity_Location",iUtils->expand_homedir(DATA_TARGET_DIR) + "/GeoLiteCity-Location.csv","Geoip database taken from www.maxmind.com");
            m_GeoLiteCity_Blocks=_ifa->getConfig()->get_string("GeoLiteCity_Blocks",iUtils->expand_homedir(DATA_TARGET_DIR) + "/GeoLiteCity-Blocks.csv","Geoip database taken from www.maxmind.com");
        }
    }
    catch(std::exception& e)
    {
        logErr2("e %s",e.what());
    }
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
        int32_t ip=E_getReferrersReq->externalListenAddr.begin()->inaddr();
        std::set<msockaddr_in>sas2=rootTreeNode[time(NULL)/3600].getIpListFromTree(ip,netgen.calcTreeRoute(ip));
        for(auto& x:sas2)
        {
            DBG(log(TRACE_log,"sa %s",x.dump().c_str()));
        }
        passEvent(new dfsReferrerEvent::ToplinkDeliverRSP(new dfsCapsEvent::GetReferrersRSP(sas2,E_getReferrersReq->route),poppedFrontRoute(e_toplink->route)));
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
            msockaddr_in sa=*e_RegisterMyRefferrerREQ->externalListenAddr.begin();
            DBG(log(TRACE_log,"sa %s",sa.dump().c_str()));
            auto tr=netgen.calcTreeRoute(sa.inaddr());
            rootTreeNode[time(NULL)/3600].addIpToTree(sa,tr);
            rootTreeNode[time(NULL)/3600+1].addIpToTree(sa,tr);
            if(rootTreeNode.size()>3)
                rootTreeNode.erase(rootTreeNode.begin());
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


    if(!m_createNetBlocks)
    {
        netgen.load(m_pn_netBlocks);
        return true;
    }


    Sqlite3Wrapper dbh(CAPS_DBNAME);

    log(INFO_log,"creating netblocks");
    bool skip=false;
    REF_getter<QueryResult> rq(dbh.exec((std::string)"SELECT name from sqlite_master"));
    for(size_t i=0; i<rq->values.size(); i++)
    {
        if(rq->values[i].size())
        {
            if(rq->values[i][0]=="tbl_caps_block") skip=true;
        }
    }
    if(!skip)
    {
        
        std::vector<NetRec> locvec;
        log(INFO_log,"importing GeoLiteCity-Blocks.csv");
        {
            FILE *f=fopen(m_GeoLiteCity_Blocks.c_str(),"r");
            if(!f)
            {
                fprintf(stderr,"error open %s %s",m_GeoLiteCity_Blocks.c_str(),strerror(errno));
                exit(1);
            }
            char s[200];
            char* stmp=fgets(s,sizeof(s),f);
            stmp=fgets(s,sizeof(s),f);
            dbh.execSimple((std::string)"DROP TABLE IF EXISTS tbl_caps_block");
            dbh.execSimple((std::string)"CREATE TABLE  IF NOT EXISTS tbl_caps_block (startIPNum INTEGER unsigned NOT NULL, endIPNum INTEGER unsigned NOT NULL,locId INTEGER unsigned NOT NULL, id INTEGER PRIMARY KEY)");
            dbh.execSimple((std::string)"CREATE INDEX IF NOT EXISTS idx_locId ON tbl_caps_block(locId)");
            dbh.execSimple((std::string)"CREATE INDEX IF NOT EXISTS idx_start_ip ON tbl_caps_block(startIPNum)");
            dbh.execSimple((std::string)"CREATE INDEX IF NOT EXISTS idx_end_ip ON tbl_caps_block(endIPNum)");

            //dbh.execSimple((std::string)"BEGIN TRANSACTION");
            int i=0;
            while(!feof(f))
            {
                char *ss=fgets(s,sizeof(s),f);
                if(ss)
                {
                    dbh.execSimple((std::string)"INSERT INTO tbl_caps_block (startIPNum, endIPNum,locId) VALUES("+ss+")");
                    if(i++%1000==0)
                        printf("\rtbl_caps_block %d                 ",i);
                }
            }
            //dbh.execSimple((std::string)"COMMIT");
            fclose(f);
        }
        {
            FILE *f=fopen(m_GeoLiteCity_Location.c_str(),"r");
            if(!f)
            {
                fprintf(stderr,"error open %s %s",m_GeoLiteCity_Location.c_str(),strerror(errno));

                exit(1);
            }
            char s[200];
            fgets(s,sizeof(s),f);
            fgets(s,sizeof(s),f);
            dbh.execSimple((std::string)"DROP TABLE IF EXISTS tbl_caps_location");
            dbh.execSimple((std::string)"CREATE TABLE  IF NOT EXISTS tbl_caps_location (locId INTEGER, country BLOB,region BLOB,city BLOB,postalCode BLOB,latitude REAL,longitude REAL,metroCode INTEGER,areaCode INTEGER)");
            dbh.execSimple((std::string)"CREATE INDEX IF NOT EXISTS idx_locId2 ON tbl_caps_location(locId)");
            dbh.execSimple((std::string)"CREATE INDEX IF NOT EXISTS idx_country ON tbl_caps_location(country)");
            dbh.execSimple((std::string)"CREATE INDEX IF NOT EXISTS idx_region ON tbl_caps_location(region)");
            dbh.execSimple((std::string)"CREATE INDEX IF NOT EXISTS idx_city ON tbl_caps_location(city)");

            dbh.execSimple((std::string)"BEGIN TRANSACTION");
            int i=0;
            while(!feof(f))
            {
                char *ss=fgets(s,sizeof(s),f);
                if(ss)
                {
                    dbh.execSimple((std::string)"INSERT INTO tbl_caps_location (locId,country,region,city,postalCode,latitude,longitude,metroCode,areaCode) VALUES("+cvt(ss)+")");
                    if(i++%1000==0)
                        printf("\rtbl_caps_location %d             ",i);
                }
            }
            //dbh.execSimple((std::string)"COMMIT");
            fclose(f);
        }
    }
    ///
    {
        

        std::set<NetRec> netArray;

        REF_getter<QueryResult> cRes=dbh.exec((std::string)"SELECT country FROM tbl_caps_location group by country");
        netgen.m_netArray.reserve(atoi(dbh.select_1((std::string)"select count(*) from tbl_caps_block").c_str()));

        printf("\n");
        for(size_t icountry=0; icountry<cRes->values.size(); icountry++)
        {
            if(cRes->values[icountry].size()==1)
            {
                std::string country=cRes->values[icountry][0];
                REF_getter<QueryResult> lRes=dbh.exec((QUERY)"SELECT locid FROM tbl_caps_location where country='?'"<<country);
                for(size_t ilocation=0; ilocation<lRes->values.size(); ilocation++)
                {
                    if(lRes->values[ilocation].size()==1)
                    {
                        int locId=atoi(lRes->values[ilocation][0].c_str());

                        REF_getter<QueryResult> nRes=dbh.exec((QUERY)"SELECT startIPNum, endIPNum FROM tbl_caps_block where locId='?'"<<locId);
                        {
                            for(size_t inetnum=0; inetnum<nRes->values.size(); inetnum++)
                            {
                                if(nRes->values[inetnum].size()==2)
                                {
                                    NetRec nr;
                                    nr.startIp=atoi(nRes->values[inetnum][0].c_str());
                                    nr.endIp=atoi(nRes->values[inetnum][1].c_str());
                                    nr.countryPos=icountry;
                                    nr.countryMax=cRes->values.size();
                                    nr.locationPos=ilocation;
                                    nr.locationMax=lRes->values.size();
                                    nr.netnumPos=inetnum;
                                    nr.netnumMax=nRes->values.size();
                                    netArray.insert(nr);
                                    printf("%s     netArray size %ld     \r",country.c_str(),netArray.size());
                                }
                            }
                        }
                    }
                }
            }
        }

        printf("\n");


        st_FILE f("/tmp/netblocks","wb");
        outBuffer o;
        o<<netArray;

        REF_getter<refbuffer> oo=o.asString();
        fwrite(oo->buffer,1, oo->size,f.f);

        netgen.m_netArray.reserve(netArray.size());
        for(auto &i:netArray)
        {
            netgen.m_netArray.push_back(i);
        }
    }
    logErr2("Done, netblocks written to /tmp/netblocks, exec 'sudo cp /tmp/netblocks %s/netblocks'", iUtils->expand_homedir(CACHE_TARGET_DIR).c_str());
    iUtils->setTerminate();

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
/*#ifdef __linux__
#ifndef isnan
double isnan(double x)
{
    return std::isnan(x);
}
#endif
#endif*/
double dfsCaps::Service::Distance(const std::pair<double,double> & c1,const std::pair<double,double> & c2)
{
    MUTEX_INSPECTOR;
    double	radius = 6372795;
    double lat1=c1.first*M_PI/180;
    double lon1=c1.second*M_PI/180;
    double lat2=c2.first*M_PI/180;
    double lon2=c2.second*M_PI/180;

    double acos_rng = sin(lat1) * sin(lat2) + cos(lat1) * cos(lat2) * cos(lon2 - lon1);
    if (acos_rng >= 1. || isnan(acos_rng))
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
