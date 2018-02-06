#ifndef _____________INconfigDBDB____H
#define _____________INconfigDBDB____H
#include <set>
#include <string>
#include <msockaddr_in.h>
#include "mtimespec.h"
#include <sys/types.h>
/** *       Интерфейс ядра для работы с конфигом in DB.
*/

class IConfigDB
{
public:
    virtual msockaddr_in get_tcpaddr(const std::string&name, const std::string& defv)const =0;
    virtual uint64_t get_uint64_t(const std::string&name, const  uint64_t& defv)const =0;
    virtual std::string get_string(const std::string&name, const std::string& defv)const =0;
    virtual void set(const std::string&, const std::string&v)=0;
    virtual ~IConfigDB() {}

};

#endif
