#ifndef _________________DBH__sqlite___HH___
#define _________________DBH__sqlite___HH___
#include <string>
#include <vector>
#include <deque>
#include "sqlite3.h"
#ifdef _WIN32
#include <time.h>
#endif
#include "DBH.h"
#include "unknown.h"
#include "listenerSimple.h"

#include "logging.h"
#include "Events/System/Run/startService.h"

#define INACTIVE_CONNECTION_DROP_TIMEOUT_SEC 60

struct sqliteConf
{
    int max_connections;
    std::string database;
};

class DBH_sqlite:public DBH
{
public:
    //static Mutex lock;
    void execSimple(const QUERY &);
    REF_getter<QueryResult> exec(const QUERY &);
    REF_getter<QueryResult> exec(const std::string &query);

    std::string escape(const std::string&);

    DBH_sqlite();
    virtual ~DBH_sqlite();
    time_t getLastQueryTime();

    void execSimple(const std::string &);


    void makeConnection(const sqliteConf& conf);
private:
    bool bIsValid();
    sqlite3 *dbh;
    bool bIsLost;

    time_t m_lastQueryTime;

protected:
};
class DBH_source_sqlite:public UnknownBase, public ListenerSimple, public DBH_source, public Mutexable
{
public:
    static UnknownBase* construct(const SERVICE_id& id, const std::string&  nm,IInstance*);
    REF_getter<DBH> get();
    void unget(const REF_getter<DBH>&);
    DBH_source_sqlite(const SERVICE_id& _id, const std::string& nm,IInstance*);

    int max_connections;

    std::deque<REF_getter<DBH> > container_MX;
    //bool init(IConfigObj*);
    //bool deinit();
    bool handleEvent(const REF_getter<Event::Base>&)
    {
        return true;
    }

    bool on_startService(const systemEvent::startService*)
    {
        return true;
    }
    sqliteConf cfg;
};
#endif
