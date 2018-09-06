#define _FILE_OFFSET_BITS 64
#include "pconfig.h"
#ifndef _MSC_VER
#include <unistd.h>
#include <dirent.h>
#endif
#include <sys/stat.h>
#include <stdio.h>
#ifdef _WIN32
#else
#include <dlfcn.h>
#endif
#include "megatronClient.h"
#include "CInstance.h"
#include "CUtils.h"
#include "IObjectProxy.h"
#include "serviceEnum.h"
#include "Events/System/Net/rpc/SendPacket.h"


//IUtils *iUtils=NULL;

#if !defined __MOBILE__

void registerHTTPModule(const char*);
void registerSqliteModule(const char*);
void registerTelnetService(const char*);
void registerWebHandlerModule(const char*);
void registerErrorDispatcherService(const char*);
#endif

void registerObjectProxyModule(const char*);
void registerOscarModule(const char*);
void registerOscarSecureModule(const char*);
void registerRPCService(const char*);
void registerSocketModule(const char*);
void registerTimerService(const char*);
void registerReferrerClientService(const char* pn);
static void registerModules()
{
    char *pn=NULL;
    {
#if !defined __MOBILE__
        registerHTTPModule(pn);
        registerTelnetService(pn);
        registerWebHandlerModule(pn);
        registerErrorDispatcherService(pn);
#endif
        registerObjectProxyModule(pn);
        registerOscarModule(pn);
        registerOscarSecureModule(pn);
        registerRPCService(pn);
        registerSocketModule(pn);
        registerTimerService(pn);
        registerReferrerClientService(pn);
    }
    void registerSSL(const char* pn);
    registerSSL(pn);
}

IUtils *Megatron::initMegatron(int argc, char** argv)
{
    std::string appName="app_name";
    if(argc>0)
    {
        appName.clear();
        for(int i=0; i<strlen(argv[0]); i++)
        {
            if(isalnum(argv[0][i]))
                appName+=argv[0][i];

        }
    }
    iUtils=new CUtils(argc,argv,appName);

    registerModules();
    return iUtils;
}
IInstance *Megatron::createInstance(const std::string& config)
{
    IInstance *instance=iUtils->createNewInstance();
    ConfigObj *cnf1=new ConfigObj("client",config);
    instance->setConfig(cnf1);
    instance->initServices();
    return instance;
}
void Megatron::setFilesDir(IUtils* iu,const std::string &fdir)
{
    iu->setFilesDir(fdir);
}

