#include <string>
#include "ITests.h"
#include "IUtils.h"
#include "version_mega.h"
#include "listenerBuffered.h"
#include "broadcaster.h"
#include "main/CInstance.h"
#include "Events/System/Run/startService.h"
#include "tools_mt.h"
//#include "Events/System/Net/rpc/IncomingOnAcceptor.h"
//#include "Events/System/Net/rpc/IncomingOnConnector.h"
//#include "Events/System/Net/rpc/SubscribeNotifications.h"
//#include "Events/System/timer/TickAlarm.h"
//bool done_test=false;
//#define BUF_SIZE_MAX (8*100*1)
//#define N_PONG 12000
//#define TI_ACTIVITY_VALUE 2.0
//#define USE_LOCAL 1

//#define REMOTE_ADDR "localhost:2001"
//int session=0;
namespace ServiceEnum {
    const SERVICE_id CTRTest("CTRTest");
}
namespace testEventEnum {
}





class CTRTest: public ITests::Base
{
public:
    int zzz;
    void run();
    CTRTest() {
        zzz=100;
    }
    ~CTRTest() {}
    static ITests::Base* construct()
    {
        XTRY;
        return new CTRTest;
        XPASS;
    }



};
template <class T> void fillrnd(I_ssl*ssl,T &z)
{
    ssl->rand_bytes((uint8_t*)&z,sizeof(z));

}
template <class T> void fillrnd(I_ssl* ssl, std::vector<T> &z)
{
    int rnd=rand()%10000;
    for(int i=0;i<rnd;i++)
    {
        int z1;
        fillrnd(ssl,z1);
        z.push_back(z1);
    }

}
template <class T> void fillrnd(I_ssl* ssl, std::deque<T> &z)
{
    int rnd=rand()%10000;
    for(int i=0;i<rnd;i++)
    {
        int z1;
        fillrnd(ssl,z1);
        z.push_back(z1);
    }

}
template <class T> void fillrnd(I_ssl* ssl, std::set<T> &z)
{
    int rnd=rand()%10000;
    for(int i=0;i<rnd;i++)
    {
        int z1;
        fillrnd(ssl,z1);
        z.insert(z1);
    }

}
template <class T1,class T2> void fillrnd(I_ssl* ssl, std::map<T1,T2> &z)
{
    int rnd=rand()%10000;
    for(int i=0;i<rnd;i++)
    {
        int z1;
        fillrnd(ssl,z1);
        int z2;
        fillrnd(ssl,z2);
        z.insert(std::make_pair(z1,z2));
    }

}


struct a
{
    std::vector<int>     n1;
    std::deque<int64_t> n2;
    std::set<int32_t> n3;
    std::vector<int8_t>  n4;
    std::deque<uint64_t> n5;
    std::map<uint32_t,uint64_t> n6;
    std::set<uint8_t>  n7;
    std::vector<long>    n8;
    std::deque<unsigned>    n9;
    std::map<unsigned  long,int> n10;
    std::deque<int64_t>    n11;
    std::set<uint16_t> n12;
    bool operator !=(const a& z)
    {
        if(n1!=z.n1){logErr2("fail %d",__LINE__); return true;}
        if(n2!=z.n2){logErr2("fail %d",__LINE__); return true;}
        if(n3!=z.n3){logErr2("fail %d",__LINE__); return true;}
        if(n4!=z.n4){logErr2("fail %d",__LINE__); return true;}
        if(n5!=z.n5){logErr2("fail %d",__LINE__); return true;}
        if(n6!=z.n6){logErr2("fail %d",__LINE__); return true;}
        if(n7!=z.n7){logErr2("fail %d",__LINE__); return true;}
        if(n8!=z.n8) {logErr2("fail %d",__LINE__); return true;}
        if(n9!=z.n9){logErr2("fail %d",__LINE__); return true;}
        if(n10!=z.n10){logErr2("fail %d",__LINE__); return true;}
        if(n11!=z.n11) return true;
        if(n12!=z.n12) return true;
        return false;
    }

    a() {
        I_ssl *ssl=(I_ssl*)iUtils->queryIface(Ifaces::SSL);
        fillrnd(ssl,n1);
        fillrnd(ssl,n2);
        fillrnd(ssl,n3);
        fillrnd(ssl,n4);
        fillrnd(ssl,n5);
        fillrnd(ssl,n6);
        fillrnd(ssl,n7);
        fillrnd(ssl,n8);
        fillrnd(ssl,n9);
        fillrnd(ssl,n10);
        fillrnd(ssl,n11);
        fillrnd(ssl,n12);
    }
};
inline outBuffer& operator<< (outBuffer& b,const a& s)
{
   b<<s.n1
    <<s.n2
    <<s.n3
    <<s.n4
    <<s.n5
    <<s.n6
    <<s.n7
    <<s.n8
    <<s.n9
    <<s.n10
    <<s.n11
    <<s.n12;
    return b;
}
inline inBuffer& operator>> (inBuffer& b,  a& s)
{
    b>>s.n1;
    b>>s.n2;
    b>>s.n3;
    b>>s.n4;
    b>>s.n5;
    b>>s.n6;
    b>>s.n7;
    b>>s.n8;
    b>>s.n9;
    b>>s.n10;
    b>>s.n11;
    b>>s.n12;
    return b;
}

