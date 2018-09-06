#ifndef _________________ifaces___h
#define _________________ifaces___h
namespace Ifaces
{
/// this class is used to create simple interfaces, shared between services

enum
    {
        AV=1,
        SSL=2,
        GEOIP=3,
    };
    class Base
    {
    public:
        virtual ~Base() {}
    };
};
#endif
