#include <stdio.h>
#include "DBH.h"
#include "IInstance.h"
#include "sqliteService.h"
#include <string>
#include "sqlite3.h"
#include "commonError.h"
#ifdef _WIN32
#include "compat_win32.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <malloc.h>
#endif
#include <deque>
#include <stdarg.h>
#include "IConfigObj.h"
#include "IInstance.h"
#include "DBHEvent.h"
#include "_QUERY.h"
#include "version_mega.h"
#include "VERSION_id.h"
#include "mutexInspector.h"
#include "app_config.h"

static Mutex __lock;
bool DBH_sqlite::bIsValid()
{

    if (!dbh) return false;
    if (bIsLost) return false;
    return true;
}

void DBH_sqlite::execSimple(const QUERY & q)
{
    MUTEX_INSPECTOR;
    exec(q);
}
void DBH_sqlite::execSimple(const std::string & q)
{
    MUTEX_INSPECTOR;
    exec(q);
}
REF_getter<QueryResult> DBH_sqlite::exec(const QUERY &query)
{
    MUTEX_INSPECTOR;
    return exec(query.prepare());
}

REF_getter<QueryResult> DBH_sqlite::exec(const std::string &query)
{
    MUTEX_INSPECTOR;
    //M_LOCK(lock);

    REF_getter<QueryResult> rr(new QueryResult);

    m_lastQueryTime=time(NULL);
    sqlite3_stmt *statement;

    if(sqlite3_prepare_v2(dbh, query.c_str(), -1, &statement, 0) == SQLITE_OK)
    {

        int cols = sqlite3_column_count(statement);
        int result = 0;
        while(true)
        {

            result = sqlite3_step(statement);


            if(result == SQLITE_ROW)
            {

                std::vector<std::string> values;
                for(int col = 0; col < cols; col++)
                {

                    sqlite3_value* val=sqlite3_column_value(statement,col);
                    int len=sqlite3_value_bytes(val);
                    const void *ptr=sqlite3_value_blob(val);
                    if(ptr)
                        values.push_back(std::string((char*)ptr,len));
                    else
                    {
                        values.push_back("");
                    }


                }
                rr->values.push_back(values);


            }
            else
            {
                break;
            }


        }
        //else throw CommonError("sqlite error: %s (%s)",sqlite3_errmsg(dbh),query.c_str());

        if(sqlite3_finalize(statement)!=SQLITE_OK)CommonError("sqlite error: %s (%s) %s",sqlite3_errmsg(dbh),query.c_str(),_DMI().c_str());;
        std::string error = sqlite3_errmsg(dbh);
        if(error != "not an error") logErr2("%s: %s", query.c_str(),error.c_str());
        return rr;

    }
    else throw CommonError("sqlite error: %s (%s) (%s)",sqlite3_errmsg(dbh),query.c_str(),_DMI().c_str());

    return rr;
}

std::string DBH_sqlite::escape(const std::string& s)
{
    MUTEX_INSPECTOR;
    return s;
}
void DBH_sqlite::makeConnection(const sqliteConf& conf)
{
    MUTEX_INSPECTOR;
    if(sqlite3_open(conf.database.c_str(), &dbh) == SQLITE_OK)
        return;
    logErr2("sqlite3: %s",sqlite3_errmsg(dbh));


    return ;

}
DBH_sqlite::~DBH_sqlite()
{
    MUTEX_INSPECTOR;

    if (dbh)
    {
        if(sqlite3_close(dbh)==SQLITE_OK)
        {
            dbh=NULL;
        }
        else
            logErr2("sqlite3_close: %s",sqlite3_errmsg(dbh));
    }
}



DBH_sqlite::DBH_sqlite(/*IConfigObj *conf*/)
    :DBH(),dbh(NULL),bIsLost(false),m_lastQueryTime(time(NULL))
{
}

DBH_source_sqlite::DBH_source_sqlite(const SERVICE_id& _id, const std::string& nm,IInstance* obj):
    UnknownBase(nm),
    ListenerSimple(this,nm,obj->getConfig(),_id),
    DBH_source(this),
    max_connections(10)
{
    MUTEX_INSPECTOR;
    cfg.max_connections=obj->getConfig()->get_int64_t("max_connections",20,"");
    cfg.database=obj->getConfig()->get_string("sqlDatabase","def.db","");
}

REF_getter<DBH> DBH_source_sqlite::get()
{
    MUTEX_INSPECTOR;
    //(NULL);
    __lock.lock();
    int nOpenedConnections;
    {
        M_LOCK(this);
        nOpenedConnections=container_MX.size();
        {
            while (1)
            {
                if (container_MX.size())
                {
                    REF_getter<DBH> dbh=*container_MX.begin();
                    container_MX.pop_front();
                    DBH_sqlite* dsqlite=(DBH_sqlite* )dbh.operator ->();
                    if (time(NULL)-dsqlite->getLastQueryTime()>INACTIVE_CONNECTION_DROP_TIMEOUT_SEC)
                    {
                        //delete dbh;
                        //dbh=NULL;
                        continue;
                    }
                    return dbh;
                }
                else break;
            }
        }
    }
    if (nOpenedConnections<max_connections)
    {
        DBH_sqlite* dbh=new DBH_sqlite;//(confconfig);
        dbh->makeConnection(cfg);
        return dbh;
    }
    else
    {
        throw CommonError("database maximum connection exceeded [%d]",max_connections);
    }
}

void DBH_source_sqlite::unget(const REF_getter<DBH>& dbh)
{

    MUTEX_INSPECTOR;
    if (dbh->bIsValid())
    {
        {
            M_LOCK(this);
            container_MX.push_back(dbh);
        }
    }
    else
    {
    }
    __lock.unlock();

}
time_t DBH_sqlite::getLastQueryTime()
{
    return m_lastQueryTime;
}
void registerSqliteModule()
{
    iUtils->registerService(COREVERSION,ServiceEnum::sqlite3,DBH_source_sqlite::construct,"sqlite3");
}

UnknownBase* DBH_source_sqlite::construct(const SERVICE_id& id, const std::string&  nm,IInstance* obj)
{
    return new DBH_source_sqlite(id,nm,obj);
}

