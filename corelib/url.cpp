#include "url.h"


std::string Url::dump()
{
    char s[1000];
    snprintf(s,sizeof(s),
             "\
protocol %s\n\
host %s\n\
path %s\n\
params %s\n\
user %s\n\
pass %s\n\
file %s\n\
port %s\n",
             protocol.c_str(),
             host.c_str(),
             path.c_str(),
             params.c_str(),
             user.c_str(),
             pass.c_str(),
             filename.c_str(),
             port.c_str()
            );
    return s;
}
void  Url::clear()
{
    protocol="";
    host="";
    path="";
    params="";
    user="";
    pass="";
    port="";
    filename="";

};
void  Url::parse(const std::string& u)
{


    clear();
    std::string::size_type curpos=0;
    std::string hostmax;

    std::string::size_type pz;
    pz=u.find("://",curpos);
    if(pz!=std::string::npos)
    {
        protocol=u.substr(curpos,pz);
        curpos+=pz+3;
    }
    //OK
    pz=u.find("/",curpos);
    if(pz!=std::string::npos)
    {
        hostmax=u.substr(curpos,pz-curpos);
        curpos=pz;
        std::string::size_type cp=0;
        pz=hostmax.find("@",cp);
        if(pz!=std::string::npos)
        {

            std::string lp=hostmax.substr(cp,pz-cp);
            cp+=pz+1;
            pz=lp.find(":",0);
            if(pz!=std::string::npos)
            {
                user=lp.substr(0,pz);
                pass=lp.substr(pz+1,lp.size()-pz-1);
            }
            else user=lp;

        }

        bool inSkobka=false;
        pz=std::string::npos;
        for(size_t i=cp; i<hostmax.size(); i++)
        {
            if(hostmax[i]=='[') inSkobka=true;
            if(hostmax[i]==']') inSkobka=false;
            if(!inSkobka && hostmax[i]==':')
            {
                pz=i;
                break;
            }

        }
        if(pz!=std::string::npos)
        {
            host=hostmax.substr(cp,pz-cp);
            port=hostmax.substr(pz+1,hostmax.size()-pz-1);
        }
        else host=hostmax.substr(cp,hostmax.size()-cp);
        host_port=hostmax.substr(cp,hostmax.size()-cp);

        pz=u.find("?",curpos);
        if(pz!=std::string::npos)
        {
            path=u.substr(curpos,pz-curpos);
            params=u.substr(pz+1,u.size()-pz-1);
        }
        else path=u.substr(curpos,u.size()-curpos);
        std::string::size_type pz2=path.rfind("/");
        if(pz2!=std::string::npos)
        {
            filename=path.substr(pz2+1,u.size()-pz2-1);
            dirname=path.substr(0,pz2);
        }
    }
    else
    {
        hostmax=u.substr(curpos,u.size()-curpos);
        std::string::size_type cp=0;
        pz=hostmax.find("@",cp);
        if(pz!=std::string::npos)
        {

            std::string lp=hostmax.substr(cp,pz-cp);
            cp+=pz+1;
            pz=lp.find(":",0);
            if(pz!=std::string::npos)
            {
                user=lp.substr(0,pz);
                pass=lp.substr(pz+1,lp.size()-pz-1);
            }
            else user=lp;

        }

        bool inSkobka=false;
        pz=std::string::npos;
        for(size_t i=cp; i<hostmax.size(); i++)
        {
            if(hostmax[i]=='[') {
                inSkobka=true;
                continue;
            }
            if(hostmax[i]==']') {
                inSkobka=false;
                continue;
            }
            if(!inSkobka && hostmax[i]==':')
            {
                pz=i;
                break;
            }

        }

        if(pz!=std::string::npos)
        {
            host=hostmax.substr(cp,pz-cp);
            port=hostmax.substr(pz+1,hostmax.size()-pz-1);
        }
        else host=hostmax.substr(cp,hostmax.size()-cp);
        host_port=hostmax.substr(cp,hostmax.size()-cp);

    }
    if(!host.empty() && host[0]=='[' && host[host.size()-1]==']')
        host=host.substr(1,host.size()-2);


}
#ifdef ____TEST
int main()
{

    Url r("http://akakist:pgaga123@ximi.com:100/kak/ee/rr?ximi=100&iaia=3000");
    printf("%s\n",r.dump().c_str());
    r.parse("http://@ximi.com:100/kak/ee/rr?ximi=100&iaia=3000");
    printf("%s\n",r.dump().c_str());
    r.parse("http://@ximi.com/kdfg54ak/ee/rr?ximi=100&iaia=3000");
    printf("%s\n",r.dump().c_str());


    r.parse("127.0.0.1:12000");
    printf("127.0.0.1:1200 : %s\n",r.dump().c_str());

    return 0;
}
#endif

