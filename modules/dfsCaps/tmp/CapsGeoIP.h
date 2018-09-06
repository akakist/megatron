#ifndef CAPSGEOIP_H
#define CAPSGEOIP_H
#include <string>
#include <algorithm>
#include "REF.h"
#include "DBH.h"
#define IPDATA "ipdata.txt"

struct geoNetRec
{
    std::string startIp,endIp;
    double lat,lon;
};
inline outBuffer & operator<< (outBuffer& b,const geoNetRec &s)
{
    b<<s.startIp<<s.endIp<<s.lat<<s.lon;
    return b;
}
inline inBuffer & operator>> (inBuffer& b,  geoNetRec &s)
{
    b>>s.startIp>>s.endIp>>s.lat>>s.lon;
    return b;
}

inline int operator<(const geoNetRec& a,const geoNetRec& b)
{
    return a.endIp<b.endIp;
}

class CapsGeoIP
{
public:
    CapsGeoIP();
    void init(IInstance *instance1);


    std::vector<geoNetRec> geoNets;

    bool findNetRec(const std::string &ip, bool isV4, geoNetRec &result);
    std::pair<std::string,std::string> getIpRange(const std::string& ipmask, bool isV4);
    static std::string setBits(unsigned char* _ip, int iplen, int netmask, bool value);
    static std::string getIpBin(const std::string& ip,  bool isV4);
    static bool getFileContent(std::string fileName, std::vector<std::string> & vecOfStrs);
    static int cmpGeo(const geoNetRec&a,const geoNetRec &b);

};

#endif // CAPSGEOIP_H
