#pragma once

#include "REF.h"
#include <string>
#include <inttypes.h>
#include "commonError.h"

/// refbuffer - string replacement with recount
struct refbuffer: public Refcountable
{
private:
public:
    uint8_t * buffer;
    size_t size_;
    size_t capacity;

    std::string asString()
    {
        if(size_ && buffer)
            return std::string((char*)buffer,size_);
        else return "";
    }
    refbuffer():buffer(NULL),size_(0),capacity(0) {}
    ~refbuffer()
    {
        if(buffer)
            ::free(buffer);
    }
    uint8_t * alloc(size_t sz)
    {
        if(size_>=sz && buffer)
            return buffer;
        if(buffer)
            free(buffer);
        buffer=(uint8_t*)malloc(sz+0x20);
        size_=sz+10;
        return buffer;
    }
};

inline REF_getter<refbuffer> toRef(const std::string&s)
{
    if(s.size())
    {
        REF_getter<refbuffer> r=new refbuffer;
        r->capacity=s.size()+0x20;
        r->buffer=(uint8_t*) malloc(r->capacity);
        if(!r->buffer) throw CommonError("alloc error for size %d",r->capacity);
        r->size_=s.size();
        memcpy(r->buffer,s.data(),s.size());
        return r;
    }
    REF_getter<refbuffer> r=new refbuffer;
    return r;
}
inline REF_getter<refbuffer> toRef(const uint8_t* buf, int size)
{
    if(size)
    {
        REF_getter<refbuffer> r=new refbuffer;
        r->capacity=size+0x20;
        r->buffer=(uint8_t*) malloc(r->capacity);
        if(!r->buffer) throw CommonError("alloc error for size %d",r->capacity);
        r->size_=size;
        memcpy(r->buffer,buf,size);
        return r;
    }
    REF_getter<refbuffer> r=new refbuffer;
    return r;
}

