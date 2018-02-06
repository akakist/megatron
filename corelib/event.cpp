#include "event.h"
void Event::NoPacked::pack(outBuffer& ) const
{
    throw CommonError("NoPacked::pack: invalid usage for class %s",id.dump().c_str());
}
void Event::NoPacked::unpack(inBuffer& )
{
    throw CommonError("NoPacked::unpack: invalid usage for class %s",id.dump().c_str());
}
std::string Event::Base::dump() const
{
    Json::Value v;
    v["evname"]=name;
    v["evroute"]=route.dump();
    jdump(v);
    Json::FastWriter w;
    return w.write(v);
}
