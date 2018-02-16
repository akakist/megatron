#ifndef ______________________EVENT______H
#define ______________________EVENT______H

#include "ioBuffer.h"
#include "REF.h"
#include "EVENT_id.h"
#include "commonError.h"
#include "route_t.h"
#include <ctype.h>
#include <json/json.h>

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
        const int channel;
        const char* name;
        virtual ~Base() {}
        virtual void jdump(Json::Value &v) const=0;
        /// маршрут
        route_t route;
        Base(const EVENT_id&_id, const int _channel, const char* _name): id(_id),channel(_channel),name(_name) {}
        Base(const EVENT_id& _id, const int _channel, const char* _name,const route_t &_route):id(_id),channel(_channel),name(_name), route(_route) {}

        /// pack/unpack to outBuffer/inBuffer, used for RPC
        virtual void pack(outBuffer& b)const =0;
        virtual void unpack(inBuffer& b)=0;

        std::string dump() const;
    };


    /// base class for local events (without RPC)
    class NoPacked:public Base
    {
    public:
        NoPacked(const EVENT_id&_id, const char* _name): Base(_id,0,_name) {}
        NoPacked(const EVENT_id& _id, const char* _name,const route_t &_route):Base(_id,0,_name,_route) {}
        void pack(outBuffer& ) const;
        void unpack(inBuffer& );

    };

}



/// Static constructor, needed only for serializebla events
typedef Event::Base* (*event_static_constructor) (const route_t& r);



#endif
