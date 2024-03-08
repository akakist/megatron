#ifndef ________UdateUpdateConfig__hREQZ1
#define ________UdateUpdateConfig__hREQZ1

#include "___dfsReferrerEvent.h"

namespace dfsReferrerEvent {
    class UpdateConfigREQ: public Event::Base
    {


    public:
        static Base* construct(const route_t &r)
        {
            return new UpdateConfigREQ(r);
        }
        UpdateConfigREQ(const route_t& r)
            :Base(dfsReferrerEventEnum::UpdateConfigREQ,r) {}
        UpdateConfigREQ(const std::string &_body,  const route_t &r)
            :Base(dfsReferrerEventEnum::UpdateConfigREQ,r),
             bod(_body) {}
        std::string bod;
        void unpack(inBuffer& o)
        {

            o>>bod;
        }
        void pack(outBuffer&o) const
        {

            o<<bod;
        }
        void jdump(Json::Value &v) const
        {
            v["body"]=bod;
        }
    };

    class UpdateConfigRSP: public Event::Base
    {


    public:
        static Base* construct(const route_t &r)
        {
            return new UpdateConfigRSP(r);
        }
        UpdateConfigRSP(const route_t& r)
            :Base(dfsReferrerEventEnum::UpdateConfigRSP,r) {}
        UpdateConfigRSP(const std::string &_body,  const route_t &r)
            :Base(dfsReferrerEventEnum::UpdateConfigRSP,r),
             bod(_body) {}
        std::string bod;
        void unpack(inBuffer& o)
        {

            o>>bod;
        }
        void pack(outBuffer&o) const
        {

            o<<bod;
        }
        void jdump(Json::Value &v) const
        {
            v["body"]=bod;
        }
    };
}

#endif
