#include <stdio.h>
#include "IInstance.h"
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
#include "_QUERY.h"
#include "version_mega.h"
#include "VERSION_id.h"
#include "mutexInspector.h"

#include "sqlite3.h"
#include "sqlite3Wrapper.h"
#include "st_FILE.h"



void Sqlite3Wrapper::execSimple(const QUERY & q)const
{
    exec(q);
}
void Sqlite3Wrapper::execSimple(const std::string & q) const
{
    exec(q);
}
REF_getter<QueryResult> Sqlite3Wrapper::exec(const QUERY &query) const
{
    return exec(query.prepare());
}
#ifdef SIMPLE_EXEC
REF_getter<QueryResult> Sqlite3Wrapper::exec(const std::string &query) const
{
    char *zErrMsg = 0;
    REF_getter<QueryResult> r=new QueryResult;
    int rc = sqlite3_exec(dbh, query.c_str(), callback, (void*)r.operator ->(), &zErrMsg);
    if( rc != SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        std::string err=zErrMsg;
        sqlite3_free(zErrMsg);

#ifdef DEBUG
        std::string str=iUtils->getIThreadNameController()->dump_mutex_inspectors();
#else
        std::string str="";
#endif
        throw CommonError("sqlite error: %s (%s) (%s)",err.c_str(),query.c_str(),str.c_str());
    } else {
        //fprintf(stdout, "Operation done successfully\n");
    }
    return r;
//    sqlite3_close(db);

}

#else

