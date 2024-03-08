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

void logErr(const char* fmt, ...)
{


    {
        va_list ap,ap1;
        va_start(ap, fmt);
        va_start(ap1, fmt);
#ifndef _WIN32
        if((0))
        {
#if !defined __ANDROID__

            vfprintf(stderr,fmt, ap);
            fprintf(stderr,"\n");
#else
            __android_log_vprint(ANDROID_LOG_DEBUG, "tupo", fmt,ap);
#endif
        }
#endif
#if !defined __MOBILE__

        if(1) {
            if(!__logLock) __logLock=new Mutex;
            M_LOCK(__logLock);
            st_FILE f(getLogName(),"a+b");
            vfprintf(f.f,fmt,ap1);
            fprintf(f.f,"\n");
        }
#endif

    }
#ifndef _WIN32
#endif
}
time_t start=time(nullptr);
FILE * fd=nullptr;
void logErr2(const char* fmt, ...)
{

#ifdef WITH_SLOG
    auto prf=iUtils->getLogPrefix();
    std::string s_prf=iUtils->join(" @ ",prf);
#endif

    {
        va_list ap,ap1;
        va_start(ap, fmt);
        va_start(ap1, fmt);
#ifndef _WIN32
        if(1)
        {
#ifdef __ANDROID__
            __android_log_vprint(ANDROID_LOG_ERROR, "megatron@@@", fmt,ap);
#else
#ifdef __IOS__
            timeb tb;
            ftime(&tb);
            fprintf(stderr, "%ld.%d - ", tb.time,tb.millitm);
#endif
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
#endif

#if !defined __MOBILE__

        if(1)
        {
            if(!__logLock) __logLock=new Mutex;
            M_LOCK(__logLock);
            if(fd==nullptr)
            {
                fd=fopen(getLogName().c_str(),"a+b");
            }
            if(fd) {
#ifndef __FreeBSD__
                timeb tb;
                ftime(&tb);
#endif
                time_t t=time(nullptr);
                struct tm tt=*localtime(&t);

#ifdef __FreeBSD__
                fprintf(fd,"%04d-%02d-%02d %02d:%02d:%02d ",
                        tt.tm_year+1900,tt.tm_mon+1,tt.tm_mday, tt.tm_hour,tt.tm_min,tt.tm_sec
                       );
#else
                fprintf(fd,"%04d.%03d %04d-%02d-%02d %02d:%02d:%02d ",
                        int(tb.time-start),tb.millitm,
                        tt.tm_year+1900,tt.tm_mon+1,tt.tm_mday, tt.tm_hour,tt.tm_min,tt.tm_sec
                       );
#endif
#ifdef WITH_SLOG
                fprintf(fd," %s ",s_prf.c_str());
#endif
                vfprintf(fd,fmt,ap1);
                fprintf(fd,"\n");
                fflush(fd);
            }
        }
#endif
    }
#if !defined(_WIN32) && !defined(__ANDROID__)
    if(0) {
        va_list ap;
        va_start(ap,fmt);
        vsyslog(LOG_ERR,fmt,ap);
    }
#endif
}


