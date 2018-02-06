#include "configDB.h"
#ifdef _WIN32
#include "compat_win32.h"
#endif
#include "queryResult.h"

#include "st_FILE.h"
#ifdef QT_CORE_LIB
#include <QFile>
#include <QDir>
#endif


ConfigDB::ConfigDB(bool shared):m_shared(shared)
{

}

std::string ConfigDB::val(const std::string& key) const
{
    MUTEX_INSPECTOR;
#ifdef QT_CORE_LIB
    QString fn=fileName(QString::fromStdString(key));
    QFile file(fn);

    if(!file.open(QIODevice::ReadOnly))
    {
        logErr2("Zcannot open %s",fn.toStdString().c_str());
        return "";
    }
    file.setTextModeEnabled(false);
    QByteArray arr=file.readAll();
    std::string res=std::string(arr.data(),arr.size());
    file.close();
    return res;

#else
    if(iUtils->calcFileSize(fileName(key))!=-1)
        return iUtils->load_file_no_throw(fileName(key));
#endif
    return "";
}


msockaddr_in ConfigDB::get_tcpaddr(const std::string&name, const std::string& defv)const
{
    MUTEX_INSPECTOR;
    XTRY;
    std::string v=val(name);
    if(v.size()==0)
    {
        v=defv;
    }
    std::vector<std::string> _vv= iUtils->splitString(":",v);
    msockaddr_in bindAddr;
    std::string serviceAddr,servicePort;
    if (_vv.size()!=2) throw CommonError("error in LISTEN task line must be ADDR:PORT %s",v.c_str());
    if (_vv.size()>0)
        serviceAddr=_vv[0];
    if (_vv.size()>1) servicePort=_vv[1];
    if (serviceAddr=="INADDR_ANY")
    {
        bindAddr.setPort(atoi(servicePort.c_str()));
    }
    else
    {
        bindAddr.init(serviceAddr.c_str(),atoi(servicePort.c_str()));
    }
    return bindAddr;
    XPASS;

}

std::string ConfigDB::get_string(const std::string& name, const std::string& defv) const
{
    MUTEX_INSPECTOR;
    std::string v=val(name);
    if(v.size()==0)
    {
        v=defv;
    }
    return v;
}
uint64_t ConfigDB::get_uint64_t(const std::string&name, const uint64_t& defv)const
{
    MUTEX_INSPECTOR;
    std::string vv=val(name);
    if(vv.size()==0)
    {
        return defv;
    }
#ifdef _MSC_VER
    return atol(vv.c_str());
#else
    return atoll(vv.c_str());
#endif
}
#ifdef QT_CORE_LIB
QString ConfigDB::fileName(const QString &k) const
{
    QString baseDir=QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    QDir dir(baseDir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    if(m_shared)
        baseDir+="/config."+k;
    else
        baseDir+= "/config."+QString::fromStdString(iUtils->app_name())+"."+k;

    return baseDir;

}
//#endif
#else
std::string ConfigDB::fileName(const std::string &k) const
{
    std::string baseDir;

    baseDir=iUtils->gCacheDir();

    std::string pn;
    if(m_shared)
        pn=baseDir+"/config."+k;
    else
        pn=baseDir+ "/config."+iUtils->app_name()+"."+k;
    return pn;

}
#endif

void ConfigDB::set(const std::string&k, const std::string&v)
{
    MUTEX_INSPECTOR;
#ifdef QT_CORE_LIB
    QString fn=fileName(QString::fromStdString(k));
    if(v.size())
    {
        QFile file(fn);

        if(!file.open(QIODevice::WriteOnly))
        {
            logErr2("cannot open %s",fn.toStdString().c_str());
            return ;
        }
        file.setTextModeEnabled(false);
        file.write(v.data(),v.size());
        file.close();
    }
    else
    {
        QFile::remove(fn);
    }
#else
    std::string pn=fileName(k);
    if(v.size())
    {
        st_FILE f(pn.c_str(),"wb");
        fwrite(v.data(),1,v.size(),f.f);
    }
    else
    {
        unlink(pn.c_str());
    }

#endif
}
