#ifndef _________dfsToplinkBroadcastByBackrouteEvent_h2
#define _________dfsToplinkBroadcastByBackrouteEvent_h2

#include "___dfsReferrerEvent.h"
namespace dfsReferrerEvent {
    class ToplinkBroadcastByBackroute: public Event::Base
    {
        enum {channel=CHANNEL_0};

    public:
        static Base* construct(const route_t &r)
        {

            return new ToplinkBroadcastByBackroute(r);
        }
        ToplinkBroadcastByBackroute(const SERVICE_id& _sourceService, const SERVICE_id& _destinationService, const REF_getter<Event::Base>& _e,const route_t& r)
            :Base(dfsReferrerEventEnum::ToplinkBroadcastByBackroute,channel,"ToplinkBroadcastByBackroute",r),
              sourceService(_sourceService),destinationService(_destinationService),
             eventData(NULL)
        {
            outBuffer o;
            iUtils->packEvent(o,_e);
            eventData=o.asString();


        }
        ToplinkBroadcastByBackroute(const route_t& r)
            :Base(dfsReferrerEventEnum::ToplinkBroadcastByBackroute,channel,"ToplinkBroadcastByBackroute",r),eventData(NULL)
        {
            if(!eventData.valid())
                eventData=new refbuffer;

        }
        REF_getter<refbuffer> eventData;
        SERVICE_id sourceService;
        SERVICE_id destinationService;
        void unpack(inBuffer& o)
        {

            o>>eventData>>sourceService>>destinationService;
        }
        void pack(outBuffer&o) const
        {

            o<<eventData<<sourceService<<destinationService;
        }
        void jdump(Json::Value &) const
        {

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
