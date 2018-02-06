#include "mutexInspector.h"
#include "dfsTimers.h"
#include "timerEvent.h"
#include "tools.h"
#include "dfsKeygenService.h"
#include "errorDispatcherCodes.h"
#include "dfsReferrerEvent.h"
#include "app_config.h"
#include "ISSL.h"
#include "dfsKeygenEvent.h"
#include "dfsCapsClientIFace.h"
#include "tbl_names.h"
#include "st_rsa.h"

extern "C" unsigned char ___rsa_private_key[];
extern "C" unsigned char ___rsa_private_key_sz;

bool dfsKeygen::Service::on_CapsOperationREQ(const dfsCapsEvent::CapsOperationREQ *e)
{
    if(e->opcode==dfsCapsEvent::_KeygenCheckUserLogin)
    {
        return true;
    }
    else if(e->opcode==dfsCapsEvent::_KeygenRegisterUser)
    {
        inBuffer in(e->data);
        std::string login=in.get_PSTR();
        std::string password=in.get_PSTR();
        Sqlite3Wrapper dbh(dbname);
        std::vector<std::string>v=dbh.select_1_column((QUERY)"SELECT id FROM "TBL_user" WHERE "TBL_user$login" like x'?'"<<iUtils->bin2hex(login));
        if(v.size())
        {
            route_t r=e->route;
            r.pop_front();
            passEvent(new dfsCapsEvent::CapsOperationRSP(e->opcode,ERR_NAME_ALREADY_EXISTS,toRef("login alredy registered"),NULL,r));
            return true;
        }
        dbh.execSimple((QUERY)"INSERT INTO "TBL_user" ("TBL_user$login","TBL_user$passSHA1") VALUES (x'?',x'?')"
                       <<iUtils->bin2hex(login)
                       <<iUtils->bin2hex(password)
                      );
        std::string id=dbh.select_1((std::string)"SELECT last_insert_rowid()");
        if(id.size()==0)
            throw CommonError("if(id.size()==0)");

        st_rsa rsa;
        rsa.initFromPrivateKey(std::string((char*)___rsa_private_key,___rsa_private_key_sz));
        std::string enc=rsa.

                        //outBuffer o;
                        return true;
    }
    else if(e->opcode==dfsCapsEvent::_KeygenGetUserAmount)
    {
        outBuffer o;
        o<<0<<0;
        route_t r=e->route;
        r.pop_front();
        passEvent(new dfsCapsEvent::CapsOperationRSP(e->opcode,ERR_NONE,o.asString(),NULL,r));
        return true;
    }
    else throw CommonError("invalid opcode %d",e->opcode);
    return false;
}
bool dfsKeygen::Service::handleEvent(const REF_getter<Event::Base>& e)
{
    MUTEX_INSPECTOR;
    switch(CONTAINER(e->id))
    {
    case rpcEventEnum::IncomingOnConnector:
    {
        const rpcEvent::IncomingOnConnector*ev=static_cast<const rpcEvent::IncomingOnConnector*>(e.operator ->());
        switch(CONTAINER(ev->e->id))
        {
        case dfsCapsEventEnum::CapsOperationREQ:
            return on_CapsOperationREQ((const dfsCapsEvent::CapsOperationREQ*)ev->e.operator ->());
        }
        return true;
    }
    case dfsCapsEventEnum::CapsOperationREQ:
        return on_CapsOperationREQ((const dfsCapsEvent::CapsOperationREQ*)e.operator ->());
    }
    return false;
}

bool dfsKeygen::Service::on_startService(const systemEvent::startService*)
{
    MUTEX_INSPECTOR;
    Sqlite3Wrapper dbh(dbname);

    dbh.execSimple((std::string)"CREATE TABLE  IF NOT EXISTS "TBL_user" ("
                   TBL_user$id" INTEGER PRIMARY KEY,"
                   TBL_user$login" BLOB,"
                   TBL_user$passSHA1" BLOB,"
                   TBL_user$amount" INTEGER DEFAULT 0"
                   ")");
    dbh.execSimple((std::string)"CREATE INDEX IF NOT EXISTS idx_"TBL_user TBL_user$login" ON "TBL_user"("TBL_user$login")");


    return true;
}

bool dfsKeygen::Service::on_CommandEntered(const telnetEvent::CommandEntered*)
{
    return true;
}
bool dfsKeygen::Service::on_RequestIncoming(const webHandlerEvent::RequestIncoming* )
{
    return true;
}

dfsKeygen::Service::~Service()
{
    MUTEX_INSPECTOR;

}


dfsKeygen::Service::Service(const SERVICE_id& id, const std::string& nm,IConfigObj* config):
    UnknownBase(nm),
    ListenerBuffered1Thread(this,nm,config,id),
    System::InterfaceEvent(this),
    Telnet::InterfaceClient(this),
    WebHandler::InterfaceClient(this),
    Logging(this,
#ifdef DEBUG
            TRACE_log
#else
            ERROR_log
#endif
           ),
    dbname(Keygen_DBNAME),

{
}


void registerDFSKeygenService(const char* pn)
{
    /// регистрация в фабрике сервиса и событий

    XTRY;
    if(pn)
    {
        iUtils->registerPlugingInfo(COREVERSION,pn,IUtils::PLUGIN_TYPE_SERVICE,ServiceEnum::DFSKeygen,"DFSKeygen");
    }
    else
    {
        iUtils->registerService(COREVERSION,ServiceEnum::DFSKeygen,dfsKeygen::Service::construct,"DFSKeygen");
        System::InterfaceEvent::regEvents();
        Telnet::InterfaceClient::regEvents();
        WebHandler::InterfaceClient::regEvents();
        DFSCaps::InterfaceClient::regEvents();
    }
    XPASS;
}


