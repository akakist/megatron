#include <string>
#include "ITests.h"
#include "IUtils.h"
#include <fstream>
#include "version_mega.h"
#include "listenerBuffered.h"
#include "broadcaster.h"
#include "main/CInstance.h"
#include "Events/System/Run/startService.h"
#include "tools_mt.h"
#include "../CapsAlgorithm.h"
#include "IGEOIP.h"
//#include "../CapsGeoIP.h"
#include "DBH.h"
#include <math.h>

namespace ServiceEnum {
    const SERVICE_id CapsTest("CapsTest");
}
namespace testEventEnum {
}





class CapsTest: public ITests::Base
{
public:
    int zzz;
    void run();
    CapsAlgorithm algo;
//    CapsGeoIP geoip;
    CapsTest() {
        zzz=100;
    }
    ~CapsTest() {}
    static ITests::Base* construct()
    {
        XTRY;
        return new CapsTest;
        XPASS;
    }



};
static bool getFileContent(std::string fileName, std::vector<std::string> & vecOfStrs)
{

    // Open the File
    std::ifstream inF(fileName.c_str(), std::ios_base::in);

    // Check if object is valid
    if(!inF)
    {
        std::cerr << "Cannot open the File : "<<fileName<<std::endl;
        return false;
    }

    std::string str;
    // Read the next line from File untill it reaches the end.
    while (std::getline(inF, str))
    {
        // Line contains string of length > 0 then save it in vector
        if(str.size() > 0)
            vecOfStrs.push_back(str);
    }
    //Close The File
    inF.close();
    return true;
}
typedef double real;
#define pi 3.14159265358979323846
real deg2rad(real deg) {
    return (deg * pi / 180);
}
real rad2deg(real rad) {
    return (rad * 180 / pi);
}
real distance(real lat1, real lon1, real lat2, real lon2, char unit) {
    real theta, dist;
    theta = lon1 - lon2;
    dist = sin(deg2rad(lat1)) * sin(deg2rad(lat2)) + cos(deg2rad(lat1)) * cos(deg2rad(lat2)) * cos(deg2rad(theta));
    dist = acos(dist);
    dist = rad2deg(dist);
    dist = dist * 60 * 1.1515;
    switch(unit) {
    case 'M':
        break;
    case 'K':
        dist = dist * 1.609344;
        break;
    case 'N':
        dist = dist * 0.8684;
        break;
    }
    return (dist);
}
void CapsTest::run()
{

    logErr2("RUN TEST %s",__PRETTY_FUNCTION__);

    IInstance *instance1=iUtils->createNewInstance();
       ConfigObj *cnf1=new ConfigObj("imsfTestServiceTop",
                                     "\nRPC.ConnectionActivity=600.000000"
                                     "\nRPC.IterateTimeoutSec=60.000000"
                                     "\nRPC.ListenerBuffered.MaxThreadsCount=10"
                                     "\nStart=RPC,DFSReferrer,IMSFTop,IMSFNode"
                                     "\nOscarSecure.ListenerBuffered.MaxThreadsCount=10"
                                     "\nOscarSecure.RSA_keysize=256"
                                     "\nOscarSecure.maxPacketSize=33554432"
                                     "\nRPC.BindAddr_MAIN=INADDR_ANY:2000"
                                     "\nRPC.BindAddr_RESERVE=NONE"
                                     "\nSocketIO.ListenerBuffered.MaxThreadsCount=10"
                                     "\nSocketIO.listen_backlog=128"
                                     "\nSocketIO.maxOutBufferSize=8388608"
                                     "\nSocketIO.size=1024"
                                     "\nSocketIO.timeout_millisec=10"
                                     "\nWebHandler.bindAddr=NONE"
//                                     "\nDFSReferrer.CapsServerUrl=" HOST ":2000"
                                     "\nDFSReferrer.IsCapsServer=true"
                                     "\nDFSReferrer.T_001_common_connect_failed=20.000000"
                                     "\nDFSReferrer.T_002_D3_caps_get_service_request_is_timed_out=15.000000"
                                     "\nDFSReferrer.T_004_cache_pong_timed_out_=2.000000"
                                     "\nDFSReferrer.T_007_D6_resend_ping_caps_short=7.000000"
                                     "\nDFSReferrer.T_008_D6_resend_ping_caps_long=20.000000"
                                     "\nDFSReferrer.T_009_pong_timed_out_caps_long=40.000000"
                                     "\nDFSReferrer.T_011_downlink_ping_timed_out=60.000000"
                                     "\nDFSReferrer.T_012_reregister_referrer=3500.000000"
                                     "\nDFSReferrer.T_019_D5_external_connection_check_timeout=22.000000"
                                     "\nDFSReferrer.T_020_D3_1_wait_after_send_PT_CACHE_on_recvd_from_GetService=7.000000"
                                     "\nTelnet.BindAddr=NONE"
                                     "\nmysql.database=caps"
                                     "\nmysql.flag="
                                     "\nmysql.host=NULL"
                                     "\nmysql.max_connections=20"
                                     "\nmysql.passwd=NULL"
                                     "\nmysql.port=0"
                                     "\nmysql.sock=NULL"
                                     "\nmysql.start_code=SET NAMES utf8"
                                     "\nmysql.user=root"
                                     "\nimsfTestService1.ListenerBuffered.MaxThreadsCount=10"
                                    );
       instance1->setConfig(cnf1);
//       instance1->initServices();
//    geoip.init(instance1);
       I_GEOIP *geoip=(I_GEOIP*)iUtils->queryIface(Ifaces::GEOIP);


    std::string ip="158.58.130.235";
    geoNetRec recMy;
    logErr2("before findNetRec");
    if( geoip->findNetRec(instance1,ip,true,recMy) )
    {
        logErr2("ip %s %lf %lf",ip.c_str(),recMy.lat,recMy.lon);
    }
    else
    {
        logErr2("ip %s not found",ip.c_str());
    }



    /// fill referrers from proxy list
    if(1){
        std::vector<std::string> vip;
        std::string fn="/Users/s.belyalov/Downloads/geoip_test_data/zip";
        if(!getFileContent(fn,vip))
        {
            logErr2("cannot read file %s",fn.c_str());
            return;
        }
        for(auto& z:vip)
        {
            bool isAl=false;
            for(size_t i=0;i<z.size();i++)
            {
                if(isalpha(z[i]))
                {
                    isAl=true;
                    break;
                }
            }
            if(isAl)
                continue;
            msockaddr_in a;
            try{
    //            logErr2("z %s",z.c_str());
                a.initFromUrl(z);
    //        logErr2("a %s",a.dump().c_str());
    //            algo.addReferrer();

                geoNetRec recZ;
                if( geoip->findNetRec(instance1,a.getStringAddr(),true,recZ) )
                {
    //                logErr2("ip %s %lf %lf",a.getStringAddr().c_str(),recZ.lat,recZ.lon);
                }
                else
                {
                    logErr2("ip %s not found",ip.c_str());
                }
                algo.addReferrer(recZ.lat,recZ.lon,a,0);
    //            algo.findReferrers()

            }catch(std::exception& e)
            {
                logErr2("exc: %s",e.what());
            }
        }
    }

    logErr2("before findReferrers");
    std::vector<REF_getter<referrerItem> > ret;
    ret=algo.findReferrers(recMy.lat,recMy.lon);

    for(auto &z: ret)
    {
        logErr2("found %s %lf %lf",z->sa.dump().c_str(),z->lat,z->lon);
    }
    if(ret.size())
    {
        double dist=distance((*ret.begin())->lat,(*ret.begin())->lon,recMy.lat,recMy.lon,'K');
        double dist1=distance((*ret.rbegin())->lat,(*ret.rbegin())->lon,recMy.lat,recMy.lon,'K');

        logErr2("min distance in selected set %lf KM",dist);
        logErr2("max distance in selected set %lf KM",dist1);
    }


//    testIoBuffer();
//    testEncoding();
    outBuffer o;
    o<<algo.rcontainers[time(NULL)/3600].referrers.size();
    for(auto& z:algo.rcontainers[time(NULL)/3600].referrers)
    {
        z.second->pack(o);
    }
    std::string s=o.asString()->asString();

    logErr2("referrers body size %d",s.size());
    logErr2("caps test finished");

}


void registerCapsTest(const char* pn)
{
    if(pn)
    {

        iUtils->registerPlugingInfo(COREVERSION,pn,IUtils::PLUGIN_TYPE_TEST,ServiceEnum::CapsTest,"CapsTest");

    }
    else
    {
        iUtils->registerITest(COREVERSION,ServiceEnum::CapsTest,CapsTest::construct);

    }
}
