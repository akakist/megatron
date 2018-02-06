#ifndef ______Rational__H
#define ______Rational__H
#include <string>
#include <sys/types.h>
#include <stdint.h>
#include "ioBuffer.h"
#include "Real.h"
//#if !defined __MOBILE__

class Rational
{
public:
    real v;

public:
    Rational():v(0) {}
    Rational(const real &d):v(d) {}
    void fromString(const std::string &s);
    std::string toString() const;
    std::string toString(const char* fmt) const;
};
outBuffer & operator<< (outBuffer& b,const Rational &d);
inBuffer & operator>> (inBuffer& b,  Rational &d);
inline bool operator < (const Rational& a,const Rational& b)
{
    return a.v<b.v;
}

//#endif
#endif
