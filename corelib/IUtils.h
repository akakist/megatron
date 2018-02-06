#ifndef _________________UTILS____H_HHH
#define _________________UTILS____H_HHH
#include <vector>
#include <string>
#include <map>
#include <deque>
#include <set>
#include "Integer.h"
#include <sys/types.h>
#include "msockaddr_in.h"
#include "SERVICE_id.h"
#include "CHUNK_id.h"
#include "REF.h"
#include "Real.h"
#include "IThreadNameController.h"
#include "VERSION_id.h"
#include "ifaces.h"
#include "SOCKET_id.h"
#include "EVENT_id.h"
#include "unknown.h"
#include "webDumpable.h"
#include "event.h"
#include "ITests.h"
class epoll_socket_info;
struct Utils_local;

/**        Интерфейс разнообразных утилитных функций. Предназначен для избежания дублирования кода в модулях.
*/
class IInstance;
typedef UnknownBase* (*unknown_static_constructor) (const SERVICE_id& id, const std::string& name,IInstance* ifa);

class IUtils;

typedef ITests::Base* (*itest_static_constructor) ();
typedef void (*REGOBJECT)(IUtils*, const char*);


class IUtils
{
public:
    struct rxfind_data {
        std::string str;
        int so;
        int eo;
    };
    /// загрузить мап из файла. В файле перечисляются данные в виде пар key=value
    virtual std::map<std::string,std::string> loadStringMapFromFile(const std::string& pathname) =0;

    /// загрузить мап из буфера
    virtual std::map<std::string,std::string> loadStringMapFromBuffer(const std::string &body, const char *linedelim)=0;

    /// загрузить файл в буфер.
    /// \throw CommonError
    virtual std::string load_file(const std::string & fn)=0;
    virtual std::string load_file_no_throw(const std::string & fn)=0;
    /// загрузить файл с диска без throw
    virtual long load_file_from_disk(std::string & res, const std::string & fn)=0;

    virtual int64_t calcFileSize(const std::string & fn)=0;
    virtual int checkPath(const std::string & _pathname)=0;
    virtual std::string findExecutable(const std::string& _fn)=0;

    virtual std::string expand_homedir(const std::string& base)=0;


    /// разбить строку по разделителю
    virtual std::vector < std::string> splitString(const char *seps, const std::string & src)=0;

    virtual std::set < std::string> splitStringSET(const char *seps, const std::string & src)=0;

    /// разбить строку по разделителю
    virtual std::deque < std::string> splitStringDQ(const char *seps, const std::string & src)=0;

    /// объединить строку с разделителем
    virtual std::string join(const char* sep,const std::deque<std::string>& )=0;

    /// объединить строку с разделителем
    virtual std::string join(const char* sep,const std::vector<std::string>& )=0;

    /// объединить строку с разделителем
    virtual std::string join(const char *pattern, const std::set < std::string> &st)=0;

    /// удалить пробелы с краев
    virtual std::string trim(const std::string &s)=0;

    virtual std::string toString(real i)=0;

    virtual std::string toString(int64_t i)=0;
    virtual std::string toString(uint64_t i)=0;
    virtual std::string toString(int32_t i)=0;
    virtual std::string toString(uint32_t i)=0;
    virtual std::string toString(int16_t i)=0;
    virtual std::string toString(uint16_t i)=0;
    virtual std::string toString(int8_t i)=0;
    virtual std::string toString(uint8_t i)=0;
#ifndef _LP64
    //virtual std::string toString(time_t i)=0;
#endif
    virtual std::string toString(const std::pair<int64_t,int64_t> &i)=0;
#ifdef __MACH__
    virtual std::string toString(size_t i)=0;
#endif
#ifdef _WIN32
//    virtual std::string toString(long int i)=0;
#endif
    virtual void rxfind(std::vector < rxfind_data > &res, const char *regexp, const char *buffer)=0;


    /// сконверировать строку в верхний регистр (без учета кодировок)
    virtual std::string strupper(const std::string &s)=0;

    /// сконверировать строку в нижний регистр (без учета кодировок)
    virtual std::string strlower(const std::string & s)=0;

    virtual std::string str_replace(const std::string &searchSample,const std::string &replacement,const std::string &src)=0;


    /// закодировать буфер в base64
    virtual std::string Base64Encode(const std::string &str)=0;

    /// раскодировать буфер из base64
    virtual std::string Base64Decode(const std::string &str)=0;

    /// закодировать в HEX
    virtual std::string hex2bin(const std::string &s)=0;

    /// раскодировать из HEX
    virtual std::string bin2hex(const std::string & in)=0;
    /// раскодировать из escaped
    virtual std::string bin2escaped(const std::string & in)=0;


    virtual std::string uriEncode(const std::string & sSrc)=0;
    virtual std::string uriDecode(const std::string & sSrc)=0;



    /// убрать из URL escapes
    virtual std::string unescapeURL(const std::string & s)=0;
    virtual std::string extractFileExt(const std::string & f)=0;
    virtual std::string extractFileName(const std::string & f)=0;


    virtual std::string get_param_string(std::deque<std::string> &tokens, const std::string& name)=0;
    virtual int get_param_int(std::deque<std::string> &tokens, const std::string& name)=0;
    virtual int64_t get_param_int64_t(std::deque<std::string> &tokens, const std::string& name)=0;

