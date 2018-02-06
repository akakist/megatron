#ifndef ______LISTENER____H
#define ______LISTENER____H
#include "unknownCastDef.h"
#include "unknown.h"
#include "event.h"

typedef bool (*eventhandler) (const REF_getter<Event::Base> &, void*);

/**
* Базовый класс, который реализует свойства слушателя евентов
*/

class ListenerBase
{
protected:
public:
    /// список хендлеров для работы ifacegen
    std::vector<std::pair<eventhandler,void*> > handlers;
    /// переопределяемый метод
    virtual void listenToEvent(const REF_getter<Event::Base>&)=0;
    virtual void listenToEvent(const std::deque<REF_getter<Event::Base> >&)=0;
    /// имя класса
    const std::string listenerName;
    /// кастинг листенера, используется механизм Unknown
    static ListenerBase* cast(UnknownBase *c);
    ListenerBase(UnknownBase* i, const std::string& nm, const SERVICE_id& sid);
    virtual ~ListenerBase() {}
    const SERVICE_id serviceId;
};

#endif

