#include "msockaddr_in.h"
#ifndef _WIN32
#include <netdb.h>
#ifdef __linux__
#include <linux/in6.h>
#endif

#endif
#include "commonError.h"
#include "mutexInspector.h"
#include "url.h"

//std::map<std::string,msockaddr_in::_addrInfo> msockaddr_in::host2AddrInfo;

outBuffer & operator<<(outBuffer&b,const msockaddr_in &a)
{
    return a.pack(b);
}
inBuffer & operator>>(inBuffer &b,msockaddr_in &a)
{
    return a.unpack(b);
}

bool msockaddr_in::operator<(const msockaddr_in&b) const
{
    if(family()<b.family()) return true;
    if(family()>b.family()) return false;

    if(family()==AF_INET)
    {
        if (u.sa4.sin_addr.s_addr<b.u.sa4.sin_addr.s_addr) return true;
        if (u.sa4.sin_addr.s_addr>b.u.sa4.sin_addr.s_addr) return false;
        if (u.sa4.sin_port<b.u.sa4.sin_port) return true;
        if (u.sa4.sin_port>b.u.sa4.sin_port) return false;
        return false;
    }
    else if(family()==AF_INET6)
    {
#ifdef __MACH__
        for(size_t i=0;i<sizeof(u.sa6.sin6_addr.__u6_addr.__u6_addr8);i++)
        {
            if (u.sa6.sin6_addr.__u6_addr.__u6_addr8[i]<b.u.sa6.sin6_addr.__u6_addr.__u6_addr8[i]) return true;
            if (u.sa6.sin6_addr.__u6_addr.__u6_addr8[i]>b.u.sa6.sin6_addr.__u6_addr.__u6_addr8[i]) return false;
        }
#elif defined(__ANDROID__)
        for(size_t i=0;i<sizeof(u.sa6.sin6_addr.in6_u.u6_addr8);i++)
        {
            if (u.sa6.sin6_addr.in6_u.u6_addr8[i]<b.u.sa6.sin6_addr.in6_u.u6_addr8[i]) return true;
            if (u.sa6.sin6_addr.in6_u.u6_addr8[i]>b.u.sa6.sin6_addr.in6_u.u6_addr8[i]) return false;
        }
#else
        for(size_t i=0;i<sizeof(u.sa6.sin6_addr.__in6_u.__u6_addr8);i++)
        {
            if (u.sa6.sin6_addr.__in6_u.__u6_addr8[i]<b.u.sa6.sin6_addr.__in6_u.__u6_addr8[i]) return true;
            if (u.sa6.sin6_addr.__in6_u.__u6_addr8[i]>b.u.sa6.sin6_addr.__in6_u.__u6_addr8[i]) return false;
        }

#endif
        if (u.sa6.sin6_port<b.u.sa6.sin6_port) return true;
        if (u.sa6.sin6_port>b.u.sa6.sin6_port) return false;
        return false;
    }
    else
    {
        throw CommonError("invalid family");
    }

    return false;
}
bool msockaddr_in::operator==(const msockaddr_in&b) const
{
    if(family()!=b.family()) return false;
    if(family()==AF_INET)
    {
        if(u.sa4.sin_addr.s_addr!=b.u.sa4.sin_addr.s_addr) return false;
        if(u.sa4.sin_port!=b.u.sa4.sin_port) return false;
    }
    else if(family()==AF_INET6)
    {
        if(u.sa6.sin6_port != b.u.sa6.sin6_port) return false;
#ifdef __MACH__
        for(size_t i=0;i<sizeof(u.sa6.sin6_addr.__u6_addr.__u6_addr8);i++)
        {
            if (u.sa6.sin6_addr.__u6_addr.__u6_addr8[i]!=b.u.sa6.sin6_addr.__u6_addr.__u6_addr8[i])
                return false;
        }
#elif defined(__ANDROID__)
        for(size_t i=0;i<sizeof(u.sa6.sin6_addr.in6_u.u6_addr8);i++)
        {
            if (u.sa6.sin6_addr.in6_u.u6_addr8[i]!=b.u.sa6.sin6_addr.in6_u.u6_addr8[i]) return false;
        }
#else
        for(size_t i=0;i<sizeof(u.sa6.sin6_addr.__in6_u.__u6_addr8);i++)
        {
            if (u.sa6.sin6_addr.__in6_u.__u6_addr8[i]!=b.u.sa6.sin6_addr.__in6_u.__u6_addr8[i]) return false;
        }

#endif

    }
    return true;
}

