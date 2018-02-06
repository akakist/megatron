#ifndef ________________LISTENER_Polled____H
#define ________________LISTENER_Polled____H
#include "listenerBase.h"
#include "mutexable.h"
#include "IConfigObj.h"
#include "IInstance.h"
class ListenerPolled;
/**
*       Базовый класс буферизованного листенера
*/
class ListenerPolled:public ListenerBase, public Mutexable
{
    std::deque<REF_getter<Event::Base> > m_container_;
protected:
/// переопределенный метод
    void listenToEvent(const REF_getter<Event::Base>&e);
    void listenToEvent(const std::deque<REF_getter<Event::Base> >&);

public:
/// полуабстрактный метод. Можно его переопределить у потомка.
    virtual bool handleEvent(const REF_getter<Event::Base>& e)=0;
    virtual ~ListenerPolled();
    ListenerPolled(UnknownBase *i, const std::string& name,IConfigObj*, const SERVICE_id& sid);
    void poll();

    bool init(IConfigObj* obj);
    bool deinit();

};
#endif
