#include "mutexable.h"
#include "mutexInspector.h"
Mutex::Mutex()
{
#ifdef KALL
#ifdef __MACH__
    //m_lock=PTHREAD_MUTEX_RECURSIVE
    pthread_mutexattr_t   mta;
//or alternatively, initialize at runtime:

    pthread_mutexattr_init(&mta);

    pthread_mutexattr_settype(&mta, PTHREAD_MUTEX_RECURSIVE);

    pthread_mutex_init(&m_lock, &mta);
#else
    m_lock=PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
#endif
#endif
#ifdef _WIN32
    InitializeCriticalSection(&m_lock);
#else
    pthread_mutex_init(&m_lock, NULL);
#endif
}
MutexC::MutexC()
{
#ifdef KALL
#ifdef __MACH__
    //m_lock=PTHREAD_MUTEX_RECURSIVE
    pthread_mutexattr_t   mta;
//or alternatively, initialize at runtime:

    pthread_mutexattr_init(&mta);

    pthread_mutexattr_settype(&mta, PTHREAD_MUTEX_RECURSIVE);

    pthread_mutex_init(&m_lock, &mta);
#else
    m_lock=PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
#endif
#endif
    pthread_mutex_init(&m_lock, NULL);
}

void Mutex::lock() const
{
    XTRY;
//#define  DEBUG1z
#ifdef _WIN32
    EnterCriticalSection((CRITICAL_SECTION*)&m_lock);
#else

#ifdef DEBUG
    time_t start=time(NULL);
    while(1)
    {
        if(pthread_mutex_trylock((pthread_mutex_t*)&m_lock))
        {

            if(time(NULL)-start>10)
            {
                throw CommonError("mutexlock timeout %s",iUtils->getIThreadNameController()->dump_mutex_inspectors().c_str());
                //logErr2("mutexlock timeout %s",_DMI().c_str());
                // start=time(NULL);
            }
            usleep(1000);
            continue;

        }
        else return;
    }

#else
    if(pthread_mutex_lock((pthread_mutex_t*)&m_lock))
        throw CommonError("pthread_mutex_lock: errno %d", errno);
#endif
#endif
    XPASS;
}
void MutexC::lock() const
{
    XTRY;
//#define  DEBUG1z

#ifdef DEBUG
    time_t start=time(NULL);
    while(1)
    {
        if(pthread_mutex_trylock((pthread_mutex_t*)&m_lock))
        {

            if(time(NULL)-start>10)
            {
                throw CommonError("mutexlock timeout %s",iUtils->getIThreadNameController()->dump_mutex_inspectors().c_str());
                //logErr2("mutexlock timeout %s",_DMI().c_str());
                // start=time(NULL);
            }
            usleep(1000);
            continue;

        }
        else return;
    }

#else
    if(pthread_mutex_lock((pthread_mutex_t*)&m_lock))
        throw CommonError("pthread_mutex_lock: errno %d", errno);
#endif
    XPASS;
}

void Mutex::unlock() const
{
#ifdef _WIN32
    LeaveCriticalSection((CRITICAL_SECTION*)&m_lock);
#else
    if(pthread_mutex_unlock((pthread_mutex_t*)&m_lock))
        throw CommonError("pthread_mutex_unlock: errno %d", errno);
#endif
}
void MutexC::unlock() const
{
    if(pthread_mutex_unlock((pthread_mutex_t*)&m_lock))
        throw CommonError("pthread_mutex_unlock: errno %d", errno);
}
Mutex::~Mutex()
{
#ifdef _WIN32
    DeleteCriticalSection(&m_lock);
#else
    if(pthread_mutex_destroy(&m_lock))
    {
        //logErr2("pthread_mutex_destroy: errno %d", errno);
    }
#endif
}
MutexC::~MutexC()
{
    if(pthread_mutex_destroy(&m_lock))
    {
        //logErr2("pthread_mutex_destroy: errno %d", errno);
    }
}

Condition::Condition(const MutexC&m): m_mx(m)
{
    XTRY;
    if(pthread_cond_init(&m_cond,NULL))
        throw CommonError("pthread_cond_init: errno %d", errno);
    XPASS;
};
void Condition::wait() const
{
    XTRY;
    if(pthread_cond_wait((pthread_cond_t*)&m_cond,(pthread_mutex_t*)&m_mx.m_lock))
        throw CommonError("pthread_cond_wait: errno %d", errno);
    XPASS;
}
void Condition::timedwait(const mtimespec& ts) const
{
    XTRY;
    int r=pthread_cond_timedwait((pthread_cond_t*)&m_cond,(pthread_mutex_t*)&m_mx.m_lock,&ts);

    if(r!=ETIMEDOUT && r!=EAGAIN && r!=ENOTSOCK)
    {
#ifndef _WIN32
        if(errno!=ETIMEDOUT && errno!=0 && errno!=EAGAIN && errno!=ENOTSOCK)
        {
//            throw CommonError
        }
#else
        if(errno!=0)
        {
            //int eno=errno;

        }
#endif
    }
    XPASS;
}
void Condition::signal() const
{
    XTRY;
    if(pthread_cond_signal((pthread_cond_t*)&m_cond))
        throw CommonError("pthread_cond_signal: errno %d", errno);
    XPASS;
}
void Condition::broadcast() const
{
    XTRY;
    if(pthread_cond_broadcast((pthread_cond_t*)&m_cond))
        throw CommonError("pthread_cond_broadcast: errno %d", errno);
    XPASS;
}
Condition::~Condition()
{
    XTRY;
    if(pthread_cond_destroy((pthread_cond_t*)&m_cond))
        throw CommonError("pthread_cond_destroy: errno %d", errno);
    XPASS;
}
