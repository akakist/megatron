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
#include "CUtils.h"
#include "IInstance.h"
#include "IThreadNameController.h"
#include "megatron.h"
#include "megatron_config.h"
void onterm(int signum);


//IUtils *iUtils=NULL;
void registerDFSCapsService(const char* pn);
void registerDFSReferrerService(const char*pn);
void registerErrorDispatcherService(const char* pn);
void registerHTTPModule(const char* pn);
void registerObjectProxyModule(const char* pn);
void registerOscarModule(const char* pn);
void registerOscarSecureModule(const char* pn);
void registerReferrerClientService(const char*pn);
void registerRPCService(const char* pn);
void registerSocketModule(const char* pn);
void registerSSL(const char* pn);
void registerTelnetService(const char* pn);
void registerTimerService(const char* pn);
void registerWebHandlerModule(const char* pn);



//megatron mega;
#ifdef KALL
static void load_plugins_info(const std::set<std::string>& bases)
{
#ifndef _MSC_VER
    for (std::set<std::string>::const_iterator I=bases.begin(); I!=bases.end(); ++I)
    {
        std::string base=iUtils->expand_homedir(*I);


        DIR * dir=opendir(base.c_str());
        if (!dir)
        {
            //fprintf(stderr,"warn: cannot opendir %s\n",base.c_str());
            continue;
        }
        struct dirent* de;
        std::set<std::string> ss;
        while ((de=readdir(dir))!=NULL)
        {
            bool ok=false;
            ok=true;
            std::string n=de->d_name;
            if(n=="." || n=="..")
                continue;
            if (ok)
            {
                std::string pn;
                //if(home)
                pn=(std::string)base+"/"+(std::string)de->d_name;
                /*else
                    pn=de->d_name;*/
                ss.insert(pn);
                DBG(logErr2("plugin %s",pn.c_str()));
            }
        }
        for (std::set<std::string>::iterator i=ss.begin(); i!=ss.end(); ++i)
        {
            std::string pn=*i;
            struct stat st;
            if (stat(pn.c_str(),&st))
            {
                logErr2("cannot load plugin %s",pn.c_str());
            }
            logErr2("load plugin %s",pn.c_str());
            if (!stat(pn.c_str(),&st))
            {
#ifdef _WIN32
                HMODULE h=LoadLibraryA(pn.c_str());
#else
                void *h=dlopen(pn.c_str(),RTLD_LAZY);
#endif
                if (!h)
                {
                    logErr2("cannot load plugin %s",pn.c_str());
                }

                if (h)
                {
#ifdef DEBUG
                    const char *funcname="registerModuleDebug";
#else
                    const char *funcname="registerModule";
#endif
#ifdef _WIN32
                    REGOBJECT f=(REGOBJECT )GetProcAddress(h,funcname);
#else
                    REGOBJECT f=(REGOBJECT)dlsym(h,funcname);
#endif
                    if (f)
                    {
                        f(iUtils,i->c_str());
                    }
#ifdef _WIN32
                    FreeLibrary(h);
#else
                    dlclose(h);
#endif
                }
            }

        }
        closedir(dir);
    }
#endif
}
#endif
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

        //try {
#ifdef _WIN32
        if(!win32Initialized)
        {
            win32Initialized=true;
            WORD wVersionRequested;
            WSADATA wsaData;
            int err;
            wVersionRequested = MAKEWORD(2, 2);
            err = WSAStartup(wVersionRequested, &wsaData);
            if (err != 0)
            {
                logErr2("WSAStartup failed\n");
                return ;
            }
        }
#endif


        iUtils=new CUtils(argc,argv,"app_name");



        std::set<std::string> sp;

registerDFSCapsService(NULL);
registerDFSReferrerService(NULL);
registerErrorDispatcherService(NULL);
registerHTTPModule(NULL);
registerObjectProxyModule(NULL);
registerOscarModule(NULL);
registerOscarSecureModule(NULL);
registerReferrerClientService(NULL);
registerRPCService(NULL);
registerSocketModule(NULL);
registerSSL(NULL);
registerTelnetService(NULL);
registerTimerService(NULL);
registerWebHandlerModule(NULL);


        sp.insert(PLUGIN_TARGET_DIR);

        iUtils->load_plugins_info(sp);


        std::vector<itest_static_constructor> tests=iUtils->getAllITests();

        for(auto z:tests)
        {

            ITests::Base *t=z();
            t->run();
            delete t;
        }
	delete iUtils;




//    megatron::run_test(argc,argv);


//    registerModules();
//        factory->initServices();
    }


    catch(std::runtime_error e)
    {
#ifdef _WIN32
        printf("CommonError: %s\n",e.what());
#else
        syslog(LOG_ERR,"CommonError: %s\n",e.what());
#endif
    }
    catch(std::exception e)
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
	    delete iUtils;
//            mega.stop();
            exit(0);

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
