#include "corelib/mutexInspector.h"
#include <time.h>
#include <time.h>
#include <map>
#include "prodtestWebServerService.h"
#include "Events/Ping.h"
#include "events_prodtestWebServer.hpp"
#include "version_mega.h"



bool prodtestWebServer::Service::on_startService(const systemEvent::startService*)
{
    MUTEX_INSPECTOR;

    auto svs=dynamic_cast<ListenerBase*> (iInstance->getServiceOrCreate(ServiceEnum::HTTP));
    if(!svs)
        throw CommonError("if(!svs)");

    sendEvent(svs,new httpEvent::DoListen(bindAddr,this));
    return true;
}


bool prodtestWebServer::Service::handleEvent(const REF_getter<Event::Base>& e)
{
    XTRY;
    try {
        MUTEX_INSPECTOR;
        auto& ID=e->id;
        if(timerEventEnum::TickTimer==ID)
        {
            const timerEvent::TickTimer*ev=static_cast<const timerEvent::TickTimer*>(e.get());
            if(ev->tid==TIMER_PUSH_NOOP)
            {
                for(auto &z: sessions)
                {
                    auto esi=z.second->esi;
                    if(esi.valid())
                    {
                        if(!esi->closed())
                        {
                            esi->write_("   ");
                        }
                    }
                    else logErr2("!if(esi.valid())");
                }
            }
            return true;
        }
        if(httpEventEnum::RequestIncoming==ID)
            return on_RequestIncoming((const httpEvent::RequestIncoming*)e.get());
        if(systemEventEnum::startService==ID)
            return on_startService((const systemEvent::startService*)e.get());


        if(prodtestEventEnum::AddTaskRSP==ID)
            return on_AddTaskRSP((const prodtestEvent::AddTaskRSP*)e.get());

        if(rpcEventEnum::IncomingOnConnector==ID)
        {
            rpcEvent::IncomingOnConnector *E=(rpcEvent::IncomingOnConnector *) e.get();
            auto& IDC=E->e->id;
            if(prodtestEventEnum::AddTaskRSP==IDC)
                return on_AddTaskRSP((const prodtestEvent::AddTaskRSP*)E->e.get());


            return false;
        }

        if(rpcEventEnum::IncomingOnAcceptor==ID)
        {
            rpcEvent::IncomingOnAcceptor *E=(rpcEvent::IncomingOnAcceptor *) e.get();
            auto& IDA=E->e->id;
            if(prodtestEventEnum::AddTaskRSP==IDA)
                return on_AddTaskRSP((const prodtestEvent::AddTaskRSP*)E->e.get());


            return false;
        }


    }
    catch(const CommonError& e)
    {
        logErr2("prodtestWebServer CommonError  %s",e.what());

    }
    catch(const std::exception &e)
    {
        logErr2("prodtestWebServer std::exception  %s",e.what());
    }
    XPASS;
    return false;
}

prodtestWebServer::Service::~Service()
{
}


prodtestWebServer::Service::Service(const SERVICE_id& id, const std::string& nm,IInstance* ins):
    UnknownBase(nm),
    ListenerBuffered1Thread(nm,id),
    Broadcaster(ins), iInstance(ins)
{
    auto ba=ins->getConfig()->get_tcpaddr("bindAddr","0.0.0.0:8088","http listen address");
    if(ba.size()==0)
        throw CommonError("if(ba.size()==0)");

    bindAddr=*ba.begin();
    prodtestServerAddr=ins->getConfig()->get_string("prodtestServerAddr","local","server prodtest address");

}

void registerprodtestServerWebService(const char* pn)
{
    MUTEX_INSPECTOR;

    XTRY;
    if(pn)
    {
        iUtils->registerPlugingInfo(COREVERSION,pn,IUtils::PLUGIN_TYPE_SERVICE,ServiceEnum::prodtestServerWeb,"prodtestServerWeb",getEvents_prodtestWebServer());
    }
    else
    {
        iUtils->registerService(COREVERSION,ServiceEnum::prodtestServerWeb,prodtestWebServer::Service::construct,"prodtestServerWeb");
        regEvents_prodtestWebServer();
    }
    XPASS;
}




bool prodtestWebServer::Service::on_RequestIncoming(const httpEvent::RequestIncoming*e)
{

    HTTP::Response resp(getIInstance());
    auto S=create_session(e->req,resp,e->esi);
    S->esi=e->esi;

    {
        std::string query_string=e->req->params["query_string"];
        {
            sendEvent(prodtestServerAddr,ServiceEnum::prodtestServer,new prodtestEvent::AddTaskREQ(S->sessionId,
                      query_string,0,ListenerBase::serviceId));
        }
        return true;
    }

    return true;
}

REF_getter<prodtestWebServer::Session> prodtestWebServer::Service::create_session( const REF_getter<HTTP::Request>& req, HTTP::Response& resp, const REF_getter<epoll_socket_info>& esi)
{

    auto session_id=cnt2++;
    REF_getter<Session>S=new Session(session_id,req,esi);
    {
        sessions.insert(std::make_pair(session_id,S));
    }
    return S;

}
REF_getter<prodtestWebServer::Session> prodtestWebServer::Service::get_session(int session_id)
{

    REF_getter<Session> S(nullptr);
    auto it=sessions.find(session_id);
    if(it!=sessions.end())
        S=it->second;
    else
    {
        throw CommonError("session not found %d",session_id);
    }
    return S;

}

bool prodtestWebServer::Service::on_AddTaskRSP(const prodtestEvent::AddTaskRSP*e)
{
    // printf("@@@@ on_AddTaskRSP \n");
    if(e->count==0)
    {

        HTTP::Response resp(getIInstance());
        auto S=get_session(e->session);
        bool keepAlive=S->req->headers["Connection"]=="Keep-Alive";
        // keepAlive=true;
        if(keepAlive)
        {
            resp.http_header_out["Connection"]="Keep-Alive";
            resp.http_header_out["Keep-Alive"]="timeout=5, max=100000";
        }
        resp.content="<div>received response </div>";
        if(keepAlive)
            resp.makeResponsePersistent(S->esi);
        else
            resp.makeResponse(S->esi);

    }
    else
    {
        sendEvent(prodtestServerAddr,ServiceEnum::prodtestServer,new prodtestEvent::AddTaskREQ(e->session,e->sampleString,e->count-1,ListenerBase::serviceId));

    }

    return true;
}
