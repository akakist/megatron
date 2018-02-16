#ifndef ________________LISTENER_BUFFERED____H1t
#define ________________LISTENER_BUFFERED____H1t
#include "listenerBase.h"
#include "mutexable.h"
#include "IConfigObj.h"
#include "IInstance.h"
#include "eventDeque.h"
class ListenerBuffered1Thread;
/**
*       Single thread listener, used if you need to create service without internal multithreading, i.e. without mutexes.
*       Each ListenerBuffered1Thread has own thread
*/
class ListenerBuffered1Thread:public ListenerBase
{

    EventDeque m_container;
    pthread_t m_pt;
    IInstance *instance;
    bool m_isTerminating;
    static void* worker(void*);
protected:

    /// implementation of base virtual
    void listenToEvent(const REF_getter<Event::Base>&e);
    void listenToEvent(const std::deque<REF_getter<Event::Base> >&);
public:

    /// call method in inherited class to process event
    virtual bool handleEvent(const REF_getter<Event::Base>& e)=0;

    virtual ~ListenerBuffered1Thread();
    ListenerBuffered1Thread(UnknownBase *i, const std::string& name,IConfigObj*, const SERVICE_id & sid,IInstance* ins);

    /// caller of handleEvent
    void processEvent(const REF_getter<Event::Base>&);



};
#endif
