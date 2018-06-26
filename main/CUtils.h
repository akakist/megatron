#ifndef _________________CUTILS____H_HHH
#define _________________CUTILS____H_HHH
#include <vector>
#include <string>
#include <map>
#include <deque>
#include <set>
#include "IUtils.h"
#include "mutexable.h"
#include "threadNameCtl.h"
#include "commonError.h"
#include "mutexInspector.h"
#include "version_mega.h"
#include "ITests.h"
#include "utils_local.h"

class CUtils: public IUtils
{
public:

    CUtils(    int ac, char** av, const std::string& app_name):
        m_app_name(app_name.substr(0,app_name.rfind("-d"))),
        _argc(ac),
        _argv(av),
        m_isTerminating(false),
        m_addrInfos(new _addrInfos)

    {}
    ~CUtils();

//#if !defined __MOBILE__

    std::map<std::string,std::string> loadStringMapFromFile(const std::string& pathname);
    std::map<std::string,std::string> loadStringMapFromBuffer(const std::string &body, const char *linedelim);
    std::string  load_file(const std::string&);
    std::string load_file_no_throw(const std::string & fn);
    long  load_file_from_disk(std::string&, const std::string&);
    int64_t calcFileSize(const std::string & fn);
    int checkPath(const std::string & _pathname); /// 0 - no error
    std::string expand_homedir(const std::string& base);

    std::string findExecutable(const std::string& _fn);
//#endif
    std::set < std::string> splitStringSET(const char *seps, const std::string & src);
    std::vector < std::string> splitString(const char *seps, const std::string & src);
    std::deque < std::string> splitStringDQ(const char *seps, const std::string & src);
    std::string join(const char* sep,const std::deque<std::string>& );
    std::string join(const char* sep,const std::vector<std::string>& );
    std::string join(const char *pattern, const std::set < std::string> &st);
    std::string trim(const std::string &s);
    std::string toString(int64_t i);
    std::string toString(uint64_t i);
    std::string toString(int32_t i);
    std::string toString(uint32_t i);
    std::string toString(int16_t i);
    std::string toString(uint16_t i);
    std::string toString(int8_t i);
    std::string toString(uint8_t i);
#ifdef __MACH__
    std::string toString(size_t i);
#endif
    std::string toString(real i);
    std::string toString(const std::pair<int64_t,int64_t> &i);




    std::string strupper(const std::string &s);
    std::string strlower(const std::string & s);
    std::string str_replace(const std::string &searchSample,const std::string &replacement,const std::string &src);
    void rxfind(std::vector < rxfind_data > &res, const char *regexp, const char *buffer);


    std::string Base64Encode(const std::string&);
    std::string  Base64Decode(const std::string&);
    std::string  hex2bin(const std::string&);
    std::string  bin2hex(const std::string&);
//    std::string bin2escaped(const std::string & in);
    std::string uriEncode(const std::string & sSrc);
    std::string uriDecode(const std::string & sSrc);



    std::string unescapeURL(const std::string&);


    std::string get_param_string(std::deque<std::string> &tokens, const std::string& name);
    int get_param_int(std::deque<std::string> &tokens, const std::string& name);
    int64_t get_param_int64_t(std::deque<std::string> &tokens, const std::string& name);

    Integer getNow();
    std::string getPercent(const real &numerator, const real &denumerator);


    struct _hostnames: Mutexable
    {
        std::map<std::string, unsigned int>  cache;
    };
    _hostnames hostnames;
    int getHostByName(const char* hostname, unsigned int& outHostname); // 0 - no error, -1 error
    std::string dump(const std::set<msockaddr_in> &s);
    std::string dump(const std::deque<msockaddr_in> &s);
    std::string dump(const std::vector<msockaddr_in> &s);
    std::string dump(const std::set<std::pair<msockaddr_in,std::string> > &s);
    std::string dump(const std::map<SERVICE_id,std::set<msockaddr_in> > &s);
    std::string dump(const std::map<msockaddr_in,std::set<SERVICE_id> > &s);