    virtual Integer getNow()=0;
    virtual std::string getPercent(const real& numerator, const real& denumerator)=0;

    virtual int getHostByName(const char* hostname, unsigned int& outHostname)=0; // 0 - no error, -1 error
    virtual std::string dump(const std::set<msockaddr_in> &s)=0;
    virtual std::string dump(const std::deque<msockaddr_in> &s)=0;
    virtual std::string dump(const std::vector<msockaddr_in> &s)=0;
    virtual std::string dump(const std::set<std::pair<msockaddr_in,std::string> > &s)=0;
    virtual std::string dump(const std::map<SERVICE_id,std::set<msockaddr_in> > &s)=0;
    virtual std::string dump(const std::map<msockaddr_in,std::set<SERVICE_id> > &s)=0;


    virtual bool readable_fd(const REF_getter<epoll_socket_info>& esi,int sec, int usec)=0;
    virtual bool writeable_fd(const REF_getter<epoll_socket_info>& esi, int timeout_sec, int timeout_usec)=0;

//#ifndef _WIN32
    virtual REF_getter<refbuffer>  zcompress(const REF_getter<refbuffer>& data)=0;
    virtual REF_getter<refbuffer>  zexpand(const REF_getter<refbuffer>& data)=0;
//#endif

    virtual std::string replace_vals(std::map<std::string,std::string> &p, const std::string &src)=0;

    virtual std::string rus_datetime(const time_t& t)=0;
    virtual std::string rus_date(const time_t& t)=0;
    virtual time_t from_rus_datetime(const std::string& s)=0;
    virtual std::string en_datetime(const time_t& t)=0;
    virtual std::string en_date(const time_t& t)=0;
    virtual time_t from_en_datetime(const std::string& s)=0;

    virtual std::string makeSlug(const std::string& s)=0;


    virtual ~IUtils() {}


    /// получить новый ИД сокета
    virtual SOCKET_id getSocketId()=0;

    /// пометить сокет ИД как ненужный
    virtual void ungetSocketId(const SOCKET_id& s)=0;

    /// получить число аллокированных сокетов
    virtual size_t getSocketCount()=0;
    virtual IThreadNameController* getIThreadNameController()=0;


    virtual void registerIface(const VERSION_id&,const SERVICE_id& id, Ifaces::Base* p)=0;
    virtual Ifaces::Base* queryIface(const SERVICE_id& id)=0;

    virtual void registerITest(const VERSION_id& vid,const SERVICE_id& id, itest_static_constructor p)=0;
    //virtual itest_static_constructor queryITest(const SERVICE_id& id)=0;
    virtual std::vector<itest_static_constructor> getAllITests()=0;


    virtual const std::string app_name()=0;

    virtual std::string gCacheDir()=0;
    virtual std::string gLogDir()=0;
    virtual std::string gConfigDir()=0;
#ifdef __MOBILE__
    virtual void setFilesDir(const std::string& s)=0;
    virtual std::string filesDir()=0;
#endif

    virtual int argc()=0;
    virtual char **argv()=0;

    enum PluginType
    {
        PLUGIN_TYPE_IFACE,
        PLUGIN_TYPE_SERVICE,
        PLUGIN_TYPE_TEST,
    };
    virtual void registerPlugingInfo(const VERSION_id& version, const char* pluginFileName, PluginType pt, const SERVICE_id &id, const char* name)=0;
    virtual void registerPluginDLL(const std::string& pn)=0;

    /// зарегистрировать сервис
    virtual void registerService(const VERSION_id& ver, const SERVICE_id& id, unknown_static_constructor cs, const std::string& literalName)=0;


    /// зарегистрировать евент
    /// \param id идентификатор евента
    /// \param ec статический конструктор евента
    /// \param literalName строковое имя евента
    virtual void registerEvent(event_static_constructor ec)=0;


    virtual REF_getter<Event::Base> unpackEvent(inBuffer&)=0;
    virtual void packEvent(outBuffer&, const REF_getter<Event::Base>&)const =0;

    /// получить имя сервиса по его ИД
    virtual std::string serviceName(const SERVICE_id& id)const=0;

    /// получить ИД сервиса по его имени
    virtual SERVICE_id serviceIdByName(const std::string& name)const=0;
    virtual bool isServiceRegistered(const SERVICE_id& svs)=0;

    virtual void setWebDumpableHandler(WebDumpable* h)=0;
    virtual void removeWebDumpableHandler(WebDumpable* h)=0;
    virtual std::string dumpWebDumpable(WebDumpable* )=0;

    virtual Utils_local *getLocals()=0;

    //virtual int64_t getEmailKey()=0;
    //virtual void setEmailKey(int64_t)=0;
    virtual std::set<IInstance*> getInstances()=0;
    virtual void registerInstance(IInstance *i)=0;
    virtual void unregisterInstance(IInstance *i)=0;
    virtual IInstance* createNewInstance()=0;
    virtual void setTerminate()=0;
    virtual bool isTerminating()=0;
    virtual  void load_plugins_info(const std::set<std::string>& bases)=0;

};
extern IUtils * iUtils;
#endif

