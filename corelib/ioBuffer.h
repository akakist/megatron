#ifndef ____________________BUFFER_H
#define ____________________BUFFER_H
#include <sys/types.h>
#include <stdint.h>
#include <vector>
#include <set>
#include <deque>
#include <string>
#include <map>
#include <list>
#include <string.h>
#include "commonError.h"
#include "refstring.h"

/**
* Класс байтового потока без битовых операций, но с темплейтами STL
*/
class inBuffer
{
public:


private:
    size_t out_pos;
    size_t m_size;
    const unsigned char *m_data;
public:
    ~inBuffer() {};
    inBuffer(const unsigned char *d, size_t size); // construct from an array
    inBuffer(const  char *d, size_t size); // construct from an array
    inBuffer(const  std::string&); // construct from string

    inBuffer(const  REF_getter<refbuffer>&); // construct from string

    size_t size() const;
    const unsigned char *data();
    size_t remains() const;
    bool beforeEnd() const;



public:
    std::string unpackRest();
    void unpack(std::string& s, int64_t size);
    void unpack_nothrow(std::string& s, size_t size, bool &success);
    void unpack(uint8_t* buf, size_t sz);


    unsigned char get_8();
    unsigned char get_8_nothrow(bool &success);

    uint64_t get_PN();
    uint64_t get_PN_nothrow(bool &success);
    std::string get_PSTR();
    std::string get_PSTR_nothrow(bool &success);

#ifdef _WIN32
    inBuffer& operator>>(long &);
#endif

#ifdef __MACH__
    inBuffer& operator>>(long &);
    inBuffer& operator>>(size_t &);
#endif
    inBuffer& operator>>(uint8_t &);
    inBuffer& operator>>(int8_t&);
    inBuffer& operator>>(uint16_t &);
    inBuffer& operator>>(int16_t&);
    inBuffer& operator>>(uint32_t &);
    inBuffer& operator>>(int32_t&);
    inBuffer& operator>>(uint64_t &);
    inBuffer& operator>>(int64_t&);
    inBuffer& operator>>(std::string&);
    inBuffer& operator>>(bool&);
//private:
    inBuffer& operator>>(double&);
    inBuffer& operator>>(float &);
public:
};
/**
* Класс байтового потока без битовых операций, но с темплейтами STL
*/
class outBuffer
{
private:
    outBuffer(const outBuffer&);             // Not defined to prevent usage
    outBuffer& operator=(const outBuffer&);  // Not defined to prevent usage
public:
    REF_getter<refbuffer> buffer;
    size_t bufsize;
    size_t cur_pos;

    outBuffer();

public:
    void construct();

    ~outBuffer();
    outBuffer& pack(const std::string& s);
    outBuffer& pack(const char * s, size_t len);
    outBuffer& pack(const unsigned char * s, size_t len);

    const unsigned char *data()const;
    unsigned char *const_data()const;

    REF_getter<refbuffer> asString() const;
    size_t size() const;
    void adjust(size_t n);
    void clear();
    outBuffer& put_8(unsigned char);
    outBuffer& put_PN(const uint64_t);
    outBuffer& put_PSTR(const std::string &);


    outBuffer& operator<<(const char*);
#ifdef __MACH__
    outBuffer& operator<<(const size_t&);
    outBuffer& operator<<(const long&);
#endif
    outBuffer& operator<<(const uint8_t&);
    outBuffer& operator<<(const int8_t&);
    outBuffer& operator<<(const uint16_t&);
    outBuffer& operator<<(const int16_t&);
    outBuffer& operator<<(const uint32_t&);
    outBuffer& operator<<(const int32_t&);
    outBuffer& operator<<(const uint64_t&);
    outBuffer& operator<<(const int64_t&);
    outBuffer& operator<<(const std::string&);
    outBuffer& operator<<(const bool&);
#ifdef _WIN32
    outBuffer& operator<<(const long&);
#endif
//private:
    outBuffer& operator<<(const double&);
    outBuffer& operator<<(const float&);
public:
private:
    outBuffer& put_8$(unsigned char c)
    {
        buffer->buffer[cur_pos++]=c;
        return *this;
    }
    outBuffer& Pack$(const std::string& s );
    outBuffer& put_PN$(const uint64_t &N);


};

template < class T > outBuffer & operator << (outBuffer& b,const std::vector < T > &v)
{
    b << (unsigned int)v.size();
#if !defined(_MSC_VER)
    for (auto j = v.begin(); j != v.end(); j++) b << *j;
#else
    for (std::vector < T >::const_iterator j = v.begin(); j != v.end(); j++) b << *j;
#endif

    return b;
}

