#ifndef PROGRESSOR_H
#define PROGRESSOR_H
#include "mutexable.h"
#include "IProgress.h"
#include <map>
#include "route_t.h"

#include "tools_mt.h"
#include "Events/System/Net/rpc/ProgressNotification.h"
#include "Events/System/Net/rpc/ProgressSetPosition.h"


struct pr_item:public Refcountable
{
    route_t route;
    pr_item() {
    }
};
class progressor: public Mutexable, public IProgress
{
    std::map<pthread_t,REF_getter<pr_item> > m_items;

    progressor(IInstance *_ifa):iInstance(_ifa) {}
    IInstance *iInstance;
public:
    void setRoute(const route_t& r)
    {
        M_LOCK(this);
        pthread_t pt=pthread_self();
        if(!m_items.count(pt))
            m_items.insert(std::make_pair(pt,new pr_item()));
        m_items.find(pt)->second->route=r;
    }
    REF_getter<pr_item> getItem()
    {
        M_LOCK(this);
        pthread_t pt=pthread_self();
        if(!m_items.count(pt))
        {
            return NULL;
        }
        return m_items.find(pt)->second;
    }

    void msg(const std::string& msg)
    {
        logErr2("%s",msg.c_str());
        REF_getter<pr_item> z=getItem();
        if(z.valid())
            iInstance->passEvent(new rpcEvent::ProgressNotification(rpc::MSG,msg,msg,poppedFrontRoute(z->route)));

    }
    void msg(const std::string& msg,const std::string& fake_msg)
    {
        logErr2("%s",msg.c_str());
        REF_getter<pr_item> z=getItem();
        if(z.valid())
            iInstance->passEvent(new rpcEvent::ProgressNotification(rpc::MSG,msg,fake_msg,poppedFrontRoute(z->route)));
    }
    void nop(const std::string& msg)
    {
        REF_getter<pr_item> z=getItem();
        if(z.valid())
            iInstance->passEvent(new rpcEvent::ProgressNotification(rpc::NOP,msg,msg,poppedFrontRoute(z->route)));
    }
    void nop(const std::string& msg,const std::string& fake_msg)
    {
        REF_getter<pr_item> z=getItem();
        if(z.valid())
            iInstance->passEvent(new rpcEvent::ProgressNotification(rpc::NOP,msg,fake_msg,poppedFrontRoute(z->route)));
    }
    void failed(const std::string& msg)
    {
        REF_getter<pr_item> z=getItem();
        if(z.valid())
            iInstance->passEvent(new rpcEvent::ProgressNotification(rpc::FAILED,msg,msg,poppedFrontRoute(z->route)));
    }
    void setPosition(int i,int max_scale_steps)
    {
        REF_getter<pr_item> z=getItem();
        if(z.valid())
            iInstance->passEvent(new rpcEvent::ProgressSetPosition(i,max_scale_steps,poppedFrontRoute(z->route)));
    }


};

#endif // PROGRESSOR_H
