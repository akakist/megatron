#include "unknown.h"
#include "mutexInspector.h"
#include "colorOutput.h"
UnknownBase::~UnknownBase()
{
}

UnknownBase::UnknownBase(const std::string& nm)
    :classname(nm)
{
}
