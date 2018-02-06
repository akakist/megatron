#ifndef _________dfsToplinkDeliverRSPEvent_h2
#define _________dfsToplinkDeliverRSPEvent_h2
#include "___dfsReferrerEvent.h"
namespace dfsReferrerEvent {
    class ToplinkDeliverRSP: public Event::Base
    {
        enum {channel=CHANNEL_0};

    public:
        static Base* construct(const route_t &r)
        {

            return new ToplinkDeliverRSP(r);
        }
        ToplinkDeliverRSP(const REF_getter<Event::Base>& _e,const route_t& r)
            :Base(dfsReferrerEventEnum::ToplinkDeliverRSP,channel,"ToplinkDeliverRSP",r),
             eventData(NULL)
        {
            outBuffer o;
            iUtils->packEvent(o,_e);
            eventData=o.asString();

        }
        ToplinkDeliverRSP(const route_t& r)
            :Base(dfsReferrerEventEnum::ToplinkDeliverRSP,channel,"ToplinkDeliverRSP",r),eventData(NULL)
        {
            if(!eventData.valid())
                eventData=new refbuffer;

        }
        REF_getter<refbuffer> eventData;
        void unpack(inBuffer& o)
        {

            o>>eventData;
        }
        void pack(outBuffer&o) const
        {

            o<<eventData;
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
