#ifndef ________UdateUpdateConfig__hREQ
#define ________UdateUpdateConfig__hREQ

#include "___dfsReferrerEvent.h"

namespace dfsReferrerEvent {
    class UpdateConfigREQ: public Event::Base
    {
        enum {channel=CHANNEL_100};

    public:
        static Base* construct(const route_t &r)
        {
            return new UpdateConfigREQ(r);
        }
        UpdateConfigREQ(const route_t& r)
            :Base(dfsReferrerEventEnum::UpdateConfigREQ,channel,"UpdateConfigREQ",r) {}
        UpdateConfigREQ(const std::string &_body,  const route_t &r)
            :Base(dfsReferrerEventEnum::UpdateConfigREQ,channel,"UpdateConfigREQ",r),
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
