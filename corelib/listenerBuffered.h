#ifndef ________________LISTENER_BUFFERED____H
#define ________________LISTENER_BUFFERED____H
#include "listenerBase.h"
#include "mutexable.h"
#include "IConfigObj.h"
#include "IInstance.h"
#include "eventDeque.h"
class ListenerBuffered;
/**
*       Базовый класс буферизованного листенера
*/
class ListenerBuffered:protected ListenerBase, protected Mutexable
{
    friend struct st_busy;

    std::map<pthread_t, REF_getter<EventDeque> > m_container;
    std::vector<std::pair<pthread_t, REF_getter<EventDeque> > > m_vector;
    size_t m_maxThreads;
    IInstance * instance;

    static void* worker(void*);
    void addDeque(pthread_t pt, const REF_getter<EventDeque>& d);
    REF_getter<EventDeque> getDeque();
    bool m_isTerminating_lb;


protected:
/// переопределенный метод
    void listenToEvent(const REF_getter<Event::Base>&e);
    void listenToEvent(const std::deque<REF_getter<Event::Base> >&);

public:
/// абстрактный метод. Можно его переопределить у потомка.
    virtual bool handleEvent(const REF_getter<Event::Base>& e)=0;
    virtual ~ListenerBuffered();
    ListenerBuffered(UnknownBase *i, const std::string& name, IConfigObj* conf, const SERVICE_id& sid,IInstance *ins);

    void processEvent(const REF_getter<Event::Base>&);
};
#endif
