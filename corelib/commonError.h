#ifndef _____COMMMON_ERR___H
#define _____COMMMON_ERR___H

#include <exception>
#include <stdexcept>
#include <string>
#include "pconfig.h"
/**
* Common exception
*/

class CommonError: public std::exception
{
private:
    std::string m_error;
public:
    void append(const std::string &s)
    {
        m_error.append(s);
    }
    CommonError(const std::string& str);
    CommonError(const char* fmt, ...);
    ~CommonError() throw();
    const char* what() const throw()
    {
        return m_error.c_str();
    };
};
void logErr(const char* fmt, ...);
void logErr2(const char* fmt, ...);
void logRemote(const char *fmt, ...);


/// DBG - hide code in release mode
#ifdef DEBUG
#define DBG(a) a
#else
#define DBG(a)
#endif

/// XTRY, XPASS is two macros used to print stack while exception throwing
#ifdef DEBUG
#define XTRY try{
#define XPASS } catch(...){logErr2("XPASS @%s %s %d",__func__,__FILE__,__LINE__);throw;}
#else
#define XTRY
#define XPASS
#endif
#endif
