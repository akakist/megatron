#ifndef _______Ilogger___H
#define _______Ilogger___H
#include "unknown.h"
#include "unknownCastDef.h"

/// interface for logger service
class ILogger
{
public:
    virtual void _log(const char* str)=0;
    virtual ~ILogger() {}
    ILogger(UnknownBase* i)
    {
        i->addClass(UnknownCast::ILogger,this);
    }
};

#endif
