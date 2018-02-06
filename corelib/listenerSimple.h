#ifndef ____________LISTENER_____H
#define ____________LISTENER_____H
#include "listenerBase.h"

/**
*       Простой листенер без буферизации и внутренних потоков
*/
class ListenerSimple:public ListenerBase
{
public:
    ListenerSimple(UnknownBase *i, const std::string& name,IConfigObj*, const SERVICE_id& sid);

/// полуабстрактный метод. Можно его переопределить у потомка.
    virtual bool handleEvent(const REF_getter<Event::Base>& e)=0;
    virtual ~ListenerSimple();

/// переопределенный метод
    void listenToEvent(const REF_getter<Event::Base>& e);
    void listenToEvent(const std::deque<REF_getter<Event::Base> >&);

};
#endif

