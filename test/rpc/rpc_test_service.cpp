#include <string>
#include <unistd.h>
#include "IUtils.h"
#include "version_mega.h"
#include "listenerBuffered.h"
#include "broadcaster.h"
#include "main/CInstance.h"
#include "Events/System/Run/startServiceEvent.h"
#include "tools_mt.h"
#include "Events/System/Net/rpcEvent.h"
#include "Events/System/timerEvent.h"
#include "colorOutput.h"
#include "ISSL.h"
#include "main/configObj.h"
#include "rpcTestEvent.h"
static bool done_test=false;
#define BUF_SIZE_MAX (8*100*1)
#define N_PONG 12000
#define TI_ACTIVITY_VALUE 2.0
//#define USE_LOCAL 1

#define REMOTE_ADDR "localhost:2001"
static int session=0;
enum timers
{
    TI_START,
    TI_ACTIVITY
};
namespace testEvent {
    class testREQ: public Event::Base
    {
    public:
        static Base* construct(const route_t &r)
        {
            return new testREQ(r);
        }
        testREQ(int session_,int _seq,const std::string &_buf, const std::string& _md5, const route_t& r)
            :Base(testEventEnum::testREQ,r),
             session(session_), seq(_seq),buf(_buf),md5(_md5)
        {
        }
        testREQ(const route_t& r)
            :Base(testEventEnum::testREQ,r)
        {
        }
        int session;
        int seq;
        std::string buf;
        std::string md5;
        void unpack(inBuffer& o)
        {
            o>>session>>seq>>buf>>md5;
        }
        void pack(outBuffer&o) const
        {
            o<<session<<seq<<buf<<md5;
        }
        void jdump(Json::Value &) const
        {
        }

    };
    class testRSP: public Event::Base
    {
    public:
        static Base* construct(const route_t &r)
        {
            return new testRSP(r);
        }
        testRSP(int session_,int _seq,const std::string &_buf, const std::string& _md5, const route_t& r)
            :Base(testEventEnum::testRSP,r), session(session_),seq(_seq),buf(_buf),md5(_md5) {}
        testRSP(const route_t& r):Base(testEventEnum::testRSP,r) {}

        int session;
        int seq;
        std::string buf;
        std::string md5;

        void unpack(inBuffer& o)
        {
            o>>session>>seq>>buf>>md5;
        }
        void pack(outBuffer&o) const
        {
            o<<session<<seq<<buf<<md5;
        }
        void jdump(Json::Value &) const
        {
        }

    };
}

class rpcTestService1: public UnknownBase, public ListenerBuffered, public Broadcaster
{
public:
    I_ssl *ssl;
    void deinit()
    {
        ListenerBuffered::deinit();
    }

    static UnknownBase* construct(const SERVICE_id& id, const std::string&  nm,IInstance* ifa)
    {
        XTRY;
        return new rpcTestService1(id,nm,ifa);
        XPASS;
    }
    rpcTestService1(const SERVICE_id &id, const std::string&  nm,IInstance* ins):
        UnknownBase(nm),
        ListenerBuffered(nm,ins->getConfig(),id,ins),
        Broadcaster(ins)
    {
        ssl=(I_ssl*)iUtils->queryIface(Ifaces::SSL);

    }
    void sendRequest(int session,int seq)
    {
        if(done_test)return;
        size_t sz=rand()%BUF_SIZE_MAX;
        //char s[sz];
        std::string buf;
        while(buf.size()<sz)
        {
            buf+="klallosalkjf;laskdfj;alskdf;alksdfa;lkdfa;lksd;alksdf";//[i]=i;//rand();
        }
        //std::string buf(s,sz);
        std::string md5=ssl->md5(buf);

        if(seq%1000==0)
            logErr2("send %d:%d",session,seq+1);
#ifdef USE_LOCAL
        sendEvent(ServiceEnum::rpcTestService2,new testEvent::testREQ(session,seq+1,buf,md5,ListenerBase::serviceId));
#else
        msockaddr_in sa;
        sa.initFromUrl(REMOTE_ADDR);
//        caps.insert(sa);
        sendEvent(sa,
                  ServiceEnum::rpcTestService2,new testEvent::testREQ(session,seq+1,buf,md5,ListenerBase::serviceId));
#endif

    }
    bool handleEvent(const REF_getter<Event::Base>& e)
    {
        if(done_test)return true;
        MUTEX_INSPECTOR;
        //logErr2("@%s",__PRETTY_FUNCTION__);
        auto& ID=e->id;
        if(timerEventEnum::TickAlarm==ID)
        {
            timerEvent::TickAlarm* ee=(timerEvent::TickAlarm*)e.get();
            if(ee->tid==TI_START)
            {
                sendRequest(session,0);
                //sendRequest(session+1,0);
                //sendRequest(session+2,0);
                //session+=3;
                return true;
            }
            if(ee->tid==TI_ACTIVITY)
            {
                session++;
                sendRequest(session,0);
                return true;
            }
            return true;
        }
        if(systemEventEnum::startService==ID)
        {
            MUTEX_INSPECTOR;
            /// timeout to wait second peer to bind
            sendEvent(ServiceEnum::Timer,new timerEvent::SetAlarm(TI_START,NULL,NULL,0.3,ListenerBase::serviceId));
            sendEvent(ServiceEnum::Timer,new timerEvent::SetAlarm(TI_ACTIVITY,NULL,NULL,TI_ACTIVITY_VALUE,ListenerBase::serviceId));

            return true;
        }
        if(rpcEventEnum::IncomingOnConnector==ID)
        {
            MUTEX_INSPECTOR;
            rpcEvent::IncomingOnConnector *ioc=(rpcEvent::IncomingOnConnector *)e.get();
            auto &IDC=ioc->e->id;
            if(testEventEnum::testRSP==IDC)
                return on_testRSP((testEvent::testRSP*)ioc->e.get());
        }
        if(testEventEnum::testRSP==ID)
            return on_testRSP((testEvent::testRSP*)e.get());
        return false;
    }
    bool on_testRSP(const testEvent::testRSP* e)
    {
        if(done_test)return true;
        MUTEX_INSPECTOR;
        if(e->seq%1000==0)
            logErr2("on rsp %d",e->seq);
        sendEvent(ServiceEnum::Timer,new timerEvent::ResetAlarm(TI_ACTIVITY,NULL,NULL,TI_ACTIVITY_VALUE,ListenerBase::serviceId));

        //logErr2("@%s",__PRETTY_FUNCTION__);
        if(e->seq>N_PONG)
        {
            logErr2(GREEN("RPC test OK %d"),e->seq);
            done_test=true;
            return true;
        }
        if(ssl->md5(e->buf)!=e->md5)
        {
            throw CommonError("wrong md5 of buffer");
        }

        sendRequest(e->session,e->seq);

        return true;

    }
};
class rpcTestService2: public UnknownBase, public ListenerBuffered, public Broadcaster
{
public:
    I_ssl *ssl;
    void deinit()
    {
        ListenerBuffered::deinit();
    }

