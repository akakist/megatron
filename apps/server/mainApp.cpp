#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#ifdef _WIN32
#else
#include <dlfcn.h>
#endif
#include <sys/stat.h>
#include <signal.h>
#include <stdio.h>
#ifndef _WIN32
#include <syslog.h>
#endif
#include "IInstance.h"
#include "IThreadNameController.h"
#include "megatron.h"
#include "colorOutput.h"
void onterm(int signum);
#ifdef _LINK_BASE
void registerSocketModule();
void registerOscarModule();
void registerOscarSecureModule();
void registerTimerService();
void registerRPCService();
void registerSqliteModule();
void registerMysqlModule();
void registerTelnetService();
void registerLoggerService();
void registerObjectProxyModule();
#endif

#ifdef _LINK_DFS
void registerDFSService();
void registerDFSAviSourceService();
void registerDFSFileStoreService();
void registerDFSPipeStoreService();
void registerDFSReferrerService();
void registerDFSSourceMixerService();
void registerHTTPModule();
void registerWebHandlerModule();
void registerDFSIOService();
void registerDFSTestService();
void onterm(int signum);
void registerDfsIo();
void registerEvents();
void registerDFSFileUploaderService();
#endif




megatron mega;
static bool ex=false;
static time_t exTime=0;

int main(int argc, char* argv[])
{
    try
    {

#ifndef _WIN32
        signal(SIGPIPE,SIG_IGN);
#endif
        signal(SIGABRT,onterm);
        signal(SIGTERM,onterm);
        signal(SIGINT,onterm);
        signal(SIGSEGV,onterm);
        signal(SIGFPE,onterm);
        signal(SIGILL,onterm);
#ifndef _WIN32
        signal(SIGQUIT,onterm);
        signal(SIGBUS,onterm);
        signal(SIGHUP,onterm);
#endif
        signal(10,onterm);

        if (argc>1)
        {
#ifndef _WIN32
            if (strcmp(argv[1],"-d")==0)
            {
                printf("Continuing as daemon\n");

                if (fork()) exit(1);
            }
#endif
        }
        mega.run(argc,argv);
        time_t startT=time(NULL);
        while(!ex)
        {
            //logErr2("waiting term %d",iUtils->isTerminating() );
            usleep(100000);
            if(time(NULL)-startT>2)
            {
//        	return 0;
            }

        }
        delete iUtils;
        iUtils=NULL;


    }
    catch(std::runtime_error e)
    {
#ifdef _WIN32
        printf("CommonError: %s\n",e.what());
#else
        syslog(LOG_ERR,"CommonError: %s\n",e.what());
#endif
    }
    catch(std::exception &e)
    {
        printf("std::exception: %s\n",e.what());
    }

}
bool is_term=false;
void onterm(int signum)
{
    if(is_term) return;
    is_term=true;
    printf("\n@@ %s\n",__FUNCTION__);
    static std::string ss;
    char s[200];
    snprintf(s,sizeof(s),RED("Terminating on SIGNAL %d"),signum);
    if(ss!=s)
    {
        logErr2("%s",s);
        ss=s;
    }

    //printf("ex %d \n",ex);
    try
    {

        if (!ex || time(NULL)-exTime>2)
        {
//    	    exTime=time(NULL);
#ifndef WIN32

            if(!iUtils)
                return;
            auto tc=iUtils->getIThreadNameController();
            if(!tc)
                return;
            if(signum==SIGHUP)
            {
                tc->print_term(signum);
                return;
            }
#endif

            exTime=time(NULL);
            ex=true;
            iUtils->setTerminate();

            tc->print_term(signum);
//            if(iUtils)
//                delete iUtils;


//            mega.stop();


            printf(RED("Terminating on SIGNAL %d"),signum);
//            exit(1);
        }
    }
    catch(std::exception e)
    {
//        delete cFactory;
        printf("onterm exception: %s\n",e.what());
    }
    catch(...)
    {
//        delete cFactory;
        printf("--Error: unknow error in on_term()\n");
    }
}
