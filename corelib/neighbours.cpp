#include "neighbours.h"

#include "configDB.h"
#define NEIGBOURS_DB_NAME "neighboursz"

_neighbours::_neighbours() {
    ConfigDB_private c;
    std::string s = c.get_string("_neighbours", "");
    if (s.size())
    {
        inBuffer in(s);
        in>>sas;
    }

}

void _neighbours::remove(const msockaddr_in& sa)
{
    sas.erase(sa);
    outBuffer o;
    o<<sas;
    ConfigDB_private c;
    c.set("_neighbours",o.asString()->asString());

}

std::vector<msockaddr_in> _neighbours::getAllAndClear()
{
    std::vector<msockaddr_in> ret;
    for(auto z: sas)
        ret.push_back(z);
    sas.clear();
    outBuffer o;
    o<<sas;
    ConfigDB_private c;
    c.set("_neighbours",o.asString()->asString());
    return ret;
}
void _neighbours::add(const msockaddr_in &sa, int pingTime)
{
    sas.insert(sa);
    outBuffer o;
    o<<sas;
    ConfigDB_private c;
    c.set("_neighbours",o.asString()->asString());

}
