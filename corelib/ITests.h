#ifndef _________________ITests___h
#define _________________ITests___h
#include "trashable.h"

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
#endif
