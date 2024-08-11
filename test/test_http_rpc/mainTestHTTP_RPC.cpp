#include "IInstance.h"
#include "colorOutput.h"
#include "configObj.h"
#include "CUtils.h"
#include <unistd.h>
void registerRPCService(const char* pn);
void registerSocketModule(const char* pn);
void registerTimerService(const char* pn);
void registerSSL(const char* pn);
void registerOscarModule(const char* pn);
void registerOscarSecureModule(const char* pn);
//void registertestServerWebService(const char* pn);
void registerHTTPModule(const char* pn);
void registerprodtestServerService(const char* pn);
void registerprodtestServerWebService(const char* pn);
#ifdef WEBDUMP
void registerWebHandlerModule(const char* pn);
#endif

int mainTestHTTP_RPC(int argc, char** argv )
{
    try {
        iUtils=new CUtils(argc, argv, "HTTP_RPC_test");

        registerRPCService(NULL);
        registerSocketModule(NULL);
        registerTimerService(NULL);
        registerSSL(NULL);
        registerOscarModule(NULL);
        registerOscarSecureModule(NULL);
//        registertestServerWebService(NULL);
        registerHTTPModule(NULL);
        registerprodtestServerService(NULL);
        registerprodtestServerWebService(NULL);
#ifdef WEBDUMP
        registerWebHandlerModule(NULL);
#endif


        printf(GREEN("RUN TEST %s"),__PRETTY_FUNCTION__);

        {
            IInstance *instance1=iUtils->createNewInstance("prodtestServer");
            ConfigObj *cnf1=new ConfigObj("prodtestServer",
                                          "\nRPC.ConnectionActivity=600.000000"
                                          "\nRPC.IterateTimeoutSec=60.000000"
                                          "\nStart=prodtestServer,RPC"
                                          "\nOscarSecure.RSA_keysize=256"
                                          "\nOscarSecure.maxPacketSize=33554432"
                                          "\nRPC.BindAddr_MAIN=INADDR_ANY:2000"
                                          "\nRPC.BindAddr_RESERVE=NONE"
                                          "\nRPC.oscarType=Oscar"
                                          "\nSocketIO.listen_backlog=128"
                                          "\nSocketIO.size=1024"
                                          "\nSocketIO.timeout_millisec=10"
                                          "\nWebHandler.bindAddr=INADDR_ANY:6002"

                                          "\n# http listen address"
                                          "\nHTTP.max_post=1000000"
                                          "\nHTTP.doc_urls=/pics,/html,/css"
                                          "\nHTTP.document_root=./www"
                                          "\nSocketIO.epoll_timeout_millisec=2000"
                                          "\n"
                                          "\n# socket poll thread count"
                                          "\nSocketIO.n_workers=3"
                                          "\nOscar.maxPacketSize=33554432"
                                         );
            instance1->setConfig(cnf1);
            instance1->initServices();
        }

        {
            IInstance *instance1=iUtils->createNewInstance("prodtestServerWeb");
            ConfigObj *cnf1=new ConfigObj("prodtestServerWeb",
                                          "\nRPC.ConnectionActivity=600.000000"
                                          "\nRPC.IterateTimeoutSec=60.000000"
                                          "\nRPC.ListenerBuffered.MaxThreadsCount=10"
                                          "\nStart=prodtestServerWeb,RPC"
                                          "\nOscarSecure.RSA_keysize=256"
                                          "\nOscarSecure.maxPacketSize=33554432"
                                          "\nRPC.BindAddr_MAIN=INADDR_ANY:0"
                                          "\nRPC.BindAddr_RESERVE=NONE"
                                          "\nRPC.oscarType=Oscar"
                                          "\nSocketIO.listen_backlog=128"
                                          "\nSocketIO.size=1024"
                                          "\nSocketIO.timeout_millisec=10"
                                          "\nWebHandler.bindAddr=INADDR_ANY:6001"
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

        sleep(2);
        auto ef=system("ab -n 1000000 -k -c 200  http://127.0.0.1:8088/");
        if(ef!=0)
            sleep(60*60);
        iUtils->setTerminate(ef);
        sleep(1);
        delete iUtils;
        return 0;

    } catch (CommonError& e)
    {
        printf("CommonError %s\n",e.what());
    }
    return 1;
}
