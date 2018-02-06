#ifndef _______Ilogger___H
#define _______Ilogger___H
#include "unknown.h"
#include "unknownCastDef.h"

class ILogger
{
public:
    // check chunk in store only
    virtual void _log(const char* str)=0;
    virtual ~ILogger() {}
    ILogger(UnknownBase* i)
    {
        i->addClass(UnknownCast::ILogger,this);
    }
};

#endif
