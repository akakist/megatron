#include "neighbours.h"

#include "configDB.h"
using namespace dfsReferrer;
#define NEIGBOURS_DB_NAME "neighbours"

_neighbours::_neighbours()
{

    Sqlite3Wrapper dbh(NEIGBOURS_DB_NAME);
    dbh.execSimple((QUERY)"BEGIN");

    dbh.execSimple((std::string)"CREATE TABLE  IF NOT EXISTS neighbours ("
                   "ip INTEGER not null,"
                   "port INTEGER not null,"
                   "ping_time INTEGER not null,"
                   "CONSTRAINT name_unique UNIQUE (ip, port)"
                   ")");
    dbh.execSimple((QUERY)"COMMIT");

}

void _neighbours::remove(const msockaddr_in& sa)
{
    Sqlite3Wrapper dbh(NEIGBOURS_DB_NAME);
    dbh.execSimple((QUERY)"BEGIN");
    dbh.execSimple((QUERY)"delete from neighbours where ip=? and port=?"<<sa.inaddr()<<sa.port());
    dbh.execSimple((QUERY)"COMMIT");

}

std::vector<msockaddr_in> _neighbours::getAllAndClear()
{
    Sqlite3Wrapper dbh(NEIGBOURS_DB_NAME);
    REF_getter<QueryResult> r=dbh.exec((std::string)"select ip,port from neighbours order by ping_time");

    std::vector<msockaddr_in> ret;
    for(auto& z: r->values)
    {
        if(z.size()!=2)
            throw CommonError("if(z.size()!=2)");
        msockaddr_in sa;
        sa.setInaddr(atoi(z[0].c_str()));
        sa.setPort(atoi(z[1].c_str()));
        ret.push_back(sa);
    }
    {
        dbh.execSimple((QUERY)"BEGIN");
        dbh.execSimple((QUERY)"delete from neighbours");
        dbh.execSimple((QUERY)"COMMIT");

    }
    return ret;
}
void _neighbours::add(const msockaddr_in &sa, int pingTime)
{
    Sqlite3Wrapper dbh(NEIGBOURS_DB_NAME);
    dbh.execSimple((QUERY)"BEGIN");
    dbh.execSimple((QUERY)"replace into neighbours (ip,port,ping_time) values(?,?,?)"<<sa.inaddr()<<sa.port()<<pingTime);
    dbh.execSimple((QUERY)"COMMIT");


}
