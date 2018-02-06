#include "commonError.h"
#include <stdarg.h>
#include <stdio.h>
#ifndef _WIN32
#include <syslog.h>
#endif

#ifdef QT5
#include <QStandardPaths>
#endif

CommonError::CommonError(const std::string& str):m_error(str)
{
    fprintf(stderr,"CommonError raised: %s\n",m_error.c_str());
}
CommonError::CommonError(const char* fmt, ...)
{

    va_list ap;
    char str[1024*10];
    va_start(ap, fmt);
    vsnprintf(str, sizeof(str), fmt, ap);
    va_end(ap);
    m_error=str;

    fprintf(stderr,"CommonError raised: %s\n",m_error.c_str());

}
CommonError::~CommonError() throw()
{
}


void assert(bool a)
{
    if(!a)
        throw CommonError("assert failed");
}
