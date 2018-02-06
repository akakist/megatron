#include <stdlib.h>
#include <stdio.h>
#include "ioBuffer.h"
#include "IInstance.h"
#include "mutexInspector.h"



inBuffer::inBuffer(const unsigned char* d, size_t siz) :  out_pos(0), m_size(siz), m_data(d) { }
inBuffer::inBuffer(const char* d, size_t siz) :  out_pos(0), m_size(siz), m_data(( unsigned char*)d) { }

inBuffer::inBuffer(const std::string& s) :  out_pos(0), m_size(s.size()) , m_data(( unsigned char*)s.data()) { }
inBuffer::inBuffer(const REF_getter<refbuffer>& s) :  out_pos(0), m_size(s->size) , m_data(( unsigned char*)s->buffer) { }

unsigned char inBuffer::get_8()
{
    if (out_pos + 1 > m_size)
    {
        throw CommonError("inBuffer::get_8: noenough "+_DMI());
    }
    unsigned char c= m_data[out_pos++];
    return c;
}

outBuffer& outBuffer::pack(const std::string& s)
{
    XTRY;
    adjust(s.size());
    memcpy(&buffer->buffer[cur_pos],s.data(),s.size());
    cur_pos+=s.size();
    XPASS;
    return *this;
}
outBuffer& outBuffer::pack(const char * s, size_t len)
{
    XTRY;
    adjust(len);
    memcpy(&buffer->buffer[cur_pos],s,len);
    cur_pos+=len;
    XPASS;
    return *this;
}
outBuffer& outBuffer::pack(const unsigned char * s, size_t len)
{
    XTRY;
    adjust(len);
    memcpy(&buffer->buffer[cur_pos],s,len);
    cur_pos+=len;
    XPASS;
    return *this;
}

void inBuffer::unpack(uint8_t* buf, size_t sz)
{
    if (sz > m_size-out_pos)
    {
        throw CommonError("unpack error not enough");
    }
    memcpy(buf,&m_data[out_pos],sz);
    out_pos += sz;

}
void inBuffer::unpack_nothrow(std::string& s, size_t size, bool &success)
{
    XTRY;
    if (size > m_size-out_pos)
    {
        success=false;
        return;
    }
    s=std::string((char*)&m_data[out_pos],size);
    out_pos += size;
    success=true;
    XPASS;
}
std::string inBuffer::unpackRest()
{
    XTRY;
    std::string s;
    unpack(s,remains());
    return s;
    XPASS;
}

void inBuffer::unpack(std::string& s, int64_t size)
{
    XTRY;
    bool success;
    unpack_nothrow(s,size,success);
    if (!success) throw CommonError("inBuffer unpack:  buffer size to small sz=%ld remains=%d %s",size,remains(),_DMI().c_str());
    XPASS;
}
void outBuffer::clear()
{
    cur_pos=0;
}
REF_getter<refbuffer> outBuffer::asString() const
{

    buffer->size=cur_pos;
    buffer->buffer=(uint8_t*)realloc(buffer->buffer,buffer->size);
    return buffer;
}
outBuffer::outBuffer():buffer(NULL), bufsize(0),cur_pos(0)
{
    construct();
}
#define SPLICE__ 0x400
void outBuffer::construct()
{
    buffer=new refbuffer;

    buffer->buffer=(unsigned char*)malloc(SPLICE__);
    if (buffer==NULL) throw CommonError("alloc error %s %d",__FILE__,__LINE__);
    bufsize=SPLICE__;
    cur_pos=0;

}

outBuffer::~outBuffer()
{

}
void outBuffer::adjust(size_t n)
{

    XTRY;
    if (cur_pos+n>=bufsize)
    {
        size_t nnew=cur_pos+n+0x10000;
        buffer->buffer=(unsigned char*)realloc(buffer->buffer,nnew);
        if (!buffer->buffer) throw CommonError("alloc error sz=%d %s %d",nnew,__FILE__,__LINE__);
        bufsize=nnew;
    }

    XPASS;
}

