#ifndef _____________INCONFIGOBJ____H
#define _____________INCONFIGOBJ____H
#include <set>
#include <string>
#include "msockaddr_in.h"
#include "mtimespec.h"
#include "Real.h"

/**        Интерфейс ядра для работы с конфигом.
*/

class IConfigObj
{
public:
    /// получить tcp address. Формат: msockaddr addr=cf->get_tcpaddr("addrParamName","192.168.0.1:2000")
    /// \param name Имя в конфиге
    /// \param defv default value
    /// \brief Ex: msockaddr addr=cf->get_tcpaddr("addrParamName","192.168.0.1:2000")
    virtual std::set<msockaddr_in> get_tcpaddr(const std::string&name, const std::string& defv, const std::string& comment)=0;
    virtual std::set<msockaddr_in> get_tcpaddr2(const std::string&name, const std::string& defv, const std::string& comment)=0;

    /// получить строку. Формат: msockaddr addr=cf->get_tcpaddr("addrParamName","192.168.0.1:2000")
    /// \param name Имя в конфиге
    /// \param defv default value
    virtual std::string get_string(const std::string&name, const std::string& defv, const std::string& comment)=0;
    virtual std::string get_string2(const std::string&name, const std::string& defv, const std::string& comment)=0;

    /// получить int64_t. Формат: long n=cf->get_long("ParamName",10)
    /// \param name Имя в конфиге
    /// \param defv default value
    virtual int64_t get_int64_t(const std::string&name, int64_t defv, const std::string& comment)=0;
    virtual int64_t get_int64_t2(const std::string&name, int64_t defv, const std::string& comment)=0;


    virtual real get_real(const std::string&name, const real& defv, const std::string& comment)=0;
    virtual real get_real2(const std::string&name, const real& defv, const std::string& comment)=0;

    /// получить bool. Формат: bool n=cf->get_bool("ParamName",true); В конфиге прописывается "true" | "false"
    /// \param name Имя в конфиге
    /// \param defv default value
    virtual bool get_bool(const std::string&name, bool defv, const std::string& comment)=0;
    virtual bool get_bool2(const std::string&name, bool defv, const std::string& comment)=0;

    /// получить std::set<std::string>. Формат: std::set<std::string> n=cf->get_stringset("ParamName","a,b,c");
    /// \param name Имя в конфиге
    /// \param defv default value
    virtual std::set<std::string> get_stringset(const std::string&name, const std::string& defv, const std::string& comment)=0;
    virtual std::set<std::string> get_stringset2(const std::string&name, const std::string& defv, const std::string& comment)=0;

    //virtual int64_t get_flaggable_int(const std::string&name, const std::string& defv, const std::string& flagset,const std::string& comment)=0;

    virtual void printUnusedItems()=0;

    virtual void push_prefix(const std::string& p)=0;
    virtual void pop_prefix()=0;
    virtual ~IConfigObj() {}

};
/**
*       Стековый префиксер
*       Использование CFG_PUSH("MyObject",cfg)
*       В имя всех параметров в области видимости добавится путь .MyObject
*/
class st_configObjPrefix
{
    IConfigObj* ref;
public:
    st_configObjPrefix(const std::string& part,IConfigObj* cfg)
        :ref(cfg)
    {
        ref->push_prefix(part);

    }
    ~st_configObjPrefix()
    {
        ref->pop_prefix();
    }

};

#define CFG_PUSH(a,b) st_configObjPrefix st_$$_dummy_obj$pushed(a,b)

#endif
