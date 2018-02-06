#include "unknown.h"
#include "mutexInspector.h"
void UnknownBase::addClass(const CLASS_id& c, void* instance)
{
    M_LOCK(__m_lock$);
    __container$[c]=instance;
}
void* UnknownBase::cast(const CLASS_id& c) const
{
    M_LOCK(__m_lock$);
    std::map<CLASS_id,void*>::const_iterator i=__container$.find(c);
    if(i==__container$.end())
    {
        throw CommonError("cast  failed %s",_DMI().c_str());
    }
    return i->second;
}
UnknownBase::~UnknownBase()
{
}

UnknownBase::UnknownBase(const std::string& nm)
    :classname(nm)
{
}
