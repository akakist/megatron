#pragma once
#include "trashable.h"
#include <string>

namespace ITests
{
    class Base
    {
    public:
        virtual int run()=0;
        virtual std::string getName()=0;
        virtual ~Base() {}
    };
}
