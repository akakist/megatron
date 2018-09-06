#ifndef CAPSALGORITHM_H_____________
#define CAPSALGORITHM_H_____________
#include <map>
#include "msockaddr_in.h"
#define MAX_REFFERRERS_COUNT_IN_RESPONSE 40
struct referrerItem: public Refcountable
{
    msockaddr_in sa;
    time_t lastReport;
    int dowlinkCount;
    double lat,lon;
    void pack(outBuffer& b)
    {
        b<<sa<<lastReport<<dowlinkCount<<lat<<lon;
    }
    void unpack(inBuffer& in)
    {
        in>>sa>>lastReport>>dowlinkCount>>lat>>lon;
    }
};

typedef std::pair<int,int> iLatLon;

struct referrerContainer
{
    void add(double lat,double lon, const msockaddr_in& sa, int downlinkCount)
    {
        auto it=referrers.find(sa);
        if(it==referrers.end())
        {
            REF_getter<referrerItem> i=new referrerItem;
            referrers.insert(std::make_pair(sa,i));
        }
        it=referrers.find(sa);
        it->second->dowlinkCount=downlinkCount;
        it->second->lastReport=time(NULL);
        it->second->lat=lat;
        it->second->lon=lon;
        it->second->sa=sa;

        iLatLon ll=std::make_pair(int(lat),int(lon));
        iLatLon ll2=std::make_pair(int(lat/2),int(lon/2));
        iLatLon ll4=std::make_pair(int(lat/4),int(lon/4));
        iLatLon ll8=std::make_pair(int(lat/8),int(lon/8));
        quadratsDegree[ll].insert(it->second);
        quadratsDegree_2[ll2].insert(it->second);
        quadratsDegree_4[ll4].insert(it->second);
        quadratsDegree_8[ll8].insert(it->second);

    }
    std::map<msockaddr_in,REF_getter<referrerItem> > referrers;

    std::map<iLatLon,std::set<REF_getter<referrerItem> > > quadratsDegree;
    std::map<iLatLon,std::set<REF_getter<referrerItem> > > quadratsDegree_2;
    std::map<iLatLon,std::set<REF_getter<referrerItem> > > quadratsDegree_4;
    std::map<iLatLon,std::set<REF_getter<referrerItem> > > quadratsDegree_8;

};

class CapsAlgorithm
{
public:

    std::map<time_t,referrerContainer> rcontainers;

    void addReferrer(double lat,double lon, const msockaddr_in& sa, int downlinkCount);

//    static double cmp_lat,cmp_lon;
    std::vector<REF_getter<referrerItem> > findReferrers(double lat,double lon);

    CapsAlgorithm();
};

#endif // CAPSALGORITHM_H
