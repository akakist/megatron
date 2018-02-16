#ifndef _______________MUTEXABLE___H
#define _______________MUTEXABLE___H
#include "pthread.h"
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "mtimespec.h"
#ifdef _WIN32
#include <windows.h>
#endif
/**
* Locks. Wrapper on pthread_mutex
*/
class Mutex
{
#ifdef _WIN32
    CRITICAL_SECTION m_lock;
#else
    pthread_mutex_t m_lock;
#endif
private:
    Mutex(const Mutex&); // protect from usage
    Mutex& operator=(const Mutex&); // protect from usage
public:
    explicit Mutex();
    void lock() const;
    void unlock() const;
    ~Mutex();
};
class MutexC
{
    friend class Condition;
    pthread_mutex_t m_lock;
private:
    MutexC(const MutexC&); // protect from usage
    MutexC& operator=(const MutexC&); // protect from usage
public:
    explicit MutexC();
    void lock() const;
    void unlock() const;
    ~MutexC();
};

/**
* Base class, wich add to inherited mutexable ability
*/
class Mutexable
{
    friend class MutexLocker;
    friend class MutexUnlocker;
private:
    Mutex m_lock;
};
/**
* Stack mutex locker
*/
class MutexLocker
{
private:
    const  Mutex* m_mutex;
public:
    explicit MutexLocker(const Mutex& m):m_mutex(&m)
    {
        m_mutex->lock();
    }
    explicit MutexLocker(const Mutex* m):m_mutex(m)
    {
        m_mutex->lock();
    }
    explicit MutexLocker(const Mutexable& m):m_mutex(&m.m_lock)
    {
        m_mutex->lock();
    }
    explicit MutexLocker(const Mutexable* m):m_mutex(&m->m_lock)
    {
        m_mutex->lock();
    }
    ~MutexLocker()
    {
        m_mutex->unlock();
    }
};
class MutexLockerC
{
private:
    const  MutexC* m_mutex;
public:
    explicit MutexLockerC(const MutexC& m):m_mutex(&m)
    {
        m_mutex->lock();
    }
    explicit MutexLockerC(const MutexC* m):m_mutex(m)
    {
        m_mutex->lock();
    }
    ~MutexLockerC()
    {
        m_mutex->unlock();
    }
};



#define M_LOCK(a) MutexLocker aaa_dummy_stackparam_lock(a)
#define M_LOCKC(a) MutexLockerC aaa_dummy_stackparam_lock(a)

/**
* Mutex unlocker
*/
class MutexUnlocker
{
private:
    const  Mutex* m_mutex;
public:
    explicit MutexUnlocker(const Mutex& m):m_mutex(&m)
    {
        m_mutex->unlock();
    }
    explicit MutexUnlocker(const Mutex* m):m_mutex(m)
    {
        m_mutex->unlock();
    }
    explicit MutexUnlocker(const Mutexable& m):m_mutex(&m.m_lock)
    {
        m_mutex->unlock();
    }
    explicit MutexUnlocker(const Mutexable* m):m_mutex(&m->m_lock)
    {
        m_mutex->unlock();
    }
    ~MutexUnlocker()
    {
        m_mutex->lock();
    }
};
#define M_UNLOCK(a) MutexUnlocker aaa_dummy_stackparam_unlock(a)


/**
* Wrapper on pthread_cond*
*/
class Condition
{
    pthread_cond_t m_cond;
    const  MutexC& m_mx;
public:
    Condition(const MutexC&m);
    ~Condition();
    void wait() const;
    void signal() const;
    void broadcast() const;
    void timedwait(const mtimespec& ts) const;

};
#endif
