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
#include "Events/Tools/telnet/CommandEntered.h"
#include "Events/Tools/webHandler/RequestIncoming.h"
#include "Events/System/timer/TickTimer.h"

#include "Events/System/Net/rpc/IncomingOnAcceptor.h"
#include "Events/System/Net/rpc/IncomingOnConnector.h"
#include "../Event/Ping.h"

#include "version_mega.h"


namespace prodtestServer
{


    class Service:
        public UnknownBase,
        public ListenerBuffered1Thread,
        public Broadcaster
    {
        bool on_startService(const systemEvent::startService*);
        bool handleEvent(const REF_getter<Event::Base>& e);




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


        bool on_AddTaskREQ(const prodtestEvent::AddTaskREQ* e);









    };

}
#endif

