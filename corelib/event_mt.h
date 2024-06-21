#pragma once

#include <json/json.h>
#include "REF.h"
#include "EVENT_id.h"
#include "route_t.h"

enum
{
    CHANNEL_LOCAL,
    CHANNEL_0,
    CHANNEL_10,
    CHANNEL_70,
    CHANNEL_100
};

/// base class for events
namespace Event
{
    class Base: public Refcountable
    {
    public:
        /// тип евента
        const EVENT_id id;
        virtual ~Base() {}
        virtual void jdump(Json::Value &v) const=0;
        /// маршрут
        route_t route;
        Base(const EVENT_id&_id): id(_id) {}
        Base(const EVENT_id& _id,const route_t &_route):id(_id),route(_route) {}

        /// pack/unpack to outBuffer/inBuffer, used for RPC
        virtual void pack(outBuffer& b)const =0;
        virtual void unpack(inBuffer& b)=0;

        Json::Value dump() const;
    };


    /// base class for local events (without RPC)
    class NoPacked:public Base
    {
    public:
        NoPacked(const EVENT_id&_id): Base(_id) {}
        NoPacked(const EVENT_id& _id, const route_t &_route):Base(_id,_route) {}
        void pack(outBuffer& ) const;
        void unpack(inBuffer& );

    };

}



/// Static constructor, needed only for serializebla events
typedef Event::Base* (*event_static_constructor) (const route_t& r);

inline void Event::NoPacked::pack(outBuffer& ) const
{
    throw CommonError("NoPacked::pack: invalid usage for class %s",id.dump().c_str());
}
inline void Event::NoPacked::unpack(inBuffer& )
{
    throw CommonError("NoPacked::unpack: invalid usage for class %s",id.dump().c_str());
}
inline Json::Value Event::Base::dump() const
{
    Json::Value v;
    v["evid"]=id.dump();
    v["evroute"]=route.dump();
    jdump(v);
    return v;
}


