#ifndef ________UdateUpdateConfig__hRSP
#define ________UdateUpdateConfig__hRSP
#include "___dfsReferrerEvent.h"

namespace dfsReferrerEvent {
    class UpdateConfigRSP: public Event::Base
    {
        enum {channel=CHANNEL_100};

    public:
        static Base* construct(const route_t &r)
        {
            return new UpdateConfigRSP(r);
        }
        UpdateConfigRSP(const route_t& r)
            :Base(dfsReferrerEventEnum::UpdateConfigRSP,channel,"UpdateConfigRSP",r) {}
        UpdateConfigRSP(const std::string &_body,  const route_t &r)
            :Base(dfsReferrerEventEnum::UpdateConfigRSP,channel,"UpdateConfigRSP",r),
             body(_body) {}
        std::string body;
        void unpack(inBuffer& o)
        {

            o>>body;
        }
        void pack(outBuffer&o) const
        {

            o<<body;
        }
        void jdump(Json::Value &v) const
        {
            v["body"]=body;
        }
    };


}

#endif
