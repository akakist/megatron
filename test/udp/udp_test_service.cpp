#include <string>
#include "ITests.h"
#include "IUtils.h"
#include "version_mega.h"
#include "listenerBuffered.h"
#include "broadcaster.h"
#include "main/CInstance.h"
#include "Events/System/Run/startService.h"
#include "tools_mt.h"
#include "Events/System/timer/TickAlarm.h"
#include "Events/System/timer/TickTimer.h"
#include "Events/System/Net/socket/UdpAssoc.h"
#include "colorOutput.h"
#include "main/configObj.h"
#include "ISSL.h"
static bool done_test=false;
namespace ServiceEnum {
    const SERVICE_id udpTest("udpTest");
    const SERVICE_id udpTestService1("udpTestService1");
    const SERVICE_id udpTestService2("udpTestService2");
}
namespace testEventEnum {
}
enum timers
{
    TI_START,
    TI_ACTIVITY
};

class udpTestService1: public UnknownBase, public ListenerBuffered, public Broadcaster
{
public:
    I_ssl *ssl;
    void deinit()
    {
        ListenerBuffered::deinit();
    }

    static UnknownBase* construct(const SERVICE_id& id, const std::string&  nm,IInstance* ifa)
    {
        XTRY;
        return new udpTestService1(id,nm,ifa);
        XPASS;
    }
    udpTestService1(const SERVICE_id &id, const std::string&  nm,IInstance* ins):
        UnknownBase(nm),
        ListenerBuffered(nm,ins->getConfig(),id,ins),
        Broadcaster(ins)
    {
        ssl=(I_ssl*)iUtils->queryIface(Ifaces::SSL);

    }
    bool handleEvent(const REF_getter<Event::Base>& e)
    {
        if(done_test) return true;
        MUTEX_INSPECTOR;
        auto& ID=e->id;
        if(systemEventEnum::startService==ID)
        {
            MUTEX_INSPECTOR;
            SOCKET_id sid4=iUtils->getSocketId();
            SOCKET_id sid6=iUtils->getSocketId();
            msockaddr_in sa4;
            sa4.init("INADDR_ANY",10011);
            msockaddr_in sa6;
            sa4.init("INADDR6_ANY",10011);
            sendEvent(ServiceEnum::Socket,new socketEvent::UdpAssoc(
                          sid4,
                          sid6,
                          sa4,
                          sa6,
                          "UDP",ListenerBase::serviceId));

            return true;
        }
        if(socketEventEnum::UdpAssocRSP==ID)
        {
            socketEvent::UdpAssocRSP* ee=(socketEvent::UdpAssocRSP*)e.operator ->();
            logErr2("UdpAssocRSP errcode %d",ee->errcode);
            return true;
        }
        if(socketEventEnum::UdpPacketIncoming==ID)
        {
            socketEvent::UdpPacketIncoming* ee=(socketEvent::UdpPacketIncoming*)e.operator ->();
            logErr2("UdpPacketIncoming from %s data %s",ee->addrFrom.dump().c_str(),ee->data.c_str());
            printf(GREEN("UDP test passed OK"));
            done_test=true;
            return true;
        }
        return false;
    }
};
class udpTestService2: public UnknownBase, public ListenerBuffered, public Broadcaster
{
public:
    I_ssl *ssl;

    int udpSock;
    void deinit()
    {
        ListenerBuffered::deinit();
    }

