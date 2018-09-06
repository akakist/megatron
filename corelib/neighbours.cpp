#include "neighbours.h"

#include "configDB.h"
//using namespace dfsReferrer;
#define NEIGBOURS_DB_NAME "neighboursz"

_neighbours::_neighbours() {
//#ifdef __ANDROID__
    ConfigDB_private c;
    std::string s = c.get_string("_neighbours", "");
    if (s.size())
    {
        inBuffer in(s);
        in>>sas;
    }
//#else
//    Sqlite3Wrapper dbh(NEIGBOURS_DB_NAME);
//    dbh.execSimple((QUERY)"BEGIN");

//    dbh.execSimple((QUERY)"CREATE TABLE  IF NOT EXISTS ? ("
//                   "ip char[100] not null,"
//                   "port INTEGER not null,"
//                   "ping_time INTEGER not null,"
//                   "CONSTRAINT name_unique UNIQUE (ip, port)"
//                   ")"
//                   <<NEIGBOURS_DB_NAME);
//    dbh.execSimple((QUERY)"COMMIT");
//#endif

}

void _neighbours::remove(const msockaddr_in& sa)
{
//#ifdef __ANDROID__
    sas.erase(sa);
    outBuffer o;
    o<<sas;
    ConfigDB_private c;
    c.set("_neighbours",o.asString()->asString());
//#else
//    Sqlite3Wrapper dbh(NEIGBOURS_DB_NAME);
//    dbh.execSimple((QUERY)"BEGIN");
//    dbh.execSimple((QUERY)"delete from ? where ip=x'?' and port=?"
//                   <<NEIGBOURS_DB_NAME
//                   <<iUtils->bin2hex(sa.getStringAddr())
//                   <<sa.port());
//    dbh.execSimple((QUERY)"COMMIT");
//#endif

}

std::vector<msockaddr_in> _neighbours::getAllAndClear()
{
    std::vector<msockaddr_in> ret;
//#ifdef __ANDROID__
    for(auto z: sas)
        ret.push_back(z);
    sas.clear();
    outBuffer o;
    o<<sas;
    ConfigDB_private c;
    c.set("_neighbours",o.asString()->asString());
//#else
//    Sqlite3Wrapper dbh(NEIGBOURS_DB_NAME);
//    REF_getter<QueryResult> r=dbh.exec((QUERY)"select ip,port from ? order by ping_time"<<NEIGBOURS_DB_NAME);

//    for(auto& z: r->values)
//    {
//        if(z.size()!=2)
//            throw CommonError("if(z.size()!=2)");
//        msockaddr_in sa;
//        sa.init(z[0].c_str(),atoi(z[1].c_str()));
////        sa.setInaddr(atoi(z[0].c_str()));
////        sa.setPort(atoi(z[1].c_str()));
//        ret.push_back(sa);
//    }
//    {
//        dbh.execSimple((QUERY)"BEGIN");
//        dbh.execSimple((QUERY)"delete from ?"<<NEIGBOURS_DB_NAME);
//        dbh.execSimple((QUERY)"COMMIT");

//    }
//#endif
    return ret;
}
void _neighbours::add(const msockaddr_in &sa, int pingTime)
{
//#ifdef __ANDROID__
    sas.insert(sa);
    outBuffer o;
    o<<sas;
    ConfigDB_private c;
    c.set("_neighbours",o.asString()->asString());
//#else
//    Sqlite3Wrapper dbh(NEIGBOURS_DB_NAME);
//    dbh.execSimple((QUERY)"BEGIN");
//    dbh.execSimple((QUERY)"replace into ? (ip,port,ping_time) values(x'?',?,?)"
//                   <<NEIGBOURS_DB_NAME
//                   <<iUtils->bin2hex(sa.getStringAddr())
//                   <<sa.port()
//                   <<pingTime);
//    dbh.execSimple((QUERY)"COMMIT");
//#endif

}
