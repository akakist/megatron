#ifndef ____________NETTREENODE_H
#define ____________NETTREENODE_H
#include <sys/types.h>
#include <stdint.h>
#include "msockaddr_in.h"
#include "json/json.h"
#include <map>
struct NetTreeNode
{
    std::map<int,NetTreeNode> children;
    std::map<msockaddr_in,time_t> hosts;
    Json::Value jdump()
    {
        Json::Value z;
        for(auto i:hosts)
        {
            Json::Value j;
            j["msockaddr_in"]=i.first.dump();
            j["time_t"]=iUtils->toString((int)i.second);
            z["hosts"].append(j);
        }
        for(auto i:children)
        {
            Json::Value j;
            j["id"]=i.first;
            j["TreeNode"]=i.second.jdump();
            z["children"].append(j);
        }
        return z;
    }
    void addIpToTree(const msockaddr_in &sa,const NetTreeRoute& r)
    {
        //NetTreeRoute r=NetTreeRouteGenerator::calcTreeRoute(sa.inaddr());
        NetTreeNode* p=this;
        for(size_t i=0; i<r.container.size(); i++)
        {
            p=&p->children[r.container[i]];
            p->hosts[sa]=time(NULL);
        }
    }
    std::set<msockaddr_in> getIpListFromTree(const int32_t &ip, const NetTreeRoute& r)
    {
        std::set<msockaddr_in> ret;
        std::vector<NetTreeNode*> collection;
        //NetTreeRoute r=NetTreeRoute::calcTreeRoute(ip);

        msockaddr_in sa;
        sa.setInaddr(ip);
        NetTreeNode* p=this;
        for(size_t i=0; i<r.container.size(); i++)
        {
            p=&p->children[r.container[i]];
            collection.push_back(p);
        }
        for(auto i=collection.rbegin(); i!=collection.rend(); i++)
        {
            NetTreeNode* c=*i;
            for(int K=0; K<c->hosts.size(); K++)
            {
                auto j = std::next(std::begin(c->hosts), rand()%c->hosts.size());
                if(time(NULL)-j->second<3600)
                {
                    ret.insert(j->first);
                }
                if(ret.size()>30)
                    return ret;
            }
        }
        return ret;
    }


};

#endif // NETTREENODE_H