    static UnknownBase* construct(const SERVICE_id& id, const std::string&  nm,IInstance* ifa)
    {
        XTRY;
        return new rpcTestService2(id,nm,ifa);
        XPASS;
    }
    rpcTestService2(const SERVICE_id &id, const std::string&  nm,IInstance* ins):
        UnknownBase(nm),
        ListenerBuffered(nm,ins->getConfig(),id,ins),
        Broadcaster(ins)
    {
        ssl=(I_ssl*)iUtils->queryIface(Ifaces::SSL);
    }
    bool on_testREQ(const testEvent::testREQ* e)
    {
        if(done_test)return true;
        MUTEX_INSPECTOR;
        if(e->seq%1000==0)
            logErr2("recv %d",e->seq);
        if(ssl->md5(e->buf)!=e->md5)
        {
            throw CommonError("wrong md5 of buffer");
        }
        passEvent(new testEvent::testRSP(e->session,e->seq,e->buf,e->md5,poppedFrontRoute(e->route)));
        return true;

    }
    bool handleEvent(const REF_getter<Event::Base>& e)
    {
        if(done_test)return true;
        MUTEX_INSPECTOR;
        //logErr2("@%s",__PRETTY_FUNCTION__);
        auto& ID=e->id;
        if(systemEventEnum::startService==ID)
        {
            return true;
        }
        if(rpcEventEnum::IncomingOnAcceptor==ID)
        {
            MUTEX_INSPECTOR;
            rpcEvent::IncomingOnAcceptor *ioa=(rpcEvent::IncomingOnAcceptor *)e.get();
            auto &IDA=ioa->e->id;
            if(testEventEnum::testREQ==IDA)
                return on_testREQ((testEvent::testREQ*)ioa->e.get());

        }
        if(testEventEnum::testREQ==ID)
        {
            MUTEX_INSPECTOR;
            testEvent::testREQ* ee=(testEvent::testREQ*) e.get();
            if(ssl->md5(ee->buf)!=ee->md5)
            {
                throw CommonError("wrong md5 of buffer");
            }
            //usleep(50000);
            passEvent(new testEvent::testRSP(ee->session,ee->seq,ee->buf,ee->md5,poppedFrontRoute(e->route)));
            return true;
        }
        return false;
    }
};







void registerRPCTest(const char* pn)
{
    if(pn)
    {

        iUtils->registerPlugingInfo(COREVERSION,pn,IUtils::PLUGIN_TYPE_SERVICE,ServiceEnum::rpcTestService1,"rpcTestService1",std::set<EVENT_id>());
        iUtils->registerPlugingInfo(COREVERSION,pn,IUtils::PLUGIN_TYPE_SERVICE,ServiceEnum::rpcTestService2,"rpcTestService2",std::set<EVENT_id>());

    }
    else
    {
        iUtils->registerService(COREVERSION,ServiceEnum::rpcTestService1,rpcTestService1::construct,"rpcTestService1");
        iUtils->registerService(COREVERSION,ServiceEnum::rpcTestService2,rpcTestService2::construct,"rpcTestService2");
        iUtils->registerEvent(testEvent::testREQ::construct);
        iUtils->registerEvent(testEvent::testRSP::construct);

    }
}