outBuffer &msockaddr_in::pack(outBuffer &b) const
{
    if(family()==AF_INET)
    {
        b.put_PN(u.sa4.sin_family);
        b.put_PN(ntohs(u.sa4.sin_port));
        b.put_PN(ntohl(u.sa4.sin_addr.s_addr));
    }
    else if(family()==AF_INET6)
    {
        b.put_PN(u.sa6.sin6_family);
        b.put_PN(htons(u.sa4.sin_port));
        char str[100];
        inet_ntop(u.sa6.sin6_family, &u.sa6.sin6_addr, str,sizeof(str));
        b.put_PSTR(str);
    }

    return b;
}
inBuffer &msockaddr_in::unpack(inBuffer&b)
{

    uint8_t fam=b.get_PN();
    if(fam==AF_INET)
    {
        u.sa4.sin_family=fam;
        u.sa4.sin_port=htons(b.get_PN());
        u.sa4.sin_addr.s_addr=htonl(b.get_PN());
    }
    else if(fam==AF_INET6)
    {
        u.sa6.sin6_family=fam;
        u.sa6.sin6_port=htons(b.get_PN());
        std::string s=b.get_PSTR();
        inet_pton(u.sa6.sin6_family,s.c_str(),&u.sa6.sin6_addr);
    }
    else
        throw CommonError("invalid family %d",fam);
    return b;
}
std::string msockaddr_in::getStringAddr() const
{
    if(family()==AF_INET)
    {
        return inet_ntoa(u.sa4.sin_addr);
    }
    else if(family()==AF_INET6)
    {
        char str[100];
        inet_ntop(u.sa6.sin6_family, &u.sa6.sin6_addr, str,sizeof(str));
        return str;
    }
    else
        throw CommonError("invalid family %d",family());
}
unsigned short msockaddr_in::port() const
{
    if(family()==AF_INET)
    return ntohs(u.sa4.sin_port);
    else
        return ntohs(u.sa6.sin6_port);
}
//std::string msockaddr_in::inaddr() const
//{
//    return ntohl(u.sa4.sin_addr.s_addr);
//}

msockaddr_in::msockaddr_in(const msockaddr_in& n)
{
    memcpy(&u,&n.u,sizeof(n.u));
//    u.sa4.sin_family=n.u.sa4.sin_family;
//    u.sa4.sin_port=n.u.sa4.sin_port;
//    u.sa4.sin_addr=n.u.sa4.sin_addr;
}
std::string msockaddr_in::dump() const
{
    char s[200];
    if(family()==AF_INET)
    {
        snprintf(s,sizeof(s),"%s:%d",getStringAddr().c_str(), ntohs(u.sa4.sin_port));
    }
    else
    {
        snprintf(s,sizeof(s),"%s@%d",getStringAddr().c_str(), ntohs(u.sa4.sin_port));

    }
    //else throw CommonError("invalid fam");

    return s;

}

std::pair<std::set<std::string>,std::set<std::string> > msockaddr_in::getAddrInfo(const std::string& host)
{
    auto ai=iUtils->getAddrInfos();
    {
        M_LOCK(ai.operator ->());

        auto pa=ai->host2AddrInfo.find(host);
        if(pa!=ai->host2AddrInfo.end())
        {
            if(time(NULL)-pa->second.t<4*3600)
            {
                return pa->second.addrs;
            }
        }
    }

    struct addrinfo hints, *res, *p;
    int status;
    char ipstr[INET6_ADDRSTRLEN];



    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // AF_INET или AF_INET6 для указания версии протокола
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(host.c_str(), NULL, &hints, &res)) != 0) {
        throw CommonError("getaddrinfo: %sn", gai_strerror(status));
    }

//    printf("IP addresses for %s:\n", host.c_str());
    std::set<std::string> ipv4;
    std::set<std::string> ipv6;

    for(p = res;p != NULL; p = p->ai_next) {
        void *addr;
        char *ipver;


        // получаем указатель на адрес, по разному в разных протоколах
        if (p->ai_family == AF_INET) { // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver = "IPv4";
            //for(int i=0;i<4;i++)
            {
                auto addr=inet_ntoa(ipv4->sin_addr);
//                logErr2("addr %s",addr);
//                unsigned int a[4];
//                a[0]=addr>>24 & 0xff;
//                a[1]=addr>>16 & 0xff;
//                a[2]=addr>>8 & 0xff;
//                a[3]=addr>>0 & 0xff;
//                for(int i=0;i<4;i++)
//                {
//                    logErr2("a %02x",a[i]);
//                }
            }
        } else { // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
            for(int i=0;i<16;i++)
            {
#ifdef __MACH__

//                logErr2("A %02x",(unsigned char)ipv6->sin6_addr.__u6_addr.__u6_addr8[i]);
#else

//                logErr2("A %02x",(unsigned char)ipv6->sin6_addr.__in6_u.__u6_addr8[i]);
#endif

            }
            ipver = "IPv6";
        }

        // преобразуем IP в строку и выводим его:
        inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
//        printf("  %s: %s\n", ipver, ipstr);
        if(p->ai_family == AF_INET)
            ipv4.insert(ipstr);
        else
            ipv6.insert(ipstr);

    }

    freeaddrinfo(res); // free the linked list

    {
        M_LOCK(ai.operator ->());
        ai->host2AddrInfo.erase(host);
        ai->host2AddrInfo.insert(std::make_pair(host,_addrInfo(std::make_pair(ipv4,ipv6))));

    }
    return std::make_pair(ipv4,ipv6);
}

