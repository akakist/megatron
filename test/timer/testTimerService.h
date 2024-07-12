#ifndef _______________displayzzPLAYBCNODE___H1INV
#define _______________displayzzPLAYBCNODE___H1INV
#include "broadcaster.h"

#include "listenerBuffered1Thread.h"
#include "listenerSimple.h"
#include <map>
#include "Events/System/Run/startServiceEvent.h"
#include "Events/System/Net/httpEvent.h"
#include "listenerSimple.h"



namespace testTimer
{
    class Service:
        public UnknownBase,
        public ListenerBuffered1Thread,
        public Broadcaster
    {
        bool on_startService(const systemEvent::startService*);
        bool handleEvent(const REF_getter<Event::Base>& e);



    public:
        Service(const SERVICE_id&, const std::string&  nm, IInstance *ins);
        ~Service();

    public:
        void deinit()
        {
            ListenerBuffered1Thread::deinit();
        }

        static UnknownBase* construct(const SERVICE_id& id, const std::string&  nm,IInstance* obj)
        {
            return new Service(id,nm,obj);
        }
        IInstance * iInstance;

    };

}
#endif

