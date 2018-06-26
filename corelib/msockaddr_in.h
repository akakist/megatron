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
struct _addrInfo{
    std::pair<std::set<std::string>,std::set<std::string> > addrs;
    time_t t;
    _addrInfo(const std::pair<std::set<std::string>,std::set<std::string> > &addrs_)
    {
        addrs=addrs_;
        t=time(NULL);
    }
};
struct _addrInfos: public Refcountable, public Mutexable{

    std::map<std::string,
        _addrInfo
    > host2AddrInfo;

};

class msockaddr_in
{

public:
    msockaddr_in()
    {
        u.sa4.sin_family=AF_INET;
        u.sa4.sin_addr.s_addr=INADDR_ANY;
        u.sa4.sin_port=0;

    }
private:

    std::pair<std::set<std::string>,std::set<std::string> > getAddrInfo(const std::string& host);

public:
    union _addr
    {
    sockaddr_in sa4;
    sockaddr_in6 sa6;
    } u;
public:
    void initFromUrl(const std::string &url);
    std::string dump() const;
    std::string getStringAddr() const;
    unsigned short port() const;
    void setPort(const unsigned short& port);
    msockaddr_in(const msockaddr_in& n);
    outBuffer &pack(outBuffer&) const;
    inBuffer &unpack(inBuffer &);
    int cmp(const msockaddr_in& a)const;
    void init(const char* hostname, unsigned short port);
    void init(const Url& url);
    std::string  asString() const;

    bool operator<(const msockaddr_in&b) const;
    bool operator==(const msockaddr_in&b) const;
    bool operator!=(const msockaddr_in&b) const
    {
        return  !operator ==(b);
    }
    bool isNAT() const;

    socklen_t addrLen() const;
    socklen_t maxAddrLen() const;
    sockaddr* addr() const;
    int8_t family() const;

};
outBuffer & operator<<(outBuffer&b,const msockaddr_in &a);
inBuffer & operator>>(inBuffer &b,msockaddr_in &a);



#endif
