#ifndef _______________SOCKETSTATS_H
#define _______________SOCKETSTATS_H
#include "REF.h"
#include "IUtils.h"
#include "epoll_socket_info.h"
namespace SocketIO
{

    struct StatElement: public Refcountable, public Mutexable
    {
        enum { enHistoryTimeout=20};
        time_t lastAccess;
        int64_t in;
        int64_t out;
        std::map<time_t,int64_t> inSpeed;
        std::map<time_t,int64_t> outSpeed;
        StatElement():lastAccess(time(NULL)),in(0),out(0) {}
        void addIn(int64_t val)
        {
            M_LOCK(this);
            in+=val;
            lastAccess=time(NULL);
            while(inSpeed.size() && inSpeed.begin()->first<lastAccess-enHistoryTimeout)
            {
                inSpeed.erase(inSpeed.begin());
            }
            auto i=inSpeed.find(lastAccess);
            if(i==inSpeed.end()) inSpeed[lastAccess]=val;
            else inSpeed[lastAccess]+=val;
        }
        void addOut(int64_t val)
        {
            M_LOCK(this);
            out+=val;
            lastAccess=time(NULL);
            while(outSpeed.size() && outSpeed.begin()->first<lastAccess-enHistoryTimeout)
            {
                outSpeed.erase(outSpeed.begin());
            }
            auto i=outSpeed.find(lastAccess);
            if(i==outSpeed.end()) outSpeed[lastAccess]=val;
            else outSpeed[lastAccess]+=val;
        }
        std::string dump()
        {
            std::string o;
            o+="IN:"+std::to_string(in)+"<br>";
            o+="OUT:"+std::to_string(out)+"<br>";
            std::vector<std::string> spin,spout;
            time_t t=time(NULL);
            for(time_t I=t-enHistoryTimeout; I!=t; I++)
            {
                {
                    auto i=inSpeed.find(I);
                    if(i==inSpeed.end())spin.push_back("0");
                    else spin.push_back(std::to_string(i->second));
                }
                {
                    auto i=outSpeed.find(I);
                    if(i==outSpeed.end())spout.push_back("0");
                    else spout.push_back(std::to_string(i->second));
                }
            }
            o+="InSpeed: "+iUtils->join(",",spin)+"<br>";
            o+="outSpeed: "+iUtils->join(",",spout)+"<br>";
            return o;
        }
    };
    class __stats: public Mutexable
    {

        std::map<std::pair<std::string,std::string>, REF_getter<StatElement> > container;
    public:
        __stats():totalStats(new StatElement), accepted(0) {}
        REF_getter<StatElement> totalStats;
        int64_t accepted;
        std::map<std::pair<std::string,std::string>, REF_getter<StatElement> > getContainer()
        {
            M_LOCK(this);
            return container;
        }
        REF_getter<StatElement> get(const REF_getter<epoll_socket_info>& esi)
        {
            REF_getter<StatElement> S(NULL);
            {
                M_LOCK(this);
                auto i=container.find(std::make_pair(esi->local_name.getStringAddr(),esi->remote_name.getStringAddr()));

                if(i!=container.end()) S=i->second;
                else
                {
                    S=new StatElement;
                    container.insert(std::make_pair(std::make_pair(esi->local_name.getStringAddr(),esi->remote_name.getStringAddr()),S));
                }


            }
            if(!S.valid()) throw CommonError("!S.valid");
            return S;

        }
        void addIn(const REF_getter<epoll_socket_info>& esi,int64_t val)
        {
            get(esi)->addIn(val);
            totalStats->addIn(val);
        }
        void addOut(const REF_getter<epoll_socket_info>& esi,int64_t val)
        {
            get(esi)->addOut(val);
            totalStats->addOut(val);
        }
        void incAccepted()
        {
            accepted++;
        }
        std::string dump()
        {
            std::string o;
            o+="accepted:"+std::to_string(accepted)+"<p>";
            o+="<h1>Total</h1><br>"+totalStats->dump()+"<p>";
            auto c=getContainer();
            for(auto& i:c)
            {
                o+=(std::string)"<b>"+i.first.first+":"+i.first.second+"</b><br>"+i.second->dump();
            }

            return o;
        }
    };


}
#endif // SOCKETSTATS_H
