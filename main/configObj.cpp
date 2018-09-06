#include "configObj.h"
#include "IInstance.h"
#ifdef _WIN32
#include "compat_win32.h"
#endif
#include "st_FILE.h"

void ConfigObj::appendLine(const std::string& key, const bool& val, const std::string& comment)
{
    if(val) appendLine(key,(std::string)"true", comment);
    else appendLine(key,(std::string)"false",comment);
}
void ConfigObj::appendLine(const std::string& key, const real& val, const std::string& comment)
{
    appendLine(key,iUtils->toString(val),comment);
}
void ConfigObj::appendLine(const std::string& key, const int64_t& val, const std::string& comment)
{
    appendLine(key,iUtils->toString(val),comment);

}
void ConfigObj::appendLine(const std::string& key, const uint64_t& val, const std::string& comment)
{
    appendLine(key,iUtils->toString(val),comment);

}

void ConfigObj::appendLine(const std::string& key, const std::string& val, const std::string& comment)
{

#if !defined __MOBILE__


    std::string::size_type pz=m_filename.rfind('/');
    if(pz!=std::string::npos)
    {
        std::string path=m_filename.substr(0,pz);
        DBG(logErr2("check path %s",path.c_str()));
        iUtils->checkPath(path);
    }
    st_FILE f(m_filename,"a+b");


    if(comment.size())
    {
        fprintf(f.f,"\n# %s\n",comment.c_str());
    }
    fprintf(f.f,"%s=%s\n",key.c_str(),val.c_str());

#endif
}

void ConfigObj::printUnusedItems()
{

    M_LOCK(this);
    for(std::map<std::string,std::pair<std::string, bool> > ::iterator i=m_container.begin(); i!=m_container.end(); i++)
    {
        if(!i->second.second)
        {
            DBG(logErr2("config: unused item %s -> %s",i->first.c_str(),i->second.first.c_str()));
        }
    }
}


void ConfigObj::load_from_file()
{
    M_LOCK(this);
    std::map<std::string,std::string> m;
    try {
        m=iUtils->loadStringMapFromFile(m_filename);
    }
    catch(...)
    {
        logErr2("exception on load file %s",m_filename.c_str());
    }
    for(std::map<std::string,std::string>::iterator i=m.begin(); i!=m.end(); i++)
    {
        m_container[i->first]=std::make_pair(i->second,false);

    }
}
void ConfigObj::load_from_buffer(const std::string& buf)
{
    M_LOCK(this);
    std::map<std::string,std::string> m=iUtils->loadStringMapFromBuffer(buf,"\r\n");
    for(std::map<std::string,std::string>::iterator i=m.begin(); i!=m.end(); i++)
    {
        m_container[i->first]=std::make_pair(i->second,false);

    }
}

