#ifndef ______E_POLL_H
#define ______E_POLL_H
#include "pconfig.h"
#ifdef __linux__
#include <sys/epoll.h>
#endif

#if defined(WIN32)
#define HAVE_SELECT
#elif defined (__MACH__) || defined(__IOS__) || defined(__FreeBSD__)
#define HAVE_KQUEUE
#elif __linux__
#define HAVE_EPOLL
#else
#error "invalid def"
#endif



#endif // E_POLL_H
