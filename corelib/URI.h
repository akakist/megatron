#pragma once

#include <string>
#include "DB_id.h"
#include "mutexInspector.h"
struct URI
{
    friend int operator<(const URI&a, const URI&b);
    friend outBuffer& operator<< (outBuffer& b,const URI& s);
    friend inBuffer& operator>> (inBuffer& b,  URI& s);
    friend int operator==(const URI&a, const URI&b);
private:
    DB_id m_dbid;
public:
    URI(const DB_id& d):m_dbid(d) {}
    URI(const std::string& d):m_dbid(atoll(d.c_str())) {}
    URI():m_dbid(0) {}
    DB_id dbid() const
    {
        MUTEX_INSPECTOR;
        return m_dbid;
    }
    void fromDfsString(const std::string& s)
    {
        MUTEX_INSPECTOR;
        m_dbid=atoll(s.c_str());
    }
    std::string toString() const
    {
        return std::to_string(CONTAINER(m_dbid));
    }
    std::string toTmpJpgName() const
    {
        return "/tmp/----------"+std::to_string(CONTAINER(m_dbid))+".jpg";
    }
};
inline int operator<(const URI&a, const URI&b)
{
    MUTEX_INSPECTOR;
    if(a.m_dbid!=b.m_dbid) return a.m_dbid<b.m_dbid;
    return 0;
}
inline int operator==(const URI&a, const URI&b)
{
    return
        a.m_dbid==b.m_dbid;
}
inline outBuffer& operator<< (outBuffer& b,const URI& s)
{
    b<<s.m_dbid;
    return b;
}
inline inBuffer& operator>> (inBuffer& b,  URI& s)
{
    b>>s.m_dbid;
    return b;
}


