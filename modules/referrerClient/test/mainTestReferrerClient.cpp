#include "IInstance.h"
#include "colorOutput.h"
#include "configObj.h"
#include "version_mega.h"
#include "CUtils.h"
#include "appHandler.h"
//bool done_test_http=false;
#include "Events/DFS/Caps/RegisterMyRefferrerNode.h"
void registerRPCService(const char* pn);
void registerSocketModule(const char* pn);
void registerTimerService(const char* pn);
void registerSSL(const char* pn);
void registerOscarModule(const char* pn);
void registerOscarSecureModule(const char* pn);
void registerDFSReferrerService(const char* pn);
void registerDFSCapsService(const char* pn);
void registerGEOIP(const char* pn);
void registerReferrerClientService(const char* pn);
void registerObjectProxyModule(const char* pn);
void registerWebHandlerModule(const char* pn);
int mainTestReferrerClient(int argc, char** argv )
{
    try {
        iUtils=new CUtils(argc, argv, "referrerClientTest");

        registerRPCService(NULL);
        registerSocketModule(NULL);
        registerTimerService(NULL);
        registerSSL(NULL);
        registerOscarModule(NULL);
        registerOscarSecureModule(NULL);
        registerDFSReferrerService(NULL);
        registerDFSCapsService(NULL);
        registerGEOIP(NULL);
        registerReferrerClientService(NULL);
        registerObjectProxyModule(NULL);
        registerWebHandlerModule(NULL);
        iUtils->registerEvent(dfsCapsEvent::RegisterMyRefferrerNodeREQ::construct);

        printf(GREEN("RUN TEST %s"),__PRETTY_FUNCTION__);

        {
            IInstance *instance1=iUtils->createNewInstance("root");
            ConfigObj *cnf1=new ConfigObj("root.conf");
            instance1->setConfig(cnf1);
            instance1->initServices();
            sleep(1);
        }
        for(int i=0; i<2; i++)
        {
            IInstance *instance1=iUtils->createNewInstance("node_"+std::to_string(i));

            ConfigObj *cnf1=new ConfigObj("node_"+std::to_string(i)+".conf");
            instance1->setConfig(cnf1);
            instance1->initServices();
            sleep(1);

        }
        std::set<AppHandler*> apps;
        for(int i=0; i<1; i++)
        {
            IInstance *instance1=iUtils->createNewInstance("client_"+std::to_string(i));
            ConfigObj *cnf1=new ConfigObj("client.conf");
            instance1->setConfig(cnf1);
            AppHandler * h=new AppHandler(instance1);
            apps.insert(h);
            h->init(argc,argv,"",instance1);
            sleep(1);
        }
        while(!iUtils->isTerminating())
        {
            sleep(1);
        }
        for(auto& z:apps)
            delete z;
        delete iUtils;
        return 0;

    } catch (CommonError& e)
    {
        printf("CommonError %s\n",e.what());
    }
    return 1;
}
