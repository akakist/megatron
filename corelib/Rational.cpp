#include "Rational.h"
#include <locale.h>
#include <math.h>

void Rational::fromString(const std::string &s)
{
#if !defined __MOBILE__

    lconv *lc=localeconv();
#endif
    char *ss=(char *)s.data();
    int sz=s.size();
    for(int i=0; i<sz; i++)
    {
        if(ss[i]==','||ss[i]=='.')
#if !defined __MOBILE__

            ss[i]=lc->decimal_point[0];
#else
            ss[i]='.';
#endif
    }
    v=strtod(s.c_str(),NULL);
}
std::string Rational::toString() const
{
    char s[200];
#if __WORDSIZE == 64
    snprintf(s,sizeof(s),"%.16lg",v);
#else
    snprintf(s,sizeof(s),"%.16g",v);
#endif
    return s;
}
std::string Rational::toString(const char* fmt) const
{
    char s[200];
    snprintf(s,sizeof(s),fmt,v);
    return s;
}
outBuffer & operator<< (outBuffer& b,const Rational &d)
{
#ifdef WITH_RATIO
    int exp;
    double z=frexp(d.v,&exp);
    std::pair<int64_t,int64_t> p=rationalize(z,100000000);
    b<<p.first<<p.second<<exp;
    double tt=double(p.first)/double(p.second)* pow(2,exp);
    if(fabs(tt-d.v)>0.0000001)
        throw CommonError("if(fabs(tt-d.v)>0.0000001) %g",fabs(tt-d.v));
#else
    b<<d.toString();
#endif
    return b;
}
inBuffer & operator>> (inBuffer& b,  Rational &d)
{

#ifdef WITH_RATIO
    int64_t num,den;
    int exp;
    b>>num>>den>>exp;
    d.v=double(num)/double(den)* pow(2,exp);
#else
    std::string s;
    b>>s;
    d.fromString(s);
#endif
    return b;
}


