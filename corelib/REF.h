#ifndef ______REF__H
#define ______REF__H
#include "mutexable.h"

#ifdef _MSC_VER
#include <time.h>
#endif

/// base class and template of smart pointer with refcount, which can catch pointer many times.
///
#define PRIVATEMUTEX
template < class T > class REF_getter;
class Refcountable
{
    friend class REF_getter < Refcountable >;
    Refcountable (const Refcountable &);	// Not defined  to prevent usage
    Refcountable & operator= (const Refcountable &);	// Not defined  to prevent usage

public:
#ifdef PRIVATEMUTEX
    Mutex __privateMuteX;
#endif
    int __Ref_Count;
    Refcountable (): __Ref_Count(0) { }

    int get_ref_count () const
    {
        return __Ref_Count;
    };
    virtual ~ Refcountable ()
    {
        if(__Ref_Count!=0)
        {
            logErr2("destructor with __Ref_Count!=0");
        }
    }
};

///      Smart pointer template
template < class T > class REF_getter
{

public:
    T * ___ptr;
    void increfcount () const
    {
        if (___ptr)
        {
#ifdef PRIVATEMUTEX
            M_LOCK (___ptr->__privateMuteX);
#endif
            ___ptr->__Ref_Count++;
        }
    }
    void decrefcount ()
    {
        bool need_destroy = false;
        {
            if (___ptr)
            {
#ifdef PRIVATEMUTEX
                M_LOCK (___ptr->__privateMuteX);
#endif
                if (___ptr->__Ref_Count == 1)
                    need_destroy = true;
                if (___ptr->__Ref_Count > 0)
                    ___ptr->__Ref_Count--;
            }
            if (need_destroy)
            {
                delete ___ptr;
                ___ptr = NULL;
            }
        }
    }

    bool operator < (const REF_getter & l) const
    {
        return ___ptr < l.___ptr;
    }
    bool valid() const
    {
        return ___ptr!=NULL;
    }
    REF_getter (const T * p):___ptr ((T *) p)
    {
        increfcount ();
    }
    REF_getter (const REF_getter < T > &bcg):___ptr (bcg.___ptr)
    {
        increfcount ();
    }

    ~REF_getter ()
    {
        decrefcount ();
    }
    bool operator== (const REF_getter & bcg) const
    {
        return ___ptr == bcg.___ptr;
    }
#ifndef _MSC_VER
    REF_getter & operator = (const REF_getter & bcg) const
    {
        if (this != &bcg)
        {
            bcg.increfcount ();
            decrefcount ();
            ___ptr = bcg.___ptr;
        }
        return *this;
    }
#endif
    REF_getter & operator = (const REF_getter & bcg)
    {
        if (this != &bcg)
        {
            bcg.increfcount ();
            decrefcount ();
            ___ptr = bcg.___ptr;
        }
        return *this;
    }
    inline  T* operator -> () const
    {
        return ___ptr;
    };
};

template < class T1,class T2 > class REF_getter2
{

public:
    T1 * ___ptr;
    void increfcount () const
    {
        if (___ptr)
        {
#ifdef PRIVATEMUTEX
            M_LOCK (___ptr->__privateMuteX);
#endif
            ___ptr->__Ref_Count++;
        }
    }
    void decrefcount ()
    {
        bool need_destroy = false;
        {
            if (___ptr)
            {
#ifdef PRIVATEMUTEX
                M_LOCK (___ptr->__privateMuteX);
#endif
                if (___ptr->__Ref_Count == 1)
                    need_destroy = true;
                if (___ptr->__Ref_Count > 0)
                    ___ptr->__Ref_Count--;
            }
            if (need_destroy)
            {
                delete ___ptr;
                ___ptr = NULL;
            }
        }
    }

    bool operator < (const REF_getter2 & l) const
    {
        return ___ptr < l.___ptr;
    }
    bool valid() const
    {
        return ___ptr!=NULL;
    }
    REF_getter2 ():___ptr (NULL)
    {
        increfcount ();
    }
    REF_getter2 (const T1 * p):___ptr ((T1 *) p)
    {
        increfcount ();
    }

    REF_getter2 (const REF_getter2 < T1,T2 > &bcg):___ptr (bcg.___ptr)
    {
        increfcount ();
    }

    ~REF_getter2 ()
    {
        decrefcount ();
    }
    bool operator== (const REF_getter2 & bcg) const
    {
        return ___ptr == bcg.___ptr;
    }
    operator T2* () const
    {
        return ___ptr->obj();
    }
    void clear()
    {
        decrefcount ();
        ___ptr=NULL;
    }
#ifndef _MSC_VER
    REF_getter2 & operator = (const REF_getter2 & bcg) const
    {
        if (this != &bcg)
        {
            bcg.increfcount ();
            decrefcount ();
            ___ptr = bcg.___ptr;
        }
        return *this;
    }
#endif
    REF_getter2 & operator = (const REF_getter2 & bcg)
    {
        if (this != &bcg)
        {
            bcg.increfcount ();
            decrefcount ();
            ___ptr = bcg.___ptr;
        }
        return *this;
    }
    inline  T2* operator -> () const
    {
        return ___ptr->obj();
    };
};

#endif