    std::string extractFileExt(const std::string&);
    std::string extractFileName(const std::string & f);
    bool readable_fd(const REF_getter<epoll_socket_info>& esi,int sec, int usec);
    bool writeable_fd(const REF_getter<epoll_socket_info>& esi, int timeout_sec, int timeout_usec);

//#ifndef _WIN32
    REF_getter<refbuffer>  zcompress(const REF_getter<refbuffer>& data);
    REF_getter<refbuffer>  zexpand(const REF_getter<refbuffer>& data);
//#endif
    std::string replace_vals(std::map<std::string,std::string> &p, const std::string &src);
    std::string rus_datetime(const time_t& t);
    std::string rus_date(const time_t& t);
    time_t from_rus_datetime(const std::string& s);

    std::string en_datetime(const time_t& t);
    std::string en_date(const time_t& t);
    time_t from_en_datetime(const std::string& s);


    std::string makeSlug(const std::string& s);

//////////////
    SOCKET_id getSocketId();
    void ungetSocketId(const SOCKET_id& s);
    size_t getSocketCount();

    IThreadNameController* getIThreadNameController();




    Utils_local local;
    void registerIface(const VERSION_id& vid,const SERVICE_id& id, Ifaces::Base* p);
    Ifaces::Base* queryIface(const SERVICE_id& id);

    void registerITest(const VERSION_id& vid,const SERVICE_id& id, itest_static_constructor p);

    std::vector<itest_static_constructor> getAllITests();


    const std::string m_app_name;
    const std::string app_name()
    {
        return m_app_name;
    }

    std::string gCacheDir();
    std::string gLogDir();
    std::string gConfigDir();

    int argc() {
        return _argc;
    }
    char **argv() {
        return _argv;
    }
    int _argc;
    char** _argv;

    struct __sockIdGen: public Mutexable
    {
        SOCKET_id gen;
        __sockIdGen()
        {
            CONTAINER(gen)=0L;
        }
        std::set<SOCKET_id> usedItems;
        SOCKET_id get()
        {
            M_LOCK(this);
            CONTAINER(gen)++;
            if(CONTAINER(gen)<=0) CONTAINER(gen)=1;
            return gen;
        }
        void unget(const SOCKET_id& id)
        {
            M_LOCK(this);
            usedItems.erase(id);
        }
        size_t size()
        {
            M_LOCK(this);
            return usedItems.size();
        }



    };
    __sockIdGen sockIdGen;


    void registerPlugingInfo(const VERSION_id& version, const char* pluginFileName, PluginType pt, const SERVICE_id &id, const char* name);
    void registerPluginDLL(const std::string& pn);

    void registerService(const VERSION_id&, const SERVICE_id& id, unknown_static_constructor cs, const std::string& literalName);



    void registerEvent(event_static_constructor ec);
    REF_getter<Event::Base> unpackEvent(inBuffer&);
    void packEvent(outBuffer&, const REF_getter<Event::Base>&)const;

    std::string serviceName(const SERVICE_id& id) const;
    SERVICE_id serviceIdByName(const std::string& name)const;
    bool isServiceRegistered(const SERVICE_id& svs);

    struct _webDumping: public Mutexable
    {
        std::set<WebDumpable*> container;
    };
    _webDumping webDumping;
    void setWebDumpableHandler(WebDumpable *h);
    void removeWebDumpableHandler(WebDumpable *h);
    std::string dumpWebDumpable(WebDumpable* h);

#ifdef __MOBILE__
    std::string filesDir();
    void setFilesDir(const std::string& s);
    std::string m_files_dir;
#endif


    Utils_local *getLocals();
    struct __instances: public Mutexable
    {
        std::set<IInstance*> container;
    };
    __instances instances;
    std::set<IInstance*> getInstances();
    void registerInstance(IInstance *i);
    void unregisterInstance(IInstance *i);
    IInstance* createNewInstance();

    bool m_isTerminating;
    void setTerminate();
    bool isTerminating();

    void load_plugins_info(const std::set<std::string>& bases);

    REF_getter<_addrInfos> m_addrInfos;
    REF_getter<_addrInfos> getAddrInfos(){return m_addrInfos;}

    struct _registered_dlls: public Mutexable
    {
        std::set<void*> registered_dlls;
    };
    _registered_dlls registered_dlls;

};
#endif