void ConfigObj::m_initFromLine(const std::string& buf)
{
#if !defined __MOBILE__

    M_LOCK(this);
    std::map<std::string,std::string> m=iUtils->loadStringMapFromBuffer(buf,",");
    for(std::map<std::string,std::string>::iterator i=m.begin(); i!=m.end(); i++)
    {
        m_container[i->first]=std::make_pair(i->second,false);

    }
#endif
}
std::set<msockaddr_in>ConfigObj::get_tcpaddr(const std::string&_name, const std::string& defv, const std::string& comment)
{
    std::set<msockaddr_in>ret;
    XTRY;
    M_LOCK(this);
    std::string val;
    std::string name=m_getPath(_name);
    std::map<std::string,std::pair<std::string, bool> > ::iterator i=m_container.find(name);
    if (i!=m_container.end())
    {
        val=i->second.first;
        i->second.second=true;
    }
    else
    {
//#ifndef __MOBILE__
        logErr2("%s: skipped param, using deflt %s=%s",m_filename.c_str(),name.c_str(),defv.c_str());
//#endif
        val=defv;
        appendLine(name,defv,comment);
    }
    std::deque<std::string> vz=iUtils->splitStringDQ(";, ",val.c_str());

    while(vz.size())
    {
        msockaddr_in m_bindAddr;
        std::string v=*vz.begin();
        vz.pop_front();
        if(iUtils->strupper(v)=="NONE")
        {
            //ret.insert(std::make_pair(m_bindAddr,false));
            continue;
        }
        std::vector<std::string> _vv= iUtils->splitString(":",v);
        std::string serviceAddr,servicePort;
        if (_vv.size()!=2) throw CommonError("error in LISTEN task line must be ADDR:PORT %s",v.c_str());
        if (_vv.size()>0)
            serviceAddr=_vv[0];
        if (_vv.size()>1) servicePort=_vv[1];
        if (serviceAddr=="INADDR_ANY")
        {
            m_bindAddr.setPort(atoi(servicePort.c_str()));
        }
        else
        {
            m_bindAddr.init(serviceAddr.c_str(),atoi(servicePort.c_str()));
        }
        ret.insert(m_bindAddr);
        continue;
    }

    XPASS;

    return ret;
}
std::set<msockaddr_in>ConfigObj::get_tcpaddr2(const std::string&_name, const std::string& defv, const std::string& comment)
{
    std::set<msockaddr_in>ret;
    XTRY;
    M_LOCK(this);
    std::string val;
    std::string name=m_getPath2(_name);
    std::map<std::string,std::pair<std::string, bool> > ::iterator i=m_container.find(name);
    if (i!=m_container.end())
    {
        val=i->second.first;
        i->second.second=true;
    }
    else
    {
//#ifndef __MOBILE__
        logErr2("%s: skipped param, using deflt %s=%s",m_filename.c_str(),name.c_str(),defv.c_str());
//#endif
        val=defv;
        appendLine(name,defv,comment);
    }
    std::deque<std::string> vz=iUtils->splitStringDQ(";, ",val.c_str());

    while(vz.size())
    {
        msockaddr_in m_bindAddr;
        std::string v=*vz.begin();
        vz.pop_front();
        if(iUtils->strupper(v)=="NONE")
        {
            //ret.insert(std::make_pair(m_bindAddr,false));
            continue;
        }
        std::vector<std::string> _vv= iUtils->splitString(":",v);
        std::string serviceAddr,servicePort;
        if (_vv.size()!=2) throw CommonError("error in LISTEN task line must be ADDR:PORT %s",v.c_str());
        if (_vv.size()>0)
            serviceAddr=_vv[0];
        if (_vv.size()>1) servicePort=_vv[1];
        if (serviceAddr=="INADDR_ANY")
        {
            m_bindAddr.setPort(atoi(servicePort.c_str()));
        }
        else
        {
            m_bindAddr.init(serviceAddr.c_str(),atoi(servicePort.c_str()));
        }
        ret.insert(m_bindAddr);
        continue;
    }

    XPASS;

    return ret;
}

