#include "event_mt.h"
#include "IUtils.h"
Json::Value Event::Base::dump() const
{
    Json::Value v;
    v["evid"]=iUtils->genum_name(id);
    v["evroute"]=route.dump();
    jdump(v);
    return v;
}


void Event::NoPacked::pack(outBuffer& ) const
{

    throw CommonError("NoPacked::pack: invalid usage for class %s",iUtils->genum_name(id));
}
void Event::NoPacked::unpack(inBuffer& )
{
    throw CommonError("NoPacked::unpack: invalid usage for class %s",iUtils->genum_name(id));
}
