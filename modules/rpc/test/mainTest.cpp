#include "IInstance.h"
#include "colorOutput.h"
#include "configObj.h"
#include "version_mega.h"
#include "rpcTestService1.h"
#include "rpcTestService2.h"
#include "testRpcEvent.h"
#include "CUtils.h"
bool done_test=false;
void registerRPCService(const char* pn);
void registerSocketModule(const char* pn);
void registerTimerService(const char* pn);
void registerSSL(const char* pn);
void registerOscarModule(const char* pn);
void registerOscarSecureModule(const char* pn);
int mainTest(int argc, char** argv )
{
    try {
        iUtils=new CUtils(argc, argv, "rpcTest");
//        iUtils->registerITest(COREVERSION,ServiceEnum::rpcTest,rpcTest::construct);
        iUtils->registerService(COREVERSION,ServiceEnum::rpcTestService1,rpcTestService1::construct,"rpcTestService1");
        iUtils->registerService(COREVERSION,ServiceEnum::rpcTestService2,rpcTestService2::construct,"rpcTestService2");
        iUtils->registerEvent(testEvent::testREQ::construct);
        iUtils->registerEvent(testEvent::testRSP::construct);

        registerRPCService(NULL);
        registerSocketModule(NULL);
        registerTimerService(NULL);
        registerSSL(NULL);
        registerOscarModule(NULL);
        registerOscarSecureModule(NULL);

        printf(GREEN("RUN TEST %s"),__PRETTY_FUNCTION__);

        {
            IInstance *instance1=iUtils->createNewInstance("rpcTestService1");
            ConfigObj *cnf1=new ConfigObj("rpcTestService1",
                                          "\nRPC.ConnectionActivity=600.000000"
                                          "\nRPC.IterateTimeoutSec=60.000000"
                                          "\nRPC.ListenerBuffered.MaxThreadsCount=10"
                                          "\nStart=RPC,rpcTestService1"
                                          "\nOscarSecure.ListenerBuffered.MaxThreadsCount=10"
                                          "\nOscarSecure.RSA_keysize=256"
                                          "\nOscarSecure.maxPacketSize=33554432"
                                          "\nRPC.BindAddr_MAIN=INADDR_ANY:2000"
                                          "\nRPC.BindAddr_RESERVE=NONE"
                                          "\nSocketIO.ListenerBuffered.MaxThreadsCount=10"
                                          "\nSocketIO.listen_backlog=128"
                                          "\nSocketIO.size=1024"
                                          "\nSocketIO.timeout_millisec=10"
                                          "\nWebHandler.bindAddr=NONE"
                                          "\nrpcTestService1.ListenerBuffered.MaxThreadsCount=10"
                                          "\nOscar.maxPacketSize=33554432"
                                          "\nSocketIO.epoll_timeout_millisec=1000"

                                          "\n# socket poll thread count"
                                          "\nSocketIO.n_workers=2"
                                         );
            instance1->setConfig(cnf1);
            instance1->initServices();
        }
        {
            IInstance *instance2=iUtils->createNewInstance("rpcTestService2");
            ConfigObj *cnf2=new ConfigObj("rpcTestService2",
                                          "\nRPC.ConnectionActivity=600.000000"
                                          "\nRPC.IterateTimeoutSec=60.000000"
                                          "\nRPC.ListenerBuffered.MaxThreadsCount=10"
                                          "\nStart=RPC,rpcTestService2"
                                          "\nOscarSecure.ListenerBuffered.MaxThreadsCount=10"
                                          "\nOscarSecure.RSA_keysize=256"
                                          "\nOscarSecure.maxPacketSize=33554432"
                                          "\nRPC.BindAddr_MAIN=INADDR_ANY:2001"
                                          "\nRPC.BindAddr_RESERVE=NONE"
                                          "\nSocketIO.ListenerBuffered.MaxThreadsCount=10"
                                          "\nSocketIO.listen_backlog=128"
                                          "\nSocketIO.size=1024"
                                          "\nSocketIO.timeout_millisec=10"
                                          "\nWebHandler.bindAddr=NONE"
                                          "\nrpcTestService2.ListenerBuffered.MaxThreadsCount=10"
                                          "\nOscar.maxPacketSize=33554432"
                                          "\nSocketIO.epoll_timeout_millisec=1000"

                                          "\n# socket poll thread count"
                                          "\nSocketIO.n_workers=2"

                                         );
            instance2->setConfig(cnf2);
            instance2->initServices();
        }

        while(!done_test)
        {
            usleep(10000);
        }
        delete iUtils;
        return 0;

    } catch (CommonError& e)
    {
        printf("CommonError %s\n",e.what());
    }
    return 1;
}