    static UnknownBase* construct(const SERVICE_id& id, const std::string&  nm,IInstance* ifa)
    {
        XTRY;
        return new udpTestService2(id,nm,ifa);
        XPASS;
    }
    udpTestService2(const SERVICE_id &id, const std::string&  nm,IInstance* ins):
        UnknownBase(nm),
        ListenerBuffered(nm,ins->getConfig(),id,ins),
        Broadcaster(ins)
    {
        ssl=(I_ssl*)iUtils->queryIface(Ifaces::SSL);

    }
    bool handleEvent(const REF_getter<Event::Base>& e)
    {
        if(done_test) return true;
        MUTEX_INSPECTOR;
        logErr2("@%s",__PRETTY_FUNCTION__);
        auto& ID=e->id;
        if(systemEventEnum::startService==ID)
        {
            udpSock=::socket(PF_INET,SOCK_DGRAM,0);
            if(udpSock<0)
            {
                logErr2("socket: %s",strerror(errno));
            }

            sendEvent(ServiceEnum::Timer,new timerEvent::SetTimer(1,NULL,NULL,0.3,ListenerBase::serviceId));
            return true;
        }
        if(timerEventEnum::TickTimer==ID)
        {
            timerEvent::TickTimer* ee=(timerEvent::TickTimer*)e.operator ->();
            if(ee->tid==1)
            {
                std::string pk="asdasdfasdfasdfasdfasdf";
                msockaddr_in sa;
                sa.init("127.0.0.1",10011);
                int r=sendto(udpSock,pk.data(),pk.size(),0,sa.addr(),sa.addrLen());
                if(r<0)
                {
                    logErr2("sendto: %s",strerror(errno));
                    return true;
                }
                logErr2("sendto success");
                return true;
            }
            return true;
        }

        return false;
    }
};




class udpTest: public ITests::Base
{
public:
    int zzz;
    int run();
    std::string getName()
    {
        return "udpTest";
    }

    udpTest() {
        zzz=100;
    }
    ~udpTest() {}
    static ITests::Base* construct()
    {
        XTRY;
        return new udpTest;
        XPASS;
    }



};

int udpTest::run()
{

    printf(GREEN("RUN TEST %s"),__PRETTY_FUNCTION__);

    {
        IInstance *instance1=iUtils->createNewInstance("udpTestService1");
        ConfigObj *cnf1=new ConfigObj("udpTestService1",
                                      "\nStart=udpTestService1"
                                      "\nSocketIO.ListenerBuffered.MaxThreadsCount=10"
                                      "\nSocketIO.listen_backlog=128"
                                      "\nSocketIO.maxOutBufferSize=8388608"
                                      "\nSocketIO.size=1024"
                                      "\nSocketIO.timeout_millisec=10"
                                      "\nWebHandler.bindAddr=NONE"
                                      "\nudpTestService1.ListenerBuffered.MaxThreadsCount=10"
                                     );
        instance1->setConfig(cnf1);
        instance1->initServices();
    }
    {
        IInstance *instance2=iUtils->createNewInstance("udpTestService2");
        ConfigObj *cnf2=new ConfigObj("udpTestService2",
                                      "\nStart=udpTestService2"
                                      "\nSocketIO.ListenerBuffered.MaxThreadsCount=10"
                                      "\nSocketIO.listen_backlog=128"
                                      "\nSocketIO.maxOutBufferSize=8388608"
                                      "\nSocketIO.size=1024"
                                      "\nSocketIO.timeout_millisec=10"
                                      "\nWebHandler.bindAddr=NONE"
                                      "\nudpTestService2.ListenerBuffered.MaxThreadsCount=10"
                                     );
        instance2->setConfig(cnf2);
        instance2->initServices();
    }


    while(!done_test)
    {
        usleep(100000);
    }
    return 0;
}


void registerUDPTest(const char* pn)
{
    if(pn)
    {

        iUtils->registerPlugingInfo(COREVERSION,pn,IUtils::PLUGIN_TYPE_TEST,ServiceEnum::udpTest,"udpTest",std::set<EVENT_id>());
        iUtils->registerPlugingInfo(COREVERSION,pn,IUtils::PLUGIN_TYPE_SERVICE,ServiceEnum::udpTestService1,"udpTestService1",std::set<EVENT_id>());
        iUtils->registerPlugingInfo(COREVERSION,pn,IUtils::PLUGIN_TYPE_SERVICE,ServiceEnum::udpTestService2,"udpTestService2",std::set<EVENT_id>());

    }
    else
    {
        iUtils->registerITest(COREVERSION,ServiceEnum::udpTest,udpTest::construct);
        iUtils->registerService(COREVERSION,ServiceEnum::udpTestService1,udpTestService1::construct,"udpTestService1");
        iUtils->registerService(COREVERSION,ServiceEnum::udpTestService2,udpTestService2::construct,"udpTestService2");

    }
}
