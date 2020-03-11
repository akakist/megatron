#include "Integer.h"
#include "IUtils.h"
Integer operator -(const Integer& a, const Integer& b)
{
    Integer ret;
    ret.set(a.get()-b.get());
    return ret;
}
Integer operator +(const Integer& a, const Integer& b)
{
    Integer ret;
    ret.set(a.get()+b.get());
    return ret;
}
Integer& Integer::operator%=(const int64_t & a)
{
    value%=a;
    return *this;
}
Integer& Integer::operator*=(const int64_t & a)
{
    value*=a;
    return *this;
}
Integer& Integer::operator/=(const int64_t & a)
{
    value/=a;
    return *this;
}
Integer& Integer::operator+=(const int64_t & a)
{
    value+=a;
    return *this;
}
std::string Integer::dump() const
{
    return std::to_string(value);
}
int Integer::operator<(const Integer& a) const
{
    return value<a.value;
}
int Integer::operator<=(const Integer& a) const
{
    return value<=a.value;
}
int Integer::operator==(const Integer& a) const
{
    return value==a.value;
}
