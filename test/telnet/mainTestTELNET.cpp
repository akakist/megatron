#include "IInstance.h"
#include "colorOutput.h"
#include "configObj.h"
#include "CUtils.h"
#include <unistd.h>
void registerSocketModule(const char* pn);
void registerTimerService(const char* pn);
void registerTelnetService(const char* pn);
void registerteldemo1(const char* pn);

int mainTestTELNET(int argc, char** argv )
{
    try {
        iUtils=new CUtils(argc, argv, "telnetTest");

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
