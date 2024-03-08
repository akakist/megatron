#ifndef _______________displayzzPLAYBCNODE___H1INV
#define _______________displayzzPLAYBCNODE___H1INV
#include "broadcaster.h"



#include "logging.h"
#include "listenerBuffered1Thread.h"
#include "DBH.h"
#include "USER_id.h"
#include "IProgress.h"
#include "tools_mt.h"
#include <map>



#include "Real.h"
#include "Events/System/Run/startService.h"
#include "Events/System/timer/TickTimer.h"

#include "Events/System/Net/http/DoListen.h"
#include "Events/System/Net/http/RequestIncoming.h"
#include "Events/System/Net/rpc/IncomingOnAcceptor.h"
#include "Events/System/Net/rpc/IncomingOnConnector.h"
#include "version_mega.h"

namespace testWebServer
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


        bool on_RequestIncoming(const httpEvent::RequestIncoming*);



    public:
        void deinit()
        {
            ListenerBuffered1Thread::deinit();
        }

        static UnknownBase* construct(const SERVICE_id& id, const std::string&  nm,IInstance* obj)
        {
            return new Service(id,nm,obj);
        }
        msockaddr_in bindAddr;

    };

}
#endif

