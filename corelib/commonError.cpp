#include "commonError.h"
#include "mutexInspector.h"
#ifndef __ANDROID__
#include <sys/timeb.h>
#endif
#ifndef _WIN32
#include <syslog.h>
#endif
#include "st_FILE.h"

#ifdef __MACH__
#ifndef __IOS__
#ifdef QT_CORE_LIB
#include <QStandardPaths>
#include <QCoreApplication>
#endif
#endif
#endif
#ifdef __ANDROID__
#include <android/log.h>
#endif
#include <stdarg.h>
#include "megatron_config.h"

#ifdef QT_CORE_LIB
#include <QStandardPaths>
#endif
#if !defined __MOBILE__
static Mutex *__logLock=NULL;
#endif
bool prevLogUnlinked=false;
CommonError::CommonError(const std::string& str):m_error(str)
{
#ifdef DEBUG
    fprintf(stderr,"CommonError raised: %s %s\n",m_error.c_str(),_DMI().c_str());
#endif
}
CommonError::CommonError(const char* fmt, ...):m_error(fmt)
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
CommonError::~CommonError() throw()
{
}

#if !defined __MOBILE__

static std::string getLogName()
{
    if(!iUtils) return ".log";
    std::string fn=(std::string)iUtils->app_name()+".log";
    {
#if defined(_WIN32) && defined(QT_CORE_LIB)
        fn="data/"+fn;
#else
        fn=iUtils->gLogDir()+"/"+fn;
#endif
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

//#if !defined __MOBILE__

    {
        va_list ap,ap1;
        va_start(ap, fmt);
        va_start(ap1, fmt);
#ifndef _WIN32
        if(/* DISABLES CODE */ (0))
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
    if(/* DISABLES CODE */ (0)) {
        va_list ap;
        va_start(ap,fmt);
        vsyslog(LOG_ERR,fmt,ap);
    }
#endif
//#endif
}
time_t start=time(NULL);
FILE * fd=NULL;
void logErr2(const char* fmt, ...)
{
//#if !defined __MOBILE__

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
            if(iUtils->argc())
            {
                fprintf(stderr,"%s: ",iUtils->argv()[0]);
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
            if(fd==NULL)
            {
                fd=fopen(getLogName().c_str(),"a+b");
            }
            if(fd) {
                timeb tb;
                ftime(&tb);
                time_t t=time(NULL);
                struct tm tt=*localtime(&t);

                fprintf(fd,"%04d.%03d %04d-%02d-%02d %02d:%02d:%02d ",
                        int(tb.time-start),tb.millitm,
                        tt.tm_year+1900,tt.tm_mon+1,tt.tm_mday, tt.tm_hour,tt.tm_min,tt.tm_sec
                       );
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
//#endif
}
#if defined(__MACH__) && !defined __IOS__ && defined QT5
#include <QStandardPaths>
#endif


