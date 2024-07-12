#include "IInstance.h"
#include "colorOutput.h"
#include "configObj.h"
#include "CUtils.h"
#include <unistd.h>
//bool done_test_timer=false;
void registerRPCService(const char* pn);
void registerSocketModule(const char* pn);
void registerTimerService(const char* pn);
void registerSSL(const char* pn);
void registerOscarModule(const char* pn);
void registerOscarSecureModule(const char* pn);
void registerTestTimer(const char* pn);
void registerTimerService(const char* pn);

int mainTestTimer(int argc, char** argv )
{
    try {
        iUtils=new CUtils(argc, argv, "timerTest");

        registerRPCService(NULL);
        registerSocketModule(NULL);
        registerTimerService(NULL);
        registerSSL(NULL);
        registerOscarModule(NULL);
        registerOscarSecureModule(NULL);
        registerTimerService(NULL);
        registerTestTimer(NULL);

        printf(GREEN("RUN TEST %s"),__PRETTY_FUNCTION__);

        {
            IInstance *instance1=iUtils->createNewInstance("testtimer1");
            ConfigObj *cnf1=new ConfigObj("testtimer1",
                                          "\nStart=testTimer"
                                         );
            instance1->setConfig(cnf1);
            instance1->initServices();
        }

        usleep(1000000);
        delete iUtils;
        return 0;

    } catch (CommonError& e)
    {
        printf("CommonError %s\n",e.what());
    }
    return 1;
}
