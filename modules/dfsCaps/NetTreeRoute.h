#ifndef ________________NETTREEROUTE_H
#define ________________NETTREEROUTE_H
#include <deque>
#include "commonError.h"
#include "mutexInspector.h"
#include "NetRec.h"
#include "_bitStream.h"
struct NetTreeRoute
{
    std::deque<int> container;
    void pop_front()
    {
        container.pop_front();
    }
    void push_back(int n)
    {
        container.push_back(n);
    }
    void push_front(int n)
    {
        container.push_front(n);
    }
    int first()
    {
        if(container.size()==0) throw CommonError("if(container.size()==0)"+_DMI());
        return *container.begin();
    }
    size_t size()
    {
        return container.size();
    }
    std::string dump() const
    {
        std::vector<std::string> v;
        for(size_t i=0; i<container.size(); i++)
        {
            v.push_back(iUtils->toString(container[i]));
        }
        return iUtils->join(":",v);
    }

};
struct NetTreeRouteGenerator
{
    bool m_loaded;
    NetTreeRouteGenerator():m_loaded(false) {}
    NetTreeRoute calcTreeRoute(const int32_t &ip);
    static int pack(outBitStream& o,int val,int maxval);
    std::map<int32_t,NetTreeRoute > calcTreeRouteCache;
    std::vector<NetRec> m_netArray;

    void load(const std::string& pn_netBlocks);
    bool isLoaded()
    {
        return m_loaded;
    }
};


#endif // NETTREEROUTE_H
