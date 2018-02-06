#ifndef ____DS_HANDLER__________h
#define ____DS_HANDLER__________h
#include "jsonHandler.h"
#include "configDB.h"

class UCHandler
    : public JsonHandler
{
public:
    UCHandler(
        IInstance *ifa
    );


    bool handleEventInDerived(const REF_getter<Event::Base> &e);





    virtual void signal_authorized()=0;








};
#endif
