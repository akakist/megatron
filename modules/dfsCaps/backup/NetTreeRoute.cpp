#include "NetTreeRoute.h"
#include <algorithm>
#include "st_FILE.h"
#include <sys/stat.h>
#include "st_malloc.h"

NetTreeRoute NetTreeRouteGenerator::calcTreeRoute(const int32_t &ip)
{
    MUTEX_INSPECTOR;
    if(calcTreeRouteCache.count(ip))
    {
        return calcTreeRouteCache[ip];
    }
    msockaddr_in sa;
    sa.setInaddr(ip);
    NetTreeRoute  outTreeRoute;
    NetRec sr;
    sr.endIp=ip;
    std::vector<NetRec>::iterator i=lower_bound(m_netArray.begin(),m_netArray.end(),sr);
    if(i==m_netArray.end())
    {
        MUTEX_INSPECTOR;
        // DBG(log(ERROR_log, "1 not found net range for ip %s",sa.getStringAddr().c_str()));
        outTreeRoute.push_back(0);
        outTreeRoute.push_back(0);
        outTreeRoute.push_back(0);
        outTreeRoute.push_back(0);
        outTreeRoute.push_back(0);
        calcTreeRouteCache[ip]=outTreeRoute;
        return outTreeRoute;
    }
    NetRec& r=*i;
    bool found=false;
    if(r.startIp<=ip && ip<=r.endIp)
    {
        found=true;
    }
    if(!found)
    {
        outTreeRoute.push_back(0);
        outTreeRoute.push_back(0);
        outTreeRoute.push_back(0);
        outTreeRoute.push_back(0);
        outTreeRoute.push_back(0);
        calcTreeRouteCache[ip]=outTreeRoute;
        return outTreeRoute;
    }

    int netMax=r.endIp-r.startIp;
    int netPos=ip-r.startIp;

    outBitStream o;
    int len=0;
    len+     =pack(o,r.countryPos,r.countryMax);
    len+=pack(o,r.locationPos,r.locationMax);
    len+=pack(o,r.netnumPos,r.netnumMax);
    len+=pack(o,netPos,netMax);
    o.put_bits(0,o.paddingSize());
    std::string obuf=o.asString();
    inBitStream in(obuf);

    for(int i=0; i<len; i+=6)
    {
        MUTEX_INSPECTOR;
        if(i+6<len)
        {
            outTreeRoute.push_back(in.get_bits(6));
        }
        else
        {
            outTreeRoute.push_back(in.get_bits(len-i));
        }
    }
    calcTreeRouteCache[ip]=outTreeRoute;
    return outTreeRoute;
}

int NetTreeRouteGenerator::pack(outBitStream& o,int val,int maxval)
{
    int r=0;
    while(maxval)
    {
        o.put_bits(val&1,1);
        maxval>>=1;
        val>>=1;
        r++;
    }
    return r;
}
void NetTreeRouteGenerator::load(const std::string& pn_netBlocks)
{
    struct stat st;
    if(!stat(pn_netBlocks.c_str(),&st))
    {
        st_FILE f(pn_netBlocks.c_str(),"rb");
        st_malloc buf(st.st_size+10);
        if(fread(buf.buf,1,st.st_size,f.f)!=st.st_size)
        {
            throw CommonError("!if(fread(buf,1,st.st_size,f)!=st.st_size)");
        }
        inBuffer in((char*)buf.buf,st.st_size);
        DBG(logErr2("load m_netArray"));
        in>>m_netArray;
        m_loaded=true;
        logErr2("Loaded netblocks");
    }

}
