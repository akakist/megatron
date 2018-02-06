#ifndef ________________LISTENER_BUFFERED____H1t
#define ________________LISTENER_BUFFERED____H1t
#include "listenerBase.h"
#include "mutexable.h"
#include "IConfigObj.h"
#include "IInstance.h"
#include "eventDeque.h"
class ListenerBuffered1Thread;
/**
*       Базовый класс буферизованного листенера
*/
class ListenerBuffered1Thread:public ListenerBase
{
    EventDeque m_container;
    pthread_t m_pt;
    IInstance *instance;
    bool m_isTerminating;
    static void* worker(void*);
protected:
/// переопределенный метод
    void listenToEvent(const REF_getter<Event::Base>&e);
    void listenToEvent(const std::deque<REF_getter<Event::Base> >&);
public:
/// полуабстрактный метод. Можно его переопределить у потомка.
    virtual bool handleEvent(const REF_getter<Event::Base>& e)=0;
    virtual ~ListenerBuffered1Thread();
    ListenerBuffered1Thread(UnknownBase *i, const std::string& name,IConfigObj*, const SERVICE_id & sid,IInstance* ins);

    void processEvent(const REF_getter<Event::Base>&);



};
#endif
