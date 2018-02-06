#ifndef _________ToplinkDeliverREQ__h
#define _________ToplinkDeliverREQ__h
#include "___dfsReferrerEvent.h"
/**
Доставка события вверх по облаку. На некотором уровне оно форвардится в сервис, согласно общей конфигурации ботов.

Eсли destination service == "broadcast", то делаем бродкаст по облаку.
*/
namespace dfsReferrerEvent {
    class ToplinkDeliverREQ: public Event::Base
    {
        enum {channel=CHANNEL_0};

    public:
        static Base* construct(const route_t &r)
        {
            return new ToplinkDeliverREQ(r);
        }
        ToplinkDeliverREQ(const SERVICE_id& _destinationService, const REF_getter<Event::Base>& e, const route_t& r)
            :Base(dfsReferrerEventEnum::ToplinkDeliverREQ,channel,"ToplinkDeliverREQ",r),
             destinationService(_destinationService),eventData(NULL)
        {
            outBuffer o;
            iUtils->packEvent(o,e);
            eventData=o.asString();
        }
        ToplinkDeliverREQ(const route_t& r)
            :Base(dfsReferrerEventEnum::ToplinkDeliverREQ,channel,"ToplinkDeliverREQ",r),eventData(NULL)
        {
            if(!eventData.valid())
                eventData=new refbuffer;

        }
        SERVICE_id destinationService;
        REF_getter<refbuffer> eventData;

        std::set<GlobalCookie_id> uuid;

        REF_getter<Event::Base> getEvent() const
        {
            inBuffer in(eventData);
            REF_getter<Event::Base> z=iUtils->unpackEvent(in);
            return z;
        }
        void unpack(inBuffer& o)
        {
            o>>destinationService>>eventData>>uuid;
        }
        void pack(outBuffer&o) const
        {
            o<<destinationService<<eventData<<uuid;
        }
        void jdump(Json::Value &v) const
        {

            v["destinationService"]=destinationService.dump();
            v["event"]=getEvent()->dump().c_str();
        }

    };

}
#endif
