#pragma once

#include "e_poll.h"
#ifdef HAVE_KQUEUE
#include <sys/event.h>
#endif
#include <string>
#include <vector>
#include "IUtils.h"
#ifdef HAVE_KQUEUE

inline std::string EVFILT_name(int16_t n)
{
    switch(n)
    {
    case EVFILT_READ:
        return "EVFILT_READ";
    case EVFILT_WRITE:
        return "EVFILT_WRITE";
    case EVFILT_AIO:
        return "EVFILT_AIO";
    case EVFILT_VNODE:
        return "EVFILT_VNODE";
    case EVFILT_PROC:
        return "EVFILT_PROC";
    case EVFILT_SIGNAL:
        return "EVFILT_SIGNAL";
    case EVFILT_TIMER:
        return "EVFILT_TIMER";
#ifdef __FreeBSD__
    case EVFILT_PROCDESC:
        return "EVFILT_MACHPORT";
    case EVFILT_LIO:
        return "EVFILT_LIO";
    case EVFILT_SENDFILE:
        return "EVFILT_SENDFILE";
#endif
#ifndef __FreeBSD__
    case EVFILT_MACHPORT:
        return "EVFILT_MACHPORT";
    case EVFILT_VM:
        return "EVFILT_VM";
    case EVFILT_EXCEPT:
        return "EVFILT_EXCEPT";
#endif
    case EVFILT_FS:
        return "EVFILT_FS";
    case EVFILT_USER:
        return "EVFILT_USER";
    case EVFILT_SYSCOUNT:
        return "EVFILT_SYSCOUNT";

    default:
        return "DEFAULT";
    }
    return "DEFAULT";
}

inline std::string EVFLAG_name(uint16_t f)
{
    std::vector<std::string> v;
    if(f&EV_ADD)v.push_back("EV_ADD");
    if(f&EV_DELETE)v.push_back("EV_DELETE");
    if(f&EV_ENABLE)v.push_back("EV_ENABLE");
    if(f&EV_DISABLE)v.push_back("EV_DISABLE");
    if(f&EV_ONESHOT)v.push_back("EV_ONESHOT");
    if(f&EV_CLEAR)v.push_back("EV_CLEAR");
    if(f&EV_RECEIPT)v.push_back("EV_RECEIPT");
    if(f&EV_DISPATCH)v.push_back("EV_DISPATCH");
    if(f&EV_SYSFLAGS)v.push_back("EV_SYSFLAGS");
    if(f&EV_FLAG1)v.push_back("EV_FLAG1");
    if(f&EV_EOF)v.push_back("EV_EOF");
    if(f&EV_ERROR)v.push_back("EV_ERROR");
#ifdef __FreeBSD__
    if(f&EV_DROP)v.push_back("EV_DROP");
    if(f&EV_FLAG1)v.push_back("EV_FLAG1");
    if(f&EV_FLAG2)v.push_back("EV_FLAG2");
    if(f&EV_FORCEONESHOT)v.push_back("EV_FORCEONESHOT");

#endif
#ifndef __FreeBSD__
    if(f&EV_UDATA_SPECIFIC)v.push_back("EV_UDATA_SPECIFIC");
    if(f&EV_DISPATCH2)v.push_back("EV_DISPATCH2");
    if(f&EV_VANISHED)v.push_back("EV_VANISHED");
    if(f&EV_FLAG0)v.push_back("EV_FLAG0");
#endif
    return iUtils->join("|",v);
}

#endif
