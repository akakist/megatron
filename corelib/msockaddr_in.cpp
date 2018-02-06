#include "msockaddr_in.h"
#ifndef _WIN32
#include <netdb.h>
#endif
#include "commonError.h"
#include "mutexInspector.h"
#include "url.h"


outBuffer & operator<<(outBuffer&b,const msockaddr_in &a)
{
    return a.pack(b);
}
inBuffer & operator>>(inBuffer &b,msockaddr_in &a)
{
    return a.unpack(b);
}

int msockaddr_in::cmp(const msockaddr_in& b)const
{
    if (sin_addr.s_addr<b.sin_addr.s_addr) return true;
    if (sin_addr.s_addr>b.sin_addr.s_addr) return false;
    if (sin_port<b.sin_port) return true;
    if (sin_port>b.sin_port) return false;
    return false;
}

outBuffer &msockaddr_in::pack(outBuffer &b) const
{
    b.put_PN(sin_family);
    b.put_PN(sin_port);
    b.put_PN(sin_addr.s_addr);
    return b;
}
inBuffer &msockaddr_in::unpack(inBuffer&b)
{
    sin_family=b.get_PN();
    sin_port=b.get_PN();
    sin_addr.s_addr=b.get_PN();
    return b;
}
std::string msockaddr_in::getStringAddr() const
{
    return inet_ntoa(sin_addr);
}
unsigned short msockaddr_in::port() const
{
    return ntohs(sin_port);
}
uint32_t msockaddr_in::inaddr() const
{
    return ntohl(sin_addr.s_addr);
}

msockaddr_in::msockaddr_in(const msockaddr_in& n)
{
    sin_family=n.sin_family;
    sin_port=n.sin_port;
    sin_addr=n.sin_addr;
}
std::string msockaddr_in::dump() const
{
    char s[200];
    snprintf(s,sizeof(s),"%s:%d",inet_ntoa(sin_addr), htons(sin_port));
    return s;

}


void msockaddr_in::init(const char* host, unsigned short port)
{
    XTRY;
    sin_port=htons(port);
    bool isA=false;
    size_t len=strlen(host);
    for(size_t i=0; i<len; i++)
    {
        if(host[i]>='a'&& host[i]<'z') isA=true;
        if(host[i]>='A'&& host[i]<'Z') isA=true;

    }

    if(!isA)
    {
        sin_addr.s_addr=inet_addr(host);
    }
    else
    {
        uint32_t addr;
        if(iUtils->getHostByName(host,addr))
        {
            throw CommonError("getHostByName: (%s) errno %d %s",host,errno,_DMI().c_str());
        }
        sin_addr.s_addr=addr;
    }
    XPASS;
}
void msockaddr_in::setPort(const unsigned short& port)
{
    sin_port=htons(port);
}
void msockaddr_in::init(const Url& u)
{
    init(u.host.c_str(),atoi(u.port.c_str()));
}
void msockaddr_in::setInaddr(const uint32_t& v)
{
    sin_addr.s_addr=htonl(v);
}

static bool check_addr_for_mask(in_addr a, const char *ipmask, int maskShift)
{
    unsigned int val;
    unsigned char *ps=(unsigned char *)&a.s_addr;
    unsigned char *pd=(unsigned char *)&val;
    pd[0]=ps[3];
    pd[1]=ps[2];
    pd[2]=ps[1];
    pd[3]=ps[0];

    int shift=32-maskShift;
    val>>=shift;
    val<<=shift;

    ps[0]=pd[3];
    ps[1]=pd[2];
    ps[2]=pd[1];
    ps[3]=pd[0];
    std::string rest=inet_ntoa(a);
    return rest==ipmask;

}
bool msockaddr_in::isNAT() const
{
    /**
        10.0.0.0/8       (10.0.0.0 - 10.255.255.255)
        172.16.0.0/12    (172.16.0.0 - 172.31.255.255)
        192.168.0.0/16   (192.168.0.0 - 192.168.255.255)

    */
    if(check_addr_for_mask(sin_addr,"10.0.0.0",8))
        return true;
    if(check_addr_for_mask(sin_addr,"172.16.0.0",12))
        return true;
    if(check_addr_for_mask(sin_addr,"192.168.0.0",16))
        return true;
    return false;
}
std::string  msockaddr_in::asString() const
{
    outBuffer o;
    pack(o);
    return o.asString()->asString();
}
msockaddr_in::msockaddr_in(const std::string &url)
{
    Url u;
    u.parse(url);
    init(u);
}
