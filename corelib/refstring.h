#ifndef ___________________________REFSTRING_H
#define ___________________________REFSTRING_H

#include "REF.h"
#include <string>
#include <inttypes.h>

/// refbuffer - string replacement with recount
struct refbuffer: public Refcountable
{
    uint8_t * buffer;
    size_t size;
    std::string asString()
    {
        if(size && buffer)
            return std::string((char*)buffer,size);
        else return "";
    }
    refbuffer():buffer(NULL),size(0) {}
    ~refbuffer()
    {
        if(buffer)
            ::free(buffer);
    }
    uint8_t * alloc(size_t sz)
    {
        if(size>=sz && buffer)
            return buffer;
        if(buffer)
            free(buffer);
        buffer=(uint8_t*)malloc(sz+10);
        size=sz+10;
        return buffer;
    }
};

inline REF_getter<refbuffer> toRef(const std::string&s)
{
    if(s.size())
    {
        REF_getter<refbuffer> r=new refbuffer;
        r->buffer=(uint8_t*) malloc(s.size());
        if(!r->buffer) throw CommonError("alloc error for size %d",s.size());
        r->size=s.size();
        memcpy(r->buffer,s.data(),s.size());
        return r;
    }
    REF_getter<refbuffer> r=new refbuffer;
    return r;
}
#endif // REFSTRING_H
