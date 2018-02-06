#ifndef __________________DBH___H______
#define __________________DBH___H______
#include <vector>
#include <string>
#include "_QUERY.h"
#include "REF.h"
#include "IConfigObj.h"
#include "unknown.h"
#include "IInstance.h"
#include "unknownCastDef.h"
#include "pthread.h"
#include "serviceEnum.h"
#define SQL_BUFFER_SIZE 4096
/**  Универсальный device independant database handler
*/
namespace ServiceEnum
{
    enum
    {
        Mysql=_xs_Mysql,
        Postgres=_xs_Postgres,

    };

}

class DBH: public Refcountable
{
public:
    enum engine {mysql,sqlite,postgres};
    ///  Выполнить SQL без возврата значений.
    ///  Example: execSimple((QEURY)"insert into table tbl (a,b,c) values(?,?,?)"<<a<<b<<c);
    virtual void execSimple(const QUERY &query)=0;

    ///   Выполнить SQL с возвратом QueryResult.
    virtual REF_getter<QueryResult> exec(const QUERY &)=0;

    ///   Выполнить SQL без возврата значений.
    ///   Example: execSimple((QEURY)"insert into table tbl (a,b,c) values(?,?,?)"<<a<<b<<c);
    virtual void execSimple(const std::string &query)=0;

    ///   Выполнить SQL с возвратом QueryResult.
    virtual REF_getter<QueryResult> exec(const std::string &)=0;

    ///   Выполнить замену спецсимволов на теговые.
    virtual std::string escape(const std::string&)=0;

    virtual time_t getLastQueryTime()=0;

    virtual ~DBH() {}
    virtual bool bIsValid()=0;
    std::string select_1(const QUERY &);
    int64_t select_1_int(const QUERY &);
    std::string select_1_orThrow(const QUERY &);
    int64_t select_1_int_orThrow(const QUERY &);

    /// получить 1 строку. Примечание строка должна действительно выбраться.
    std::vector<std::string> select_1_row(const QUERY&);

    /// получить 1 столбец. Примечание столбец должен действительно выбраться.
    std::vector<std::string> select_1_column(const QUERY&);
    std::set<std::string> select_1_columnSet(const QUERY&q);


    /// получить 1 элемент из запроса типа "select last_insert_rowid()". Примечание element должeн действительно выбраться.
    std::string select_1(const std::string &);
    std::string select_1_orThrow(const std::string &);

    /// получить 1 строку. Примечание строка должна действительно выбраться.
    std::vector<std::string> select_1_row(const std::string &);

    /// получить 1 столбец. Примечание столбец должен действительно выбраться.
    std::vector<std::string> select_1_column(const std::string &);
    std::set<std::string> select_1_columnSet(const std::string&q);

    /// сконструировать мап для инсерта
    static void insertStrVal(std::map<std::string,std::string>&m,const std::string& name,const char* prefix, const std::string& val, const char* postfix);
    /// получить инсерт в виде (,,,,) values (,,,)
    static std::string insertString(const std::map<std::string,std::string>&m, const char* tblname);

protected:
};
class DBH_source
{
public:
    virtual REF_getter<DBH> get()=0;
    virtual void unget(const REF_getter<DBH>&)=0;
    static DBH_source* cast(UnknownBase *c);
    DBH_source(UnknownBase*);
    virtual ~DBH_source() {}
};
class st_DBH
{
private:
    DBH_source* m_source;
    bool isTransaction;
    bool transactionResult;
public:
    REF_getter<DBH> dbh;

    st_DBH(DBH_source* src, bool transaction);
    ~st_DBH();
};
struct DBH_feature
{
private:
    DBH_source *dbh_src;
    Mutex mx;
    std::map<pthread_t,std::pair<REF_getter<DBH>,time_t > > m_sources;
public:
    DBH_feature(IInstance* ifa)
    {
        dbh_src=static_cast<DBH_source*>(ifa->getServiceOrCreate(ServiceEnum::Mysql)->cast(UnknownCast::DBH_source));
    }
    REF_getter<DBH> getDB()
    {
        pthread_t pt=pthread_self();
        M_LOCK(mx);
        std::map<pthread_t,std::pair<REF_getter<DBH>,time_t > >::iterator it=m_sources.find(pt);
        if(it!=m_sources.end())
        {
            if(time(NULL)-it->second.second>600)
            {
                m_sources.erase(pt);
            }
            else
            {
                return m_sources.find(pt)->second.first;
            }
        }

        REF_getter<DBH> d = dbh_src->get();
        m_sources.insert(std::make_pair(pt,std::make_pair(d,time(NULL))));
        return d;
    }
};
struct st_TRANSACTION
{
    REF_getter<DBH> dbh;
    enum
    {
        ___UNDEF,___COMMIT,___ROLLBACK
    };
    int exitState;
    void commit()
    {
        exitState=___COMMIT;
    }
    void rollback()
    {
        exitState=___ROLLBACK;
    }
    st_TRANSACTION(const REF_getter<DBH> &df):dbh(df),exitState(___ROLLBACK)
    {
        dbh->execSimple((std::string)"BEGIN");

    }
    ~st_TRANSACTION()
    {
        switch (exitState) {
        case ___UNDEF:
            throw CommonError("exitState UNDEF");
            break;
        case ___COMMIT:
            dbh->execSimple((std::string)"COMMIT");
            break;
        case ___ROLLBACK:
            dbh->execSimple((std::string)"ROLLBACK");
            break;
        default:
            throw CommonError("exitState UNDEF");
            break;
        }
    }

};
#endif
