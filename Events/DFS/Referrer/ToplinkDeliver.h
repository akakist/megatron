#ifndef _________ToplinkDeliverREQ__hZ1
#define _________ToplinkDeliverREQ__hZ1
#include "___dfsReferrerEvent.h"
#include "mutexInspector.h"
/**
Доставка события вверх по облаку. На некотором уровне оно форвардится в сервис, согласно общей конфигурации ботов.

Eсли destination service == "broadcast", то делаем бродкаст по облаку.
*/
namespace dfsReferrerEvent {
    class ToplinkDeliverREQ: public Event::Base
    {
        enum {rpcChannel=CHANNEL_0};

    public:
        static Base* construct(const route_t &r)
        {
            return new ToplinkDeliverREQ(r);
        }
        ToplinkDeliverREQ(const SERVICE_id& _destinationService, const REF_getter<Event::Base>& e, const route_t& r)
            :Base(dfsReferrerEventEnum::ToplinkDeliverREQ,rpcChannel,r),
             destinationService(_destinationService),eventData(NULL)
        {
            outBuffer o;
            iUtils->packEvent(o,e);
            eventData=o.asString();
        }
        ToplinkDeliverREQ(const route_t& r)
            :Base(dfsReferrerEventEnum::ToplinkDeliverREQ,rpcChannel,r),eventData(NULL)
        {
            if(!eventData.valid())
                eventData=new refbuffer;

        }
        SERVICE_id destinationService;
        REF_getter<refbuffer> eventData;

        std::set<GlobalCookie_id> uuid;

        REF_getter<Event::Base> getEvent() const
        {
            MUTEX_INSPECTOR;
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
        }

    };
    class ToplinkDeliverRSP: public Event::Base
    {
        enum {rpcChannel=CHANNEL_0};

    public:
        static Base* construct(const route_t &r)
        {

            return new ToplinkDeliverRSP(r);
        }
        ToplinkDeliverRSP(const REF_getter<Event::Base>& _e,const route_t& r)
            :Base(dfsReferrerEventEnum::ToplinkDeliverRSP,rpcChannel,r),
             eventData(NULL)
        {
            outBuffer o;
            iUtils->packEvent(o,_e);
            eventData=o.asString();

        }
        ToplinkDeliverRSP(const route_t& r)
            :Base(dfsReferrerEventEnum::ToplinkDeliverRSP,rpcChannel,r),eventData(NULL)
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
            MUTEX_INSPECTOR;
            inBuffer in(eventData);
            REF_getter<Event::Base> z=iUtils->unpackEvent(in);
            return z;
        }

    };


}
#endif
