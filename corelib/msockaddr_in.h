#ifndef ___________MSOCKADDR_IN____H
#define ___________MSOCKADDR_IN____H

#ifndef _WIN32
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#else
#include <winsock.h>
#endif
#include <string>
#include "SOCKET_id.h"
#include "CONTAINER.h"
#ifndef in_addr_t
#define in_addr_t unsigned int
#endif
struct Url;
class inBuffer;
class outBuffer;

/**
* C++ wrapper for sockaddr_in
*/

class msockaddr_in:public sockaddr_in
{

public:
    msockaddr_in()
    {
        sin_family=AF_INET;
        sin_addr.s_addr=INADDR_ANY;
        sin_port=0;

    }
    std::string dump() const;
    std::string getStringAddr() const;
    uint32_t inaddr() const;
    void setInaddr(const uint32_t&);
    unsigned short port() const;
    void setPort(const unsigned short& port);
    msockaddr_in(const msockaddr_in& n);
    outBuffer &pack(outBuffer&) const;
    inBuffer &unpack(inBuffer &);
    int cmp(const msockaddr_in& a)const;
    void init(const char* hostname, unsigned short port);
    void init(const Url& url);
    msockaddr_in(const std::string &url);
    std::string  asString() const;

    bool operator<(const msockaddr_in&b) const
    {
        return cmp(b);
    }
    bool operator==(const msockaddr_in&b) const
    {
        return sin_addr.s_addr==b.sin_addr.s_addr && sin_port==b.sin_port && sin_family==b.sin_family;
    }
    bool operator!=(const msockaddr_in&b) const
    {
        return sin_addr.s_addr!=b.sin_addr.s_addr || sin_port!=b.sin_port || sin_family!=b.sin_family;
    }
    bool isNAT() const;


};
outBuffer & operator<<(outBuffer&b,const msockaddr_in &a);
inBuffer & operator>>(inBuffer &b,msockaddr_in &a);



#endif
