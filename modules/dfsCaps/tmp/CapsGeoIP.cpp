#include "CapsGeoIP.h"
#include <iostream>
#include <fstream>
#include <ios>



inline bool exists_file (const std::string& name) {
    return ( access( name.c_str(), F_OK ) != -1 );
}
std::pair<std::string,std::string> CapsGeoIP::getIpRange(const std::string& ipmask, bool isV4)
{
    std::vector<std::string> v=iUtils->splitString("/",ipmask);
    std::string ip=v[0];
    if(v.size()!=2)
    {
        logErr2("invalid format");
    }
    int mask=atoi(v[1].c_str());
    if(isV4)
    {
        std::vector<std::string> vi=iUtils->splitString(".",ip);
        unsigned char ipBin[4];
        for(int i=0;i<4;i++)
        {
            ipBin[i]=atoi(vi[i].c_str());
        }
        std::string begin_addr=setBits(ipBin,4,mask,false);
        std::string end_addr=setBits(ipBin,4,mask,true);
        return std::make_pair(begin_addr,end_addr);
    }
    else
    {
        in6_addr addr;
        int r=inet_pton(AF_INET6, ip.c_str(), &addr);
        if(r<0)
            throw CommonError("Not in presentation format %s",ip.c_str());
        unsigned char ipBin[16];
        for(int i=0;i<16;i++)
        {
            ipBin[i]=addr.s6_addr[i];
        }
        std::string begin_addr=setBits(ipBin,16,mask,false);
        std::string end_addr=setBits(ipBin,16,mask,true);
        return std::make_pair(begin_addr,end_addr);
    }
}

void CapsGeoIP::init(IInstance* instance1)
{
//    REF_getter<DBH> dbh=getDB();

    int i=0;
    if(!exists_file(IPDATA))
    {
        FILE *f=fopen(IPDATA,"w");
        if(!f) throw CommonError("if(!f)");
        {
            DBH_feature df(instance1);
            auto dbh=df.getDB();
            st_TRANSACTION tr(dbh);

            REF_getter<QueryResult> r=dbh->exec((QUERY)"select network, latitude,longitude from caps.city_block6");
            for(auto row:r->values)
            {
                i++;
                if(row.size()==3)
                {
                    std::string nw=row[0];
                    std::string lat=row[1];
                    std::string lon=row[2];
                    auto res=getIpRange(nw,false);
                    fprintf(f,"%s %s %s %s\n",iUtils->bin2hex(res.first).c_str(),iUtils->bin2hex(res.second).c_str(),lat.c_str(),lon.c_str());
                }
                if(i%1000==0)
                    logErr2("processed %d",i);


            }
            tr.commit();
        }
        {
            DBH_feature df(instance1);
            auto dbh=df.getDB();
            st_TRANSACTION tr(dbh);

            REF_getter<QueryResult> r=dbh->exec((QUERY)"select network, latitude,longitude from caps.city_block4");
            for(auto row:r->values)
            {
                i++;
                if(row.size()==3)
                {
                    std::string nw=row[0];
                    std::string lat=row[1];
                    std::string lon=row[2];
                    auto res=getIpRange(nw,true);
                    fprintf(f,"%s %s %s %s\n",iUtils->bin2hex(res.first).c_str(),iUtils->bin2hex(res.second).c_str(),lat.c_str(),lon.c_str());
                }
                if(i%1000==0)
                    logErr2("processed %d",i);


            }
            tr.commit();
        }
        logErr2("prepare ipdata.txt complete");
        fclose(f);
    }
//    else
    {
        std::vector<std::string> vlines;
        getFileContent(IPDATA,vlines);
        logErr2("read v %d",vlines.size());
        for(auto &z:vlines)
        {
            std::vector<std::string> v=iUtils->splitString(" ",z);
            if(v.size()==4)
            {
                geoNetRec n;
                n.startIp=iUtils->hex2bin(v[0]);
                n.endIp=iUtils->hex2bin(v[1]);
                n.lat=strtod(v[2].c_str(),NULL);
                n.lon=strtod(v[3].c_str(),NULL);
                geoNets.push_back(n);
            }
        }
        sort(geoNets.begin(),geoNets.end(),cmpGeo);
    }

    logErr2("CapsGeoIP::init done");
//    outBuffer o;
//    o<<geoNets;
//    std::string str=o.asString()->asString();
//    FILE *out=fopen("ipdata.bin","wb");
//    if(out==NULL)
//        throw CommonError("if(out==NULL)");

//    fwrite(str.data(),1,str.size(),out);
//    fclose(out);
//    logErr2("ipdata.bin done");
}
int CapsGeoIP::cmpGeo(const geoNetRec&a,const geoNetRec &b)
{
    return a.endIp<b.endIp;
}

CapsGeoIP::CapsGeoIP()
{
}
bool CapsGeoIP::findNetRec(const std::string &ip, bool isV4,geoNetRec& result)
{
   std::string ipBin=getIpBin(ip,isV4);
   geoNetRec sample;
   sample.endIp=ipBin;
   auto it=std::lower_bound(geoNets.begin(),geoNets.end(),sample);
   if(it!=geoNets.end())
   {
       result=*it;
       return true;
   }
   return false;
}

std::string CapsGeoIP::setBits(unsigned char* _ip, int iplen, int netmask, bool value)
{
    unsigned char ip[iplen];
    memcpy(ip,_ip,iplen);
    for(int i=netmask;i<iplen*8;i++)
    {
        int nbyte=i/8;
        int nbit=i%8;
        if(value)
        {
            ip[nbyte]|=1<<(7-nbit);
        }
        else
        {
            ip[nbyte]&=~(1<<(7-nbit));
        }
    }
    return std::string((char*)ip,iplen);
}
std::string CapsGeoIP::getIpBin(const std::string& ip,  bool isV4)
{
    if(isV4)
    {
        std::vector<std::string> vi=iUtils->splitString(".",ip);
        unsigned char ipBin[4];
        for(int i=0;i<4;i++)
        {
            ipBin[i]=atoi(vi[i].c_str());
        }
        return std::string((char*)ipBin,4);
    }
    else
    {
        in6_addr addr;
        int r=inet_pton(AF_INET6, ip.c_str(), &addr);
        if(r<0)
            throw CommonError("Not in presentation format %s",ip.c_str());
        unsigned char ipBin[16];
        for(int i=0;i<16;i++)
        {
            ipBin[i]=addr.s6_addr[i];
        }
        return std::string((char*)ipBin,16);
    }

}
bool CapsGeoIP::getFileContent(std::string fileName, std::vector<std::string> & vecOfStrs)
{

    // Open the File
    std::ifstream inF(fileName.c_str(), std::ios_base::in);

    // Check if object is valid
    if(!inF)
    {
        std::cerr << "Cannot open the File : "<<fileName<<std::endl;
        return false;
    }

    std::string str;
    // Read the next line from File untill it reaches the end.
    while (std::getline(inF, str))
    {
        // Line contains string of length > 0 then save it in vector
        if(str.size() > 0)
            vecOfStrs.push_back(str);
    }
    //Close The File
    inF.close();
    return true;
}
