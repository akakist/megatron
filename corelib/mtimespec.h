#ifndef ____MTIMESPEC___H
#define ____MTIMESPEC___H
#ifndef _MSC_VER
#include <sys/time.h>
#endif
#if !defined __MOBILE__
#ifndef  __ANDROID_API__
#include <sys/timeb.h>
#endif
#endif
#include "commonError.h"
#ifdef _WIN32
#include <pthread.h>
#endif

#define _nano_ 1000000000

struct mtimespec: public timespec
{
    mtimespec(long sec,long nsec);
    mtimespec() ;
};
void nanotime(timespec* t);

int operator<(const mtimespec& a,const mtimespec& b);
int operator<=(const mtimespec& a,const mtimespec& b);
mtimespec operator+(const mtimespec& a,const mtimespec& b);
mtimespec operator-(const mtimespec& a,const mtimespec& b);
#endif
