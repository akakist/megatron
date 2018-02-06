#ifndef __________EVENT_ID___H
#define __________EVENT_ID___H
#include <stddef.h>
#include <sys/types.h>
#include <string>
#include "commonError.h"
#include "ioBuffer.h"
struct EVENT_id
{
    enum
    {
        ID_INT,
        ID_STRING,
        ID_UNDEF

    };

    EVENT_id(size_t i):type(ID_INT),i_id(i) {}
    EVENT_id(const std::string& i):type(ID_STRING),s_id(i) {}
    EVENT_id():type(ID_UNDEF) {}

    int getType() const {
        return type;
    }
    std::string getSid() const
    {
        if(type!=ID_STRING)
            throw CommonError("Event::getSid: if(type!=ID_STRING)");
        return s_id;
    }
    size_t getIid() const
    {
        if(type!=ID_INT)
            throw CommonError("Event::getSid: if(type!=ID_INT)");
        return i_id;
    }
private:
    unsigned char type;
    size_t i_id;
    std::string s_id;

    friend int operator<(const EVENT_id&a, const EVENT_id&b);
    friend int operator==(const EVENT_id&a, const EVENT_id&b);
    friend outBuffer& operator<< (outBuffer& b,const EVENT_id& s);
    friend inBuffer& operator>> (inBuffer& b,  EVENT_id& s);

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
inline int operator<(const EVENT_id&a, const EVENT_id&b)
{
    if(a.type!=b.type)
        return a.type<b.type;
    if(a.type==EVENT_id::ID_INT)
        return a.i_id<b.i_id;
    if(a.type==EVENT_id::ID_STRING)
        return a.s_id<b.s_id;
    throw CommonError("INVALID CASE %s %d",__FILE__,__LINE__);

}
inline int operator==(const EVENT_id&a, const EVENT_id&b)
{
    if(a.type==b.type)
    {
        if(a.type==EVENT_id::ID_INT)
            return a.i_id==b.i_id;
        else if(a.type==EVENT_id::ID_STRING)
            return a.s_id==b.s_id;
        else
            throw CommonError("INVALID CASE %s %d",__FILE__,__LINE__);

    }
    else
    {
    }
    return false;
}
inline outBuffer& operator<< (outBuffer& b,const EVENT_id& s)
{
    b<<s.type;
    if(s.type==EVENT_id::ID_INT)
        b<<s.i_id;
    else if(s.type==EVENT_id::ID_STRING)
        b<<s.s_id;
    else
        throw CommonError("INVALID CASE %s %d",__FILE__,__LINE__);
    return b;
}
inline inBuffer& operator>> (inBuffer& b,  EVENT_id& s)
{

    b>>s.type;
    if(s.type==EVENT_id::ID_INT)
        b>>s.i_id;
    else if(s.type==EVENT_id::ID_STRING)
        b>>s.s_id;
    else
        throw CommonError("INVALID CASE %s %d",__FILE__,__LINE__);
    return b;
}



#endif
