#ifndef TEST_RPC_EVENT__H
#define TEST_RPC_EVENT__H

#include "SERVICE_id.h"
#include "json/json.h"
#include "event_mt.h"

namespace ServiceEnum {
    const SERVICE_id rpcTest("rpcTest");
    const SERVICE_id rpcTestService1("rpcTestService1");
    const SERVICE_id rpcTestService2("rpcTestService2");
}
namespace testEventEnum {
    const EVENT_id testREQ("testREQ");
    const EVENT_id testRSP("testRSP");
}
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
#endif