REF_getter<QueryResult> Sqlite3Wrapper::exec(const std::string &query) const
{
    MUTEX_INSPECTOR;
#ifdef DEBUG
#endif
    REF_getter<QueryResult> rr(new QueryResult);
    sqlite3_stmt *statement;
    if(sqlite3_prepare_v2(dbh, query.c_str(), -1, &statement, 0) == SQLITE_OK)
    {
        MUTEX_INSPECTOR;

        int cols = sqlite3_column_count(statement);
        int result = 0;
        while(true)
        {
            MUTEX_INSPECTOR;

            result = sqlite3_step(statement);



            if(result == SQLITE_ROW)
            {
                MUTEX_INSPECTOR;
                std::vector<std::string> values;
                for(int col = 0; col < cols; col++)
                {
                    MUTEX_INSPECTOR;
                    sqlite3_value* val=sqlite3_column_value(statement,col);
                    int len=sqlite3_value_bytes(val);
                    const void *ptr=sqlite3_value_blob(val);
                    if(ptr)
                    {
                        MUTEX_INSPECTOR;
                        values.push_back(std::string((char*)ptr,len));
                    }
                    else
                    {
                        MUTEX_INSPECTOR;
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



        if(sqlite3_finalize(statement)!=SQLITE_OK)
        {
            MUTEX_INSPECTOR;
#ifdef DEBUG
            std::string str=iUtils->getIThreadNameController()->dump_mutex_inspectors();
#else
            std::string str="";
#endif
            throw CommonError("sqlite error: %s (%s) %s",sqlite3_errmsg(dbh),query.c_str(),str.c_str());;
        }


        std::string error = sqlite3_errmsg(dbh);
        if(error != "not an error") logErr2("%s: %s", query.c_str(),error.c_str());

        return rr;

    }
    else
    {
#ifdef DEBUG
        std::string str=iUtils->getIThreadNameController()->dump_mutex_inspectors();
#else
        std::string str="";
#endif
        throw CommonError("sqlite error: %s (%s) (%s)",sqlite3_errmsg(dbh),query.c_str(),str.c_str());
    }
    return rr;
}
#endif
static Mutex fullSqliteMutex;
Sqlite3Wrapper::~Sqlite3Wrapper()
{

    if(inTransaction)
        rollbackTransaction();


    //execSimple((std::string)"COMMIT");



    try {
        if (dbh)
        {

            if(sqlite3_close(dbh)==SQLITE_OK)
            {

                dbh=NULL;
            }
            else
            {
                logErr2("sqlite3_close: %s",sqlite3_errmsg(dbh));
            }


        }
    }
    catch(...)
    {

        logErr2("catched ... %s %d",__FILE__,__LINE__);
    }

    fullSqliteMutex.unlock();


}


void Sqlite3Wrapper::beginTransaction()
{
    if(inTransaction)
        return;
    inTransaction=true;
    execSimple((QUERY)"BEGIN");
}
void Sqlite3Wrapper::commitTransaction()
{
    if(inTransaction)
    {
        inTransaction=false;
        execSimple((QUERY)"COMMIT");
    }
}
void Sqlite3Wrapper::rollbackTransaction()
{
    if(inTransaction)
    {
        inTransaction=false;
        execSimple((QUERY)"ROLLBACK");
    }

}


Sqlite3Wrapper::Sqlite3Wrapper(const std::string& name):dbname(name),inTransaction(false),noErrors(true)
{
    {
        MUTEX_INSPECTOR;
        fullSqliteMutex.lock();
    }

    MUTEX_INSPECTOR;
    std::string pn;
    std::string an=iUtils->app_name();
    if(an.size()==0)
        an="app";


    {
        MUTEX_INSPECTOR;
        pn=iUtils->gCacheDir()+"/"+an+"-"+dbname+".db";
        iUtils->checkPath(iUtils->gCacheDir());
    }

    {
        MUTEX_INSPECTOR;
        if(sqlite3_open_v2(pn.c_str(), &dbh,SQLITE_OPEN_EXCLUSIVE|SQLITE_OPEN_CREATE|SQLITE_OPEN_READWRITE|SQLITE_OPEN_FULLMUTEX,NULL) == SQLITE_OK)
        {

            //execSimple((std::string)"BEGIN");
            return;
        }
    }

    throw CommonError("sqlite3: %s",sqlite3_errmsg(dbh));

}

std::string Sqlite3Wrapper::select_1(const QUERY &q) const
{
    MUTEX_INSPECTOR;
    REF_getter<QueryResult> r=exec(q);
    if(r->values.size()!=1) return "";
    if(r->values[0].size()!=1) throw CommonError("if(r->values[0].size()!=1) %s %d",__FILE__,__LINE__);
    if(r->values[0].size()==1)
    {
        return r->values[0][0];
    }
    return "";
}
std::string Sqlite3Wrapper::select_1(const std::string &q) const
{
    MUTEX_INSPECTOR;
    REF_getter<QueryResult> r=exec(q);
    if(r->values.size()!=1) return "";
    if(r->values.size()==1)
    {
        if(r->values[0].size()!=1) throw CommonError("if(r->values[0].size()!=1) %s %d",__FILE__,__LINE__);
        if(r->values[0].size()==1)
        {
            return r->values[0][0];
        }
    }
    std::string rr;
    return rr;
}

std::vector<std::string> Sqlite3Wrapper::select_1_row(const std::string&q) const
{
    MUTEX_INSPECTOR;
    REF_getter<QueryResult> r=exec(q);
    if(r->values.size()>1) throw CommonError("if(r->values.size()>1) %s %d %s",__FILE__,__LINE__,_DMI().c_str());
    if(r->values.size())
        return r->values[0];

    std::vector<std::string> ret;
    return ret;

}
std::vector<std::string> Sqlite3Wrapper::select_1_row(const QUERY&q) const
{
    MUTEX_INSPECTOR;
    REF_getter<QueryResult> r=exec(q);
    if(r->values.size()>1) throw CommonError("if(r->values.size()>1) %s %d %s",__FILE__,__LINE__,_DMI().c_str());
    if(r->values.size())
        return r->values[0];

    std::vector<std::string> ret;
    return ret;

}

std::vector<std::string> Sqlite3Wrapper::select_1_column(const QUERY&q) const
{
    MUTEX_INSPECTOR;
    std::vector<std::string>  ret;
    REF_getter<QueryResult> r=exec(q);
    for(size_t i=0; i<r->values.size(); i++)
    {
        if(r->values[i].size()!=1) throw CommonError("if(r->values[i].size()!=1)" +_DMI());
        ret.push_back(r->values[i][0]);
    }
    return ret;

}
std::vector<std::string> Sqlite3Wrapper::select_1_column(const std::string&q) const
{
    MUTEX_INSPECTOR;
    std::vector<std::string>  ret;
    REF_getter<QueryResult> r=exec(q);
    for(size_t i=0; i<r->values.size(); i++)
    {
        if(r->values[i].size()!=1) throw CommonError("if(r->values[i].size()!=1)" +_DMI());
        ret.push_back(r->values[i][0]);
    }
    return ret;

}