outBuffer& outBuffer::put_8(unsigned char c)
{
    XTRY;
    adjust(1);
    buffer->buffer[cur_pos++]=c;
    XPASS;
    return *this;

}
outBuffer& outBuffer::put_PN(uint64_t N)
{
    return put_PN$(N);
}
std::string inBuffer::get_PSTR()
{
    XTRY;
    uint64_t sz=get_PN();
    std::string r;
    unpack(r,sz);
    return r;
    XPASS;
}
std::string inBuffer::get_PSTR_nothrow(bool &success)
{
    std::string container = "";
    XTRY;

    uint64_t sz=get_PN_nothrow(success);
    if (!success) return "";
    if (remains()<sz)
    {
        success=false;
        return "";
    }
    for (size_t i = 0; i < sz && beforeEnd(); i++)
    {
        unsigned char c=get_8_nothrow(success);
        if (!success) return "";
        container += std::string((char *)&c, 1);
    }
    XPASS;
    return container;
}
outBuffer& outBuffer::put_PSTR(const std::string & c)
{
    XTRY;
    adjust(10+c.size());
    put_PN$((unsigned long)c.size());
    Pack$(c);
    XPASS;
    return *this;
}
#ifdef _WIN32
inBuffer& inBuffer::operator>>(long& p)
{
    XTRY;
    p=get_PN();
    XPASS;
    return *this;
}
#endif
#ifdef __MACH__
inBuffer& inBuffer::operator>>(long &p)
{
    p=get_PN();

    return *this;
}
inBuffer& inBuffer::operator>>(size_t &p)
{
    p=get_PN();

    return *this;
}
#endif

inBuffer& inBuffer::operator>>(double& p)
{

    XTRY;
    Rational r;
    *this>>r;
    p=r.v;
    XPASS;
    return *this;
}
inBuffer& inBuffer::operator>>(float& p)
{

    XTRY;
    Rational r;
    *this>>r;
    p=r.v;
    XPASS;
    return *this;
}

inBuffer& inBuffer::operator>>(int8_t& p)
{
    XTRY;
    p=get_PN();
    XPASS;
    return *this;
}
inBuffer& inBuffer::operator>>(uint8_t& p)
{
    XTRY;
    p=get_PN();
    XPASS;
    return *this;
}
inBuffer& inBuffer::operator>>(int16_t& p)
{
    XTRY;
    p=get_PN();
    XPASS;
    return *this;
}
inBuffer& inBuffer::operator>>(uint16_t& p)
{
    XTRY;
    p=get_PN();
    XPASS;
    return *this;
}
inBuffer& inBuffer::operator>>(int32_t& p)
{
    XTRY;
    p=get_PN();
    XPASS;
    return *this;
}
inBuffer& inBuffer::operator>>(uint32_t& p)
{
    XTRY;
    p=get_PN();
    XPASS;
    return *this;
}
inBuffer& inBuffer::operator>>(int64_t& p)
{
    XTRY;
    p=get_PN();
    XPASS;
    return *this;
}
inBuffer& inBuffer::operator>>(uint64_t& p)
{
    XTRY;
    p=get_PN();
    XPASS;
    return *this;
}


inBuffer& inBuffer::operator>>(std::string&p)
{
    XTRY;
    p=get_PSTR();
    XPASS;
    return *this;

}
inBuffer& inBuffer::operator>>(bool& p)
{
    XTRY;
    p=get_PN();
    XPASS;
    return *this;

}

#ifdef _WIN32
outBuffer& outBuffer::operator<<(const long&c)
{
    XTRY;
    put_PN(c);
    XPASS;
    return *this;
}
#endif

#ifdef __MACH__
outBuffer& outBuffer::operator<<(const size_t&c)
{
    put_PN(c);
    return *this;
}
outBuffer& outBuffer::operator<<(const long&c)
{
    put_PN(c);
    return *this;
}
#endif
outBuffer& outBuffer::operator<<(const double &c)
{
    XTRY;
    Rational r=c;
    *this<<r;
    //put_PN(c);
    XPASS;
    return *this;
}
outBuffer& outBuffer::operator<<(const float &c)
{
    XTRY;
    Rational r=c;
    *this<<r;
    //put_PN(c);
    XPASS;
    return *this;
}