std::string ConfigObj::get_string(const std::string& _name, const std::string& defv, const std::string& comment)
{
    M_LOCK(this);
    std::string v;
    std::string name=m_getPath(_name);
    std::map<std::string,std::pair<std::string, bool> > ::iterator i=m_container.find(name);
    if (i!=m_container.end())
    {
        v=i->second.first;
        i->second.second=true;

    }
    else
    {
//#ifndef __MOBILE__
        logErr2("%s: skipped param, using deflt %s=%s",m_filename.c_str(),name.c_str(),defv.c_str());
//#endif
        v=defv;
        appendLine(name,defv,comment);
    }
    return v;
}
std::string ConfigObj::get_string2(const std::string& _name, const std::string& defv, const std::string& comment)
{
    M_LOCK(this);
    std::string v;
    std::string name=m_getPath2(_name);
    std::map<std::string,std::pair<std::string, bool> > ::iterator i=m_container.find(name);
    if (i!=m_container.end())
    {
        v=i->second.first;
        i->second.second=true;

    }
    else
    {
//#ifndef __MOBILE__
        logErr2("%s: skipped param, using deflt %s=%s",m_filename.c_str(),name.c_str(),defv.c_str());
//#endif
        v=defv;
        appendLine(name,defv,comment);
    }
    return v;
}
real ConfigObj::get_real(const std::string&_name, const real& defv, const std::string& comment)
{
    M_LOCK(this);
    real v;
    std::string name=m_getPath(_name);
    std::map<std::string,std::pair<std::string, bool> > ::iterator i=m_container.find(name);
    if (i!=m_container.end())
    {
        v=strtod(i->second.first.c_str(),NULL);
        i->second.second=true;

    }
    else
    {
//#ifndef __MOBILE__
        logErr2("%s: skipped param, using deflt %s=%f",m_filename.c_str(),name.c_str(),float(defv));
//#endif
        v=defv;
        appendLine(name,defv,comment);
    }
    return v;

}
real ConfigObj::get_real2(const std::string&_name, const real &defv, const std::string& comment)
{
    M_LOCK(this);
    real v;
    std::string name=m_getPath2(_name);
    std::map<std::string,std::pair<std::string, bool> > ::iterator i=m_container.find(name);
    if (i!=m_container.end())
    {
        v=strtod(i->second.first.c_str(),NULL);
        i->second.second=true;

    }
    else
    {
//#ifndef __MOBILE__
        logErr2("%s: skipped param, using deflt %s=%f",m_filename.c_str(),name.c_str(),defv);
//#endif
        v=defv;
        appendLine(name,defv,comment);
    }
    return v;

}

std::set<std::string> ConfigObj::get_stringset(const std::string& _name, const std::string& defv, const std::string& comment)
{
    std::set<std::string> v;
    std::string name=m_getPath(_name);

    M_LOCK(this);
    std::map<std::string,std::pair<std::string, bool> > ::iterator i=m_container.find(name);
    if (i!=m_container.end())
    {
        std::vector<std::string> vv=iUtils->splitString(",|;",i->second.first);
        i->second.second=true;

        for (size_t i=0; i<vv.size(); i++)
        {
            v.insert(vv[i]);
        }

    }
    else
    {
//#ifndef __MOBILE__
        logErr2("%s: skipped param, using deflt %s=%s",m_filename.c_str(),name.c_str(),defv.c_str());
//#endif
        appendLine(name,defv,comment);
        std::vector<std::string> vv=iUtils->splitString(",|",defv);
        for (size_t i=0; i<vv.size(); i++)
        {
            v.insert(vv[i]);
        }
    }
    return v;

}

std::set<std::string> ConfigObj::get_stringset2(const std::string& _name, const std::string& defv, const std::string& comment)
{
    std::set<std::string> v;
    M_LOCK(this);
    std::string name=m_getPath2(_name);

    std::map<std::string,std::pair<std::string, bool> > ::iterator i=m_container.find(name);
    if (i!=m_container.end())
    {
        std::vector<std::string> vv=iUtils->splitString(",|;",i->second.first);
        i->second.second=true;

        for (size_t i=0; i<vv.size(); i++)
        {
            v.insert(vv[i]);
        }

    }
    else
    {
//#ifndef __MOBILE__
        logErr2("%s: skipped param, using deflt %s=%s",m_filename.c_str(),name.c_str(),defv.c_str());
//#endif
        appendLine(name,defv,comment);
        std::vector<std::string> vv=iUtils->splitString(",|",defv);
        for (size_t i=0; i<vv.size(); i++)
        {
            v.insert(vv[i]);
        }
    }
    return v;

}

