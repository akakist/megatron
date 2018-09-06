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
        while(!iUtils->isTerminating())
        {
            //logErr2("waiting term %d",iUtils->isTerminating() );
            usleep(100000);
        }


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

static bool ex=false;
void onterm(int signum)
{
    static std::string ss;
    char s[200];
    snprintf(s,sizeof(s),"Terminating on SIGNAL %d\n",signum);
    if(ss!=s)
    {
        printf("%s",s);
        ss=s;
    }

    try
    {
        if (!ex)
        {
#ifndef WIN32
            if(signum==SIGHUP)
            {
                iUtils->getIThreadNameController()->print_term(signum);
                return;
            }
#endif
            ex=true;
            iUtils->getIThreadNameController()->print_term(signum);
            iUtils->setTerminate();
//            mega.stop();

            printf("Terminating on SIGNAL %d\n",signum);
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