outBuffer& outBuffer::operator<<(const int8_t &c)
{
    XTRY;
    put_PN(c);
    XPASS;
    return *this;
}
outBuffer& outBuffer::operator<<(const uint8_t &c)
{
    XTRY;
    put_PN(c);
    XPASS;
    return *this;
}
outBuffer& outBuffer::operator<<(const int16_t &c)
{
    XTRY;
    put_PN(c);
    XPASS;
    return *this;
}
outBuffer& outBuffer::operator<<(const uint16_t &c)
{
    XTRY;
    put_PN(c);
    XPASS;
    return *this;
}
outBuffer& outBuffer::operator<<(const int32_t &c)
{
    XTRY;
    put_PN(c);
    XPASS;
    return *this;
}
outBuffer& outBuffer::operator<<(const uint32_t &c)
{
    XTRY;
    put_PN(c);
    XPASS;
    return *this;
}
outBuffer& outBuffer::operator<<(const int64_t &c)
{
    XTRY;
    put_PN(c);
    XPASS;
    return *this;
}
outBuffer& outBuffer::operator<<(const uint64_t &c)
{
    XTRY;
    put_PN(c);
    XPASS;
    return *this;
}
outBuffer& outBuffer::operator<<(const std::string& c)
{
    XTRY;
    put_PSTR(c);
    XPASS;
    return *this;
}
outBuffer& outBuffer::operator<<(const char* c)
{
    XTRY;
    put_PSTR(c);
    XPASS;
    return *this;
}
outBuffer& outBuffer::operator<<(const bool &c)
{
    XTRY;
    put_PN(c);
    XPASS;
    return *this;
}

size_t outBuffer::size() const
{
    return cur_pos;
}

outBuffer& outBuffer::put_PN$(const uint64_t &N)
{
    XTRY;
    adjust(10);
    int start=56;

    for (int i=start; i>=0; i-=7)
    {
        unsigned char c=(N>>i) & 0x7F;
        if (c || i==0)
        {
            for (; i>=0; i-=7)
            {
                if (i) c=(N>>i) & 0x7F;
                else c=N & 0x7F;
                if (i)
                    put_8$(c|0x80);
                else
                    put_8$(c);
            }
            return *this;
        }

    }
    XPASS;
    return *this;
}

outBuffer& outBuffer::Pack$(const std::string& s )
{
    memcpy(&buffer->buffer[cur_pos],s.data(),s.size());
    cur_pos+=(int)s.size();
    return *this;
}

size_t inBuffer::size() const
{
    return m_size;
}
const unsigned char *inBuffer::data()
{
    return m_data;
}
size_t inBuffer::remains() const
{
    return m_size - out_pos;
}
bool inBuffer::beforeEnd() const
{
    return (out_pos < m_size);
}
const unsigned char *outBuffer::data()const
{
    return buffer->buffer;
}
unsigned char *outBuffer::const_data()const
{
    return buffer->buffer;
}

outBuffer & operator<< (outBuffer& b,const REF_getter<refbuffer> &s)
{
    
    bool packed=false;
    if(s.valid())
    {
        if(s->buffer)
        {
            b.put_PN(s->size);
            b.pack(s->buffer,s->size);
            packed=true;
        }
    }
    if(!packed)
        b.put_PN(0);

    return b;
}
inBuffer & operator>> (inBuffer& b,  REF_getter<refbuffer> &s)
{
    
    size_t size=b.get_PN();
    s=new refbuffer;
    if(size)
    {
        s->buffer=(uint8_t*)malloc(size);
        if(!s->buffer) throw CommonError("memory alloc error %d",size);
        s->size=size;
        b.unpack(s->buffer,s->size);
    }
    return b;
}
