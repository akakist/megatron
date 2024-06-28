#include "IInstance.h"
#include "colorOutput.h"
#include "configObj.h"
#include "CUtils.h"
#include <unistd.h>
//bool done_test_http=false;
// void registerRPCService(const char* pn);
void registerSocketModule(const char* pn);
void registerTimerService(const char* pn);
void registerTelnetService(const char* pn);
void registerteldemo1(const char* pn);

int mainTestTELNET(int argc, char** argv )
{
    try {
        iUtils=new CUtils(argc, argv, "telnetTest");

        // registerRPCService(NULL);
        registerSocketModule(NULL);
        registerTimerService(NULL);
        registerTelnetService(NULL);

    registerteldemo1(NULL);

        printf(GREEN("RUN TEST %s"),__PRETTY_FUNCTION__);

        {
            IInstance *instance1=iUtils->createNewInstance("teldemo1");
            ConfigObj *cnf1=new ConfigObj("teldemo1",
                                          "\nStart=teldemo1"
                                          "\nTelnet.BindAddr=127.0.0.1:8081"
                                          "\nTelnet.deviceName=Device"
                                          "\nSocketIO.ListenerBuffered.MaxThreadsCount=10"
                                          "\nSocketIO.listen_backlog=128"
                                          "\nSocketIO.size=1024"
                                          "\nSocketIO.timeout_millisec=10"

                                          "\nSocketIO.epoll_timeout_millisec=2000"
                                          "\n"
                                          "\n# socket poll thread count"
                                          "\nSocketIO.n_workers=3"
                                          "\nOscar.maxPacketSize=33554432"
                                         );
            instance1->setConfig(cnf1);
            instance1->initServices();
        }

#ifdef KALL
        {
            IInstance *instance1=iUtils->createNewInstance("prodtestServerWeb");
            ConfigObj *cnf1=new ConfigObj("prodtestServerWeb",
                                          "\nRPC.ConnectionActivity=600.000000"
                                          "\nRPC.IterateTimeoutSec=60.000000"
                                          "\nRPC.ListenerBuffered.MaxThreadsCount=10"
                                          "\nStart=prodtestServerWeb,RPC"
                                          "\nOscarSecure.ListenerBuffered.MaxThreadsCount=10"
                                          "\nOscarSecure.RSA_keysize=256"
                                          "\nOscarSecure.maxPacketSize=33554432"
                                          "\nRPC.BindAddr_MAIN=INADDR_ANY:0"
                                          "\nRPC.BindAddr_RESERVE=NONE"
                                          "\nSocketIO.ListenerBuffered.MaxThreadsCount=10"
                                          "\nSocketIO.listen_backlog=128"
                                          "\nSocketIO.size=1024"
                                          "\nSocketIO.timeout_millisec=10"
                                          "\nWebHandler.bindAddr=NONE"

                                          "\n# http listen address"
                                          "\ntestHTTP.bindAddr=0.0.0.0:8088"
                                          "\nHTTP.max_post=1000000"
                                          "\nHTTP.doc_urls=/pics,/html,/css"
                                          "\nHTTP.document_root=./www"
                                          "\nSocketIO.epoll_timeout_millisec=2000"
                                          "\n"
                                          "\n# socket poll thread count"
                                          "\nSocketIO.n_workers=4"

                                          "\nOscar.maxPacketSize=33554432"
                                          "\n"
                                          "\n# http listen address:"
                                          "\nprodtestServerWeb.bindAddr=0.0.0.0:8088"
                                          "\n"
                                          "\n# server prodtest address"
                                          "\nprodtestServerWeb.prodtestServerAddr=127.0.0.1:2000"
                                         );
            instance1->setConfig(cnf1);
            instance1->initServices();
        }
#endif

	printf("connecting to server app with telnet console\nUse help command to get help\n");
        sleep(2);
        system("telnet 127.0.0.1 8081");
        sleep(1);
        delete iUtils;
        return 0;

    } catch (CommonError& e)
    {
        printf("CommonError %s\n",e.what());
    }
    return 1;
}