struct Z
{
//    n _n;
    a _a;
    std::deque<std::string> strs;
    Z()
    {
        I_ssl *ssl=(I_ssl*)iUtils->queryIface(Ifaces::SSL);
        int rnd=rand()%10000;
        for(int i=0;i<rnd;i++)
        {
            int r2=rand()%10000;
            uint8_t s[r2];
            ssl->rand_bytes(s,sizeof(s));
            strs.push_back(std::string((char*)s,sizeof(s)));
        }

    }
    bool operator !=(const Z& z)
    {
        if(_a!=z._a){ logErr2("a failed");return true;}
//        if(_n!=z._n){ logErr2("n failed");return true;}
        if(strs!=z.strs){ logErr2("strs failed");return true;}
        return false;
    }
};
inline outBuffer& operator<< (outBuffer& b,const Z& s)
{
    b<<s._a;
//    b<<s._n;
    b<<s.strs;

    return b;
}
inline inBuffer& operator>> (inBuffer& b,  Z& s)
{
    b>>s._a;
//    b>>s._n;
    b>>s.strs;
    return b;
}
inline void testIoBuffer()
{
    try

    {
        uint64_t sn[]={
            0xFFFFFFFFFFFFFFFF,
            0xFFFFFFFFFFFFFFF,
            0xFFFFFFFFFFFFFF,
            0xFFFFFFFFFFFFF,
            0xFFFFFFFFFFFF,
            0xFFFFFFFFFFF,
            0xFFFFFFFFFF,
            0xFFFFFFFFF,
            0xFFFFFFFF,
            0xFFFFFFF,
            0xFFFFFF,
            0xFFFFF,
            0xFFFF,
            0xFFF,
            0xFF,
            0xF
            };
        for(int i=sizeof(sn)-1;i>=0;i--)
        {
            uint64_t n=sn[i];
//            printf("test %lx\n",sn[i]);
            outBuffer o;
            o<<n;
            std::string buf=o.asString()->asString();
//            logErr2("buf %s",iUtils->bin2hex(buf).c_str());
            inBuffer in(buf);
            int64_t nn;
            in>>nn;
            if(n!=nn)
            {
                printf("n!=nn %lx %lx\n",n,nn);
                exit(1);
            }
        }
        printf("!!!!!!!!!!!!! iobuffer numbers success\n");

        for(int i=0;i<3;i++)
        {
            Z z;
            outBuffer o;
            o<<z;
            std::string buf=o.asString()->asString();
            logErr2("buf %d",buf.size());
            inBuffer in(buf);
            Z zz;
            in >> zz;
            if(z!=zz)
            {
                fprintf(stderr,"iobuffer 1 failed\n");
                exit(1);
            }
        }
        printf("!!!! iobuffer 1 OK\n");

    }
    catch(std::exception& e)
    {
        printf("catched %s",e.what());
        exit(1);
    }

}
void testEncoding()
{

    I_ssl *ssl=(I_ssl*)iUtils->queryIface(Ifaces::SSL);
    int rn=rand()%10000;
    for(int i=0;i<rn;i++)
    {
        std::string str=ssl->rand_bytes(rand()%10000);
        if(str!=iUtils->Base64Decode(iUtils->Base64Encode(str)))
        {
            printf("base64 failed\n");
            exit(1);
        }
        if(str!=iUtils->hex2bin(iUtils->bin2hex(str)))
        {
            printf("bin2hex failed\n");
            exit(1);
        }
    }
    printf("!!!! bin2hex,base64 OK\n");
}
void CTRTest::run()
{

    logErr2("RUN TEST %s",__PRETTY_FUNCTION__);


    testIoBuffer();
    testEncoding();



}


void registerCTRTest(const char* pn)
{
    if(pn)
    {

        iUtils->registerPlugingInfo(COREVERSION,pn,IUtils::PLUGIN_TYPE_TEST,ServiceEnum::CTRTest,"CTRTest");

    }
    else
    {
        iUtils->registerITest(COREVERSION,ServiceEnum::CTRTest,CTRTest::construct);

    }
}