void msockaddr_in::init(const char* host, unsigned short port)
{
    XTRY;
    auto z=getAddrInfo(host);
    if(z.first.size())
    {
        u.sa4.sin_family=AF_INET;
        u.sa4.sin_port=htons(port);
        u.sa4.sin_addr.s_addr=inet_addr(z.first.begin()->c_str());
    }
    else if(z.second.size())
    {
        u.sa6.sin6_family=AF_INET6;
        u.sa6.sin6_port=htons(port);
        inet_pton(u.sa6.sin6_family,z.second.begin()->c_str(),&u.sa6.sin6_addr);

    }
    /*u.sa4.sin_port=htons(port);
    bool isA=false;
    size_t len=strlen(host);
    for(size_t i=0; i<len; i++)
    {
        if(host[i]>='a'&& host[i]<'z') isA=true;
        if(host[i]>='A'&& host[i]<'Z') isA=true;

    }

    if(!isA)
    {
        //if(strchr(host,'.')==NULL)
        u.sa4.sin_addr.s_addr=inet_addr(host);
    }
    else
    {
        uint32_t addr;
        if(iUtils->getHostByName(host,addr))
        {
            throw CommonError("getHostByName: (%s) errno %d %s",host,errno,_DMI().c_str());
        }
        u.sa4.sin_addr.s_addr=addr;
    }*/
    XPASS;
}
void msockaddr_in::setPort(const unsigned short& port)
{
    if(family()==AF_INET)
        u.sa4.sin_port=htons(port);
    else
        u.sa6.sin6_port=htons(port);
}
void msockaddr_in::init(const Url& u)
{
    init(u.host.c_str(),atoi(u.port.c_str()));
}
//void msockaddr_in::setInaddr(const uint32_t& v)
//{
//    u.sa4.sin_addr.s_addr=htonl(v);
//}

static bool check_addr_for_mask(in_addr a, const char *ipmask, int maskShift)
{
    unsigned int val=ntohl(a.s_addr);
    /*unsigned char *ps=(unsigned char *)&a.s_addr;
    unsigned char *pd=(unsigned char *)&val;
    pd[0]=ps[3];
    pd[1]=ps[2];
    pd[2]=ps[1];
    pd[3]=ps[0];*/

    int shift=32-maskShift;
    val>>=shift;
    val<<=shift;

    a.s_addr=htonl(val);
//    ps[0]=pd[3];
//    ps[1]=pd[2];
//    ps[2]=pd[1];
//    ps[3]=pd[0];
    std::string rest=inet_ntoa(a);
    return rest==ipmask;

}
bool msockaddr_in::isNAT() const
{
    if(family()==AF_INET)
    {
        /**
            10.0.0.0/8       (10.0.0.0 - 10.255.255.255)
            172.16.0.0/12    (172.16.0.0 - 172.31.255.255)
            192.168.0.0/16   (192.168.0.0 - 192.168.255.255)

        */
        if(check_addr_for_mask(u.sa4.sin_addr,"10.0.0.0",8))
            return true;
        if(check_addr_for_mask(u.sa4.sin_addr,"172.16.0.0",12))
            return true;
        if(check_addr_for_mask(u.sa4.sin_addr,"192.168.0.0",16))
            return true;
    }

    return false;
}
std::string  msockaddr_in::asString() const
{
    outBuffer o;
    pack(o);
    return o.asString()->asString();
}
void msockaddr_in::initFromUrl(const std::string &url)
{
    Url u;
    u.parse(url);
    init(u);
}

socklen_t msockaddr_in::addrLen() const
{
    if(family()==AF_INET)
        return sizeof(sockaddr_in);
    else
        return sizeof(sockaddr_in6);
}
socklen_t msockaddr_in::maxAddrLen() const
{
    return sizeof(u);
}
sockaddr* msockaddr_in::addr() const
{
    return (sockaddr*)&u;
}
int8_t msockaddr_in::family() const
{
    return u.sa4.sin_family;
}

