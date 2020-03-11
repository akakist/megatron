#include "event.h"
void Event::NoPacked::pack(outBuffer& ) const
{
    throw CommonError("NoPacked::pack: invalid usage for class %s",id.dump().c_str());
}
void Event::NoPacked::unpack(inBuffer& )
{
    throw CommonError("NoPacked::unpack: invalid usage for class %s",id.dump().c_str());
}
Json::Value Event::Base::dump() const
{
    Json::Value v;
    v["evid"]=id.dump();
    v["evroute"]=route.dump();
    jdump(v);
    return v;
}
