#include "IInstance.h"
#include "colorOutput.h"
#include "configObj.h"
#include "version_mega.h"
#include "CUtils.h"
//bool done_test_http=false;
void registerRPCService(const char* pn);
void registerSocketModule(const char* pn);
void registerTimerService(const char* pn);
void registerSSL(const char* pn);
void registerOscarModule(const char* pn);
void registerOscarSecureModule(const char* pn);
void registerDFSReferrerService(const char* pn);
void registerDFSCapsService(const char* pn);
void registerGEOIP(const char* pn);
int mainTestReferrer(int argc, char** argv )
{
    try {
        iUtils=new CUtils(argc, argv, "referrerTest");

        registerRPCService(NULL);
        registerSocketModule(NULL);
        registerTimerService(NULL);
        registerSSL(NULL);
        registerOscarModule(NULL);
        registerOscarSecureModule(NULL);
        registerDFSReferrerService(NULL);
        registerDFSCapsService(NULL);
        registerGEOIP(NULL);

        printf(GREEN("RUN TEST %s"),__PRETTY_FUNCTION__);

        {
            IInstance *instance1=iUtils->createNewInstance("root");
            ConfigObj *cnf1=new ConfigObj("root.conf");
            instance1->setConfig(cnf1);
            instance1->initServices();
            sleep(1);
        }
        for(int i=0; i<10; i++)
        {
            IInstance *instance1=iUtils->createNewInstance("node_"+std::to_string(i));

            ConfigObj *cnf1=new ConfigObj("node.conf");
            instance1->setConfig(cnf1);
            instance1->initServices();
            sleep(1);

        }
        while(!iUtils->isTerminating())
        {
            sleep(1);
        }
        delete iUtils;
        return 0;

    } catch (CommonError& e)
    {
        printf("CommonError %s\n",e.what());
    }
    return 1;
}
