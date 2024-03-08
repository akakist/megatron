#include "listenerSimple.h"
#include "mutexInspector.h"
ListenerSimple::~ListenerSimple()
{
}


ListenerSimple::ListenerSimple(const std::string& name,IConfigObj*, const SERVICE_id& sid):ListenerBase(name,sid) {}


void ListenerSimple::listenToEvent(const REF_getter<Event::Base>& e)
{
    XTRY;
    try {
        if(!e.valid()) return;
//        for(auto& i:handlers)
//        {
//            if(i.first(e.operator ->(),i.second))
//                return;
//        }
        if(!handleEvent(e.operator ->()))
        {
            logErr2("ListenerSimple: unhandled event %s  in %s",e->dump().toStyledString().c_str(),listenerName.c_str());
        }
    }
    catch(CommonError& ec)
    {
        logErr2("CommonError: %s %s",ec.what(),_DMI().c_str());

    }
    catch(std::exception &ec)
    {
        logErr2("ListenerSimple std::exception: %s %s %d",ec.what(),__FILE__,__LINE__);
    }
    XPASS;
}

