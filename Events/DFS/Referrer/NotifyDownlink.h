#ifndef _________dfsNotifyDownlinkEvent_h2
#define _________dfsNotifyDownlinkEvent_h2

#include "___dfsReferrerEvent.h"
namespace dfsReferrerEvent {
    class NotifyDownlink: public Event::Base
    {
        enum {channel=CHANNEL_0};

    public:
        static Base* construct(const route_t &r)
        {

            return new NotifyDownlink(r);
        }
        NotifyDownlink(const SERVICE_id& _destinationService, const REF_getter<Event::Base>& _e,const route_t& r)
            :Base(dfsReferrerEventEnum::NotifyDownlink,channel,"NotifyDownlink",r),
              destinationService(_destinationService),
             eventData(NULL)
        {
            outBuffer o;
            iUtils->packEvent(o,_e);
            eventData=o.asString();


        }
        NotifyDownlink(const route_t& r)
            :Base(dfsReferrerEventEnum::NotifyDownlink,channel,"NotifyDownlink",r),eventData(NULL)
        {
            if(!eventData.valid())
                eventData=new refbuffer;

        }
        REF_getter<refbuffer> eventData;
        SERVICE_id destinationService;
        void unpack(inBuffer& o)
        {

            o>>eventData>>destinationService;
        }
        void pack(outBuffer&o) const
        {

            o<<eventData<<destinationService;
        }
        void jdump(Json::Value & j) const
        {
            inBuffer in(eventData);
            REF_getter<Event::Base> z=iUtils->unpackEvent(in);
            j["event"]=z->dump().c_str();
        }
        REF_getter<Event::Base> getEvent() const
        {
            inBuffer in(eventData);
            REF_getter<Event::Base> z=iUtils->unpackEvent(in);
            return z;
        }

    };

}
#endif
