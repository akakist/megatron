#include "mutexInspector.h"
#include <time.h>
#include <time.h>
#include <map>
#include "teldemo1Service.h"
#include "events_teldemo1.hpp"



bool teldemo1::Service::on_startService(const systemEvent::startService*)
{
    MUTEX_INSPECTOR;

    sendEvent(ServiceEnum::Telnet,new telnetEvent::RegisterCommand("demo1","push_back","push data into queue back","STRING",ListenerBase::serviceId));
    sendEvent(ServiceEnum::Telnet,new telnetEvent::RegisterCommand("demo1","push_front","push data into queue front","STRING",ListenerBase::serviceId));
    sendEvent(ServiceEnum::Telnet,new telnetEvent::RegisterCommand("demo1","pop_front","pop data from front of queue","",ListenerBase::serviceId));
    sendEvent(ServiceEnum::Telnet,new telnetEvent::RegisterCommand("demo1","pop_back","pop data from back of queue","",ListenerBase::serviceId));
    sendEvent(ServiceEnum::Telnet,new telnetEvent::RegisterCommand("demo1","front","get first element from queue","",ListenerBase::serviceId));
    sendEvent(ServiceEnum::Telnet,new telnetEvent::RegisterCommand("demo1","back","get last element from queue","",ListenerBase::serviceId));
    return true;
}

bool  teldemo1::Service::on_CommandEntered(const telnetEvent::CommandEntered *e)
{
    if(e->tokens.size())
    {
        if(e->tokens[0]=="push_back")
        {
            sampleDeque.push_back(e->tokens[1]);
            sendEvent(ServiceEnum::Telnet,new telnetEvent::Reply(e->socketId,"OK",ListenerBase::serviceId));
            return true;
        }
        else if(e->tokens[0]=="push_front")
        {
            sampleDeque.push_front(e->tokens[1]);
            sendEvent(ServiceEnum::Telnet,new telnetEvent::Reply(e->socketId,"OK",ListenerBase::serviceId));
            return true;
        }
        else if(e->tokens[0]=="pop_front")
        {
            sampleDeque.pop_front();
            sendEvent(ServiceEnum::Telnet,new telnetEvent::Reply(e->socketId,"OK",ListenerBase::serviceId));
            return true;
        }
        else if(e->tokens[0]=="pop_back")
        {
            sampleDeque.pop_back();
            sendEvent(ServiceEnum::Telnet,new telnetEvent::Reply(e->socketId,"OK",ListenerBase::serviceId));
            return true;
        }
        else if(e->tokens[0]=="front")
        {
            sendEvent(ServiceEnum::Telnet,new telnetEvent::Reply(e->socketId,sampleDeque.front(),ListenerBase::serviceId));
            return true;
        }
        else if(e->tokens[0]=="back")
        {
            sendEvent(ServiceEnum::Telnet,new telnetEvent::Reply(e->socketId,sampleDeque.back(),ListenerBase::serviceId));
            return true;
        }
    }

    return true;
}

bool teldemo1::Service::handleEvent(const REF_getter<Event::Base>& e)
{
    XTRY;
    try {
        MUTEX_INSPECTOR;
        auto& ID=e->id;
        if(timerEventEnum::TickTimer==ID)
        {
            const timerEvent::TickTimer*ev=static_cast<const timerEvent::TickTimer*>(e.get());
            return true;
        }
        if(telnetEventEnum::CommandEntered==ID)
            return on_CommandEntered((const telnetEvent::CommandEntered*)e.get());
        if(systemEventEnum::startService==ID)
            return on_startService((const systemEvent::startService*)e.get());



        if(rpcEventEnum::IncomingOnConnector==ID)
        {
            rpcEvent::IncomingOnConnector *E=(rpcEvent::IncomingOnConnector *) e.get();
            auto& IDC=E->e->id;


            return false;
        }

        if(rpcEventEnum::IncomingOnAcceptor==ID)
        {
            rpcEvent::IncomingOnAcceptor *E=(rpcEvent::IncomingOnAcceptor *) e.get();
            auto& IDA=E->e->id;

            return false;
        }


    }
    catch(const CommonError& e)
    {
        logErr2("teldemo1WebServer CommonError  %s",e.what());

    }
    catch(const std::exception &e)
    {
        logErr2("teldemo1WebServer std::exception  %s",e.what());
    }

    logErr2("teldemo1: unhandled event %s",e->dump().toStyledString().c_str());
    XPASS;
    return false;
}

teldemo1::Service::~Service()
{
}


teldemo1::Service::Service(const SERVICE_id& id, const std::string& nm,IInstance* ins):
    UnknownBase(nm),
    ListenerBuffered1Thread(nm,id),
    Broadcaster(ins)
{

}

void registerteldemo1(const char* pn)
{
    MUTEX_INSPECTOR;

    XTRY;
    if(pn)
    {
        iUtils->registerPlugingInfo(COREVERSION,pn,IUtils::PLUGIN_TYPE_SERVICE,ServiceEnum::teldemo1,"teldemo1",getEvents_teldemo1());
    }
    else
    {
        iUtils->registerService(COREVERSION,ServiceEnum::teldemo1,teldemo1::Service::construct,"teldemo1");
        regEvents_teldemo1();
    }
    XPASS;
}






static int cnt2;
