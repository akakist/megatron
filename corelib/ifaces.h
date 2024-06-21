#pragma once
#include "SERVICE_id.h"
namespace Ifaces
{
/// this class is used to create simple interfaces, shared between services

    const SERVICE_id AV("AV");
    const SERVICE_id SSL("SSL");
    const SERVICE_id GEOIP("GEOIP");
    class Base
    {
    public:
        virtual ~Base() {}
    };
};