bool ConfigObj::get_bool(const std::string& _name, bool defv, const std::string& comment)
{
    bool v;
    M_LOCK(this);
    std::string name=m_getPath(_name);
    std::map<std::string,std::pair<std::string, bool> > ::iterator i=m_container.find(name);
    if (i!=m_container.end())
    {
        v=i->second.first=="true";
        i->second.second=true;

    }
    else
    {
//#ifndef __MOBILE__
        logErr2("%s: skipped param, using deflt %s=%s",m_filename.c_str(),name.c_str(),defv?"true":"false");
//#endif
        v=defv;
        appendLine(name,defv,comment);
    }
    return v;
}
bool ConfigObj::get_bool2(const std::string& _name, bool defv, const std::string& comment)
{
    bool v;
    M_LOCK(this);
    std::string name=m_getPath2(_name);
    std::map<std::string,std::pair<std::string, bool> > ::iterator i=m_container.find(name);
    if (i!=m_container.end())
    {
        v=i->second.first=="true";
        i->second.second=true;

    }
    else
    {
//#ifndef __MOBILE__
        logErr2("%s: skipped param, using deflt %s=%s",m_filename.c_str(),name.c_str(),defv?"true":"false");
//#endif
        v=defv;
        appendLine(name,defv,comment);
    }
    return v;
}
void ConfigObj::m_addItem(const std::string& _name, const std::string& v)
{
    std::string name=m_getPath(_name);
    m_container[name]=std::make_pair(v,false);

}

int64_t ConfigObj::get_int64_t(const std::string&_name, int64_t defv, const std::string& comment)
{
    int64_t v;
    M_LOCK(this);
    std::string name=m_getPath(_name);
    std::map<std::string,std::pair<std::string, bool> > ::iterator i=m_container.find(name);
    if (i!=m_container.end())
    {
#ifdef _MSC_VER
        v=atol(i->second.first.c_str());
#else
        v=atol(i->second.first.c_str());
#endif
        i->second.second=true;

    }
    else
    {
//#ifndef __MOBILE__
        logErr2("%s: skipped param, using deflt %s=%s",m_filename.c_str(),name.c_str(),iUtils->toString(defv).c_str());
//#endif
        v=defv;
        appendLine(name,defv,comment);
    }
    return v;
}
int64_t ConfigObj::get_int64_t2(const std::string&_name, int64_t defv, const std::string& comment)
{
    int64_t v;
    M_LOCK(this);
    std::string name=m_getPath2(_name);
    std::map<std::string,std::pair<std::string, bool> > ::iterator i=m_container.find(name);
    if (i!=m_container.end())
    {
#ifdef _MSC_VER
        v=atol(i->second.first.c_str());
#else
        v=atol(i->second.first.c_str());
#endif
        i->second.second=true;

    }
    else
    {
//#ifndef __MOBILE__
        logErr2("%s: skipped param, using deflt %s=%s",m_filename.c_str(),name.c_str(),iUtils->toString(defv).c_str());
//#endif
        v=defv;
        appendLine(name,defv,comment);
    }
    return v;
}

uint64_t ConfigObj::get_uint64_t(const std::string&_name, uint64_t defv, const std::string& comment)
{
    uint64_t v;
    M_LOCK(this);
    std::string name=m_getPath(_name);
    std::map<std::string,std::pair<std::string, bool> > ::iterator i=m_container.find(name);
    if (i!=m_container.end())
    {
#ifdef _MSC_VER
        v=atol(i->second.first.c_str());
#else
        v=atol(i->second.first.c_str());
#endif
        i->second.second=true;
    }
    else
    {
//#ifndef __MOBILE__
        logErr2("%s: skipped param, using deflt %s=%s",m_filename.c_str(),name.c_str(),iUtils->toString(defv).c_str());
//#endif
        v=defv;
        appendLine(name,defv,comment);
    }
    return v;
}

std::string ConfigObj::m_getPath(const std::string&name) const
{
    std::deque<std::string> d;
    std::map<pthread_t,std::deque<std::string> >::const_iterator i=m_prefixes.find(pthread_self());
    if(i!=m_prefixes.end())d=i->second;
    d.push_back(name);
    return iUtils->join(".",d);
}
std::string ConfigObj::m_getPath2(const std::string&name) const
{
    return name;
}



void ConfigObj::push_prefix(const std::string& p)
{
    M_LOCK(this);
    m_prefixes[pthread_self()].push_back(p);
}
void ConfigObj::pop_prefix()
{
    M_LOCK(this);
    m_prefixes[pthread_self()].pop_back();

}
