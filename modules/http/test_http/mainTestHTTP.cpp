#include "IInstance.h"
#include "colorOutput.h"
#include "configObj.h"
#include "CUtils.h"
#include <unistd.h>
//bool done_test_http=false;
void registerRPCService(const char* pn);
void registerSocketModule(const char* pn);
void registerTimerService(const char* pn);
void registerSSL(const char* pn);
void registerOscarModule(const char* pn);
void registerOscarSecureModule(const char* pn);
void registertestServerWebService(const char* pn);
void registerHTTPModule(const char* pn);

int mainTestHTTP(int argc, char** argv )
{
    try {
        iUtils=new CUtils(argc, argv, "httpTest");

        registerRPCService(NULL);
        registerSocketModule(NULL);
        registerTimerService(NULL);
        registerSSL(NULL);
        registerOscarModule(NULL);
        registerOscarSecureModule(NULL);
        registertestServerWebService(NULL);
        registerHTTPModule(NULL);

        printf(GREEN("RUN TEST %s"),__PRETTY_FUNCTION__);

        {
            IInstance *instance1=iUtils->createNewInstance("testHTTP1");
            ConfigObj *cnf1=new ConfigObj("testHTTP1",
                                          "\nRPC.ConnectionActivity=600.000000"
                                          "\nRPC.IterateTimeoutSec=60.000000"
                                          "\nRPC.ListenerBuffered.MaxThreadsCount=10"
                                          "\nStart=testHTTP"
                                          "\nOscarSecure.ListenerBuffered.MaxThreadsCount=10"
                                          "\nOscarSecure.RSA_keysize=256"
                                          "\nOscarSecure.maxPacketSize=33554432"
                                          "\nRPC.BindAddr_MAIN=INADDR_ANY:2000"
                                          "\nRPC.BindAddr_RESERVE=NONE"
                                          "\nSocketIO.ListenerBuffered.MaxThreadsCount=10"
                                          "\nSocketIO.listen_backlog=128"
                                          "\nSocketIO.size=1024"
                                          "\nSocketIO.timeout_millisec=7000"
                                          "\nWebHandler.bindAddr=NONE"

                                          "\n# http listen address"
                                          "\ntestHTTP.bindAddr=0.0.0.0:8088"
                                          "\nHTTP.max_post=1000000"
                                          "\nHTTP.doc_urls=/pics,/html,/css"
                                          "\nHTTP.document_root=./www"
                                          "\nSocketIO.epoll_timeout_millisec=12000"
                                          "\n"
                                          "\n# socket poll thread count"
                                          "\nSocketIO.n_workers=6"
                                         );
            instance1->setConfig(cnf1);
            instance1->initServices();
        }

        usleep(1000000);
        system("ab -n 1000000  -k -c 100  http://127.0.0.1:8088/");
//        system("ab -n 1000000   -c 10  http://127.0.0.1:8088/");
//        sleep(1);
        delete iUtils;
        return 0;

    } catch (CommonError& e)
    {
        printf("CommonError %s\n",e.what());
    }
    return 1;
}
