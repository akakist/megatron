#ifndef _________________PTHREADS_WIN32____H
#define _________________PTHREADS_WIN32____H

/**
portability wrappers
*/
#include <pthread.h>
#ifndef _MSC_VER
#include <unistd.h>
#else
#include <Windows.h>
#endif

#ifdef _WIN32
typedef int socklen_t;
#endif
#if defined(_WIN32) || defined(_MSC_VER)
#include <sys/types.h>
#endif
#endif
