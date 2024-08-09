#include "commonError.h"
#include "mutexInspector.h"
#if !defined __ANDROID__ && !defined __FreeBSD__

#include <sys/timeb.h>
#endif
#ifndef _WIN32
#include <syslog.h>
#endif
#include "st_FILE.h"

#ifdef __MACH__
#ifndef __IOS__
#endif
#endif
#ifdef __ANDROID__
#include <android/log.h>
#endif
#include <stdarg.h>
#include "megatron_config.h"


#if !defined __MOBILE__
static Mutex *__logLock=nullptr;
#endif
bool prevLogUnlinked=false;
CommonError::CommonError(const std::string& str):m_error(str)
{
#ifdef DEBUG
    fprintf(stderr,"CommonError raised: %s %s\n",m_error.c_str(),_DMI().c_str());
#endif
}
CommonError::CommonError(const char* fmt, ...) :m_error(fmt)
{

    va_list ap;
    char str[1024*10];
    va_start(ap, fmt);
    vsnprintf(str, sizeof(str), fmt, ap);
    va_end(ap);
    m_error=str;
#ifdef DEBUG
    fprintf(stderr,"CommonError raised: %s %s\n",m_error.c_str(),_DMI().c_str());
#endif

}

#if !defined __MOBILE__

static std::string getLogName()
{
    if(!iUtils) return ".log";
    std::string fn=(std::string)iUtils->app_name()+".log";
    {
        fn=iUtils->gLogDir()+"/"+fn;
    }
    if(!prevLogUnlinked)
    {
        prevLogUnlinked=true;
    }
    return fn;
}
#else
#endif

void logErr2(const char* fmt, ...)
{

#ifdef WITH_SLOG
    auto prf=iUtils->getLogPrefix();
    std::string s_prf=iUtils->join(" @ ",prf);
#endif

    {
        va_list ap;
        va_start(ap, fmt);
#ifndef _WIN32
        if(1)
        {
            {
                fprintf(stderr,"%s: ",iUtils->app_name().c_str());
#ifdef WITH_SLOG
                fprintf(stderr," %s ",s_prf.c_str());
#endif
            }
            vfprintf(stderr,fmt, ap);
            fprintf(stderr,"\n");
#endif
        }

    }
}


