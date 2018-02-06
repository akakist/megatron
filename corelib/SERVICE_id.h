#ifndef _______________SERVICE___ID__H
#define _______________SERVICE___ID__H
#include <sys/types.h>
#include <stdint.h>
#include "ioBuffer.h"
//#ifdef DEBUG
struct SERVICE_id
{
    enum
    {
        ID_INT,
        ID_STRING,
        ID_UNDEF

    };

    SERVICE_id(size_t i):type(ID_INT),i_id(i) {}
    SERVICE_id(const std::string& i):type(ID_STRING),s_id(i) {}
    SERVICE_id():type(ID_UNDEF) {}

    int getType() {
        return type;
    }
    std::string getSid() const
    {
        if(type!=ID_STRING)
            throw CommonError("SERVICE_id::getSid: if(type!=ID_STRING)");
        return s_id;
    }
    size_t getIid() const
    {
        if(type!=ID_INT)
            throw CommonError("SERVICE_id::getSid: if(type!=ID_INT)");
        return i_id;
    }
private:
    unsigned char type;
    size_t i_id;
    std::string s_id;

    friend int operator<(const SERVICE_id&a, const SERVICE_id&b);
    friend int operator==(const SERVICE_id&a, const SERVICE_id&b);
    friend outBuffer& operator<< (outBuffer& b,const SERVICE_id& s);
    friend inBuffer& operator>> (inBuffer& b,  SERVICE_id& s);
    friend int operator!=(const SERVICE_id&a, const SERVICE_id&b);

public:
    std::string dump() const
    {
        char s[100];
        if(type==ID_INT)
        {
            snprintf(s,sizeof(s),"i-%zu",i_id);
            return s;
        }
        else
        {
            snprintf(s,sizeof(s),"s-%s",s_id.c_str());
            return s;
        }
    }
};
inline int operator<(const SERVICE_id&a, const SERVICE_id&b)
{
    if(a.type!=b.type)
        return a.type<b.type;
    if(a.type==SERVICE_id::ID_INT)
        return a.i_id<b.i_id;
    else if(a.type==SERVICE_id::ID_STRING)
        return a.s_id<b.s_id;
    else throw CommonError("INVALID CASE %s %d",__FILE__,__LINE__);

    //return a.container<b.container;
}
inline int operator==(const SERVICE_id&a, const SERVICE_id&b)
{
    if(a.type==b.type)
    {
        if(a.type==SERVICE_id::ID_INT)
            return a.i_id==b.i_id;
        else if(a.type==SERVICE_id::ID_STRING)
            return a.s_id==b.s_id;
        else
            throw CommonError("INVALID CASE %s %d",__FILE__,__LINE__);

    }
    else
    {
        // logErr2("compare SERVICE_id with different types");
    }
    return false;
}

inline outBuffer& operator<< (outBuffer& b,const SERVICE_id& s)
{
    b<<s.type;
    if(s.type==SERVICE_id::ID_INT)
        b<<s.i_id;
    else if(s.type==SERVICE_id::ID_STRING)
        b<<s.s_id;
    else
        throw CommonError("INVALID CASE %s %d",__FILE__,__LINE__);

    return b;
}
inline inBuffer& operator>> (inBuffer& b,  SERVICE_id& s)
{

    b>>s.type;
    if(s.type==SERVICE_id::ID_INT)
        b>>s.i_id;
    else if(s.type==SERVICE_id::ID_STRING)
        b>>s.s_id;
    else
        throw CommonError("INVALID CASE %s %d",__FILE__,__LINE__);
    return b;
}
inline int operator!=(const SERVICE_id&a, const SERVICE_id&b)
{
    if(a.type!=b.type)
        return true;
    if(a.type==SERVICE_id::ID_INT)
    {
        if(a.i_id!=b.i_id)
            return true;
    }
    else
    {
        if(a.s_id!=b.s_id)
            return true;

    }
    return false;
}
#endif
