#ifndef __________________CONFIG_DB__HH____
#define __________________CONFIG_DB__HH____
#include <map>
#include <deque>
#include <string>
#include <set>
#include <stdio.h>
#include "mutexable.h"
#include "msockaddr_in.h"
#ifdef QT_CORE_LIB
#include <QString>
#include <QStandardPaths>

#endif
class ConfigDB_shared;
class ConfigDB_private;
class ConfigDB
{
    friend class ConfigDB_private;
    friend class ConfigDB_shared;

    std::string val(const std::string& key) const;
    ConfigDB(bool shared);
#if defined(QT_CORE_LIB)
    QString fileName(const QString &k) const;
#else
    std::string fileName(const std::string &k) const;
#endif

public:

    bool m_shared;
    void set(const std::string& key, const std::string&val);
    msockaddr_in get_tcpaddr(const std::string&name, const std::string& defv)const;
    uint64_t get_uint64_t(const std::string&name, const uint64_t& defv)const;
    std::string get_string(const std::string&name, const std::string& defv)const;
};
class ConfigDB_shared: public ConfigDB
{
public:
    ConfigDB_shared():ConfigDB(true) {}
};
class ConfigDB_private: public ConfigDB
{
public:
    ConfigDB_private():ConfigDB(false) {}
};

//#endif

#endif
