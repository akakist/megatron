#ifndef _______________displayPLAY___H
#define _______________displayPLAY___H
//#include "local_SDL.h"
//#include "videoState.h"
#include "broadcaster.h"
#include "systemEventIFace.h"
#include "telnetClientIFace.h"
#include "webHandlerClientIFace.h"
#include "logging.h"
#include "listenerBuffered1Thread.h"
namespace dfsKeygen
{

    class Service:
        private UnknownBase,
        private ListenerBuffered1Thread,
        private Broadcaster,
        private System::InterfaceEvent,
        private Telnet::InterfaceClient,
        public WebHandler::InterfaceClient,
        public Logging
    {
        bool on_startService(const systemEvent::startService*);
        bool handleEvent(const REF_getter<Event::Base>& e);



        Service(const SERVICE_id&, const std::string&  nm,IConfigObj* obj);
        ~Service();


        bool on_CommandEntered(const telnetEvent::CommandEntered*);
    public:
        static UnknownBase* construct(const SERVICE_id& id, const std::string&  nm,IConfigObj* obj)
        {
            return new Service(id,nm,obj);
        }
        bool on_RequestIncoming(const webHandlerEvent::RequestIncoming* e);


        bool on_CapsOperationREQ(const dfsCapsEvent::CapsOperationREQ*);

        const std::string dbname;

    };

};
#endif