template < class T > outBuffer & operator << (outBuffer& b,const std::list < T > &v)
{

    b << (unsigned int)v.size();
#if defined(_MSC_VER)
    for (std::list<T>::const_iterator j = v.begin(); j != v.end(); j++) b << *j;
#else
    for (auto  j = v.begin(); j != v.end(); j++) b << *j;
#endif
    return b;
}

template < class T > outBuffer & operator << (outBuffer& b,const std::deque < T > &v)
{

    b << (unsigned int)v.size();
#if defined (_MSC_VER)
    for (std::deque<T>::const_iterator j = v.begin(); j != v.end(); j++) b << *j;
#else
    for (auto j = v.begin(); j != v.end(); j++) b << *j;
#endif
    return b;
}
template < class T > outBuffer & operator << (outBuffer& b,const std::set < T > &v)
{

    b << (unsigned int)v.size();
#ifdef _MSC_VER
    for (std::set < T > ::const_iterator j = v.begin(); j != v.end(); j++) b << *j;
#else
    for (auto j = v.begin(); j != v.end(); j++) b << *j;
#endif
    return b;
}

template < class T1, class T2 > outBuffer & operator << (outBuffer& b,const std::map < T1, T2 > &v)
{
    b << (unsigned int)v.size();
#if defined (_MSC_VER)
    for (std::map < T1, T2 >::const_iterator j = v.begin(); j != v.end(); j++)
#else
    for (auto j = v.begin(); j != v.end(); j++)
#endif
    {
        b << j->first << j->second;
    }
    return b;
}

template < class T1, class T2 > outBuffer & operator << (outBuffer& b,const std::pair < T1, T2 > &v)
{
    b << v.first<<v.second;
    return b;
}


template < class T > inBuffer & operator >> (inBuffer& b,std::set < T > &v)
{
    v.clear();
    unsigned int n;
    b >> n;
    for (unsigned int j = 0; j < n; j++)
    {
        T t;
        b >> t;
        v.insert(t);
    }
    return b;
}
template < class T > inBuffer& operator >> (inBuffer& b,std::list < T > &v)
{
    v.clear();
    unsigned int n;
    b >> n;
    for (unsigned int j = 0; j < n; j++)
    {
        T t;
        b >> t;
        v.push_back(t);
    }
    return b;
}
template < class T1, class T2 > inBuffer & operator >> (inBuffer& b,std::map < T1, T2 > &v)
{
    v.clear();
    unsigned int n;
    b >> n;
    for (unsigned int j = 0; j < n; j++)
    {
        T1 t1;
        T2 t2;
        b >> t1;
        b >> t2;
        v.insert(std::pair<T1,T2>(t1,t2));
    }
    return b;
}
template < class T1, class T2 > inBuffer & operator >> (inBuffer& b,std::pair < T1, T2 > &v)
{
    b >> v.first;
    b >> v.second;
    return b;
}
template < class T > inBuffer & operator >> (inBuffer& b,std::vector < T > &v)
{
    v.clear();
    unsigned int n;
    b >> n;
    v.reserve(n);
    for (unsigned int j = 0; j < n; j++)
    {
        T t;
        b >> t;
        v.push_back(t);
    }
    return b;
}

template < class T > inBuffer & operator >> (inBuffer& b,std::deque < T > &v)
{
    v.clear();
    unsigned int n;
    b >> n;
    for (unsigned int j = 0; j < n; j++)
    {
        T t;
        b >> t;
        v.push_back(t);
    }
    return b;
}

inline unsigned char inBuffer::get_8_nothrow(bool &success)
{
    if (out_pos + 1 > m_size)
    {
        success=false;
        return 0;
    }
    success=true;
    unsigned char c= m_data[out_pos++];
    return c;
}
inline uint64_t inBuffer::get_PN()
{
    uint64_t NN=0;
    while (1)
    {
        unsigned char c=get_8();
        if (c<0x80)
        {
            NN<<=7;
            NN|=c;
            return NN;
        }
        else
        {
            NN<<=7;
            NN|=c&0x7F;

        }
    }
}
inline uint64_t inBuffer::get_PN_nothrow(bool &success)
{
    uint64_t NN=0;
    while (1)
    {
        unsigned char c=get_8_nothrow(success);
        if (!success)return 0;
        if (c<0x80)
        {
            NN<<=7;
            NN|=c;
            return NN;
        }
        else
        {
            NN<<=7;
            NN|=c&0x7F;

        }
    }
}
outBuffer & operator<< (outBuffer& b,const REF_getter<refbuffer> &s);
inBuffer & operator>> (inBuffer& b,  REF_getter<refbuffer> &s);



#endif
