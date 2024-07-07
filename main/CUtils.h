#ifndef _________________CUTILS____H_HHH1
#define _________________CUTILS____H_HHH1
#ifndef _WIN32
#include <dlfcn.h>
#include <IUtils.h>
#include <CapsId.h>

#endif

#include "colorOutput.h"
#include "utils_local.h"
#include "Real.h"
#include "IInstance.h"

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


    std::map<std::string,std::string> loadStringMapFromFile(const std::string& pathname);
    std::map<std::string,std::string> loadStringMapFromBuffer(const std::string &bod, const char *linedelim);
    std::string  load_file(const std::string&);
    std::string load_file_no_throw(const std::string & fn);
    long  load_file_from_disk(std::string&, const std::string&);
    int64_t calcFileSize(const std::string & fn);
    int checkPath(const std::string & _pathname); /// 0 - no error
    std::string expand_homedir(const std::string& base);

    std::string findExecutable(const std::string& _fn);
    std::set < std::string> splitStringSET(const char *seps, const std::string & src);
    std::vector < std::string> splitString(const char *seps, const std::string & src);
    std::deque < std::string> splitStringDQ(const char *seps, const std::string & src);
    std::string join(const char* sep,const std::deque<std::string>& );
    std::string join(const char* sep,const std::vector<std::string>& );
    std::string join(const char *pattern, const std::set < std::string> &st);
    std::string trim(const std::string &s);

    std::string strupper(const std::string &s);
    std::string strlower(const std::string & s);
    std::string str_replace(const std::string &searchSample,const std::string &replacement,const std::string &src);
    void rxfind(std::vector < rxfind_data > &res, const char *regexp, const char *buffer);


    std::string Base64Encode(const std::string&);
    std::string  Base64Decode(const std::string&);
    std::string  hex2bin(const std::string&);
    std::string  bin2hex(const std::string&);
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

#ifdef __WITH_ZLIB
    REF_getter<refbuffer>  zcompress(const REF_getter<refbuffer>& data);
    REF_getter<refbuffer>  zexpand(const REF_getter<refbuffer>& data);
#endif
    std::string replace_vals(std::map<std::string,std::string> &p, const std::string &src);
    std::string rus_datetime(const time_t& t);
    std::string rus_date(const time_t& t);
    time_t from_rus_datetime(const std::string& s);

    std::string en_datetime(const time_t& t);
    std::string en_date(const time_t& t);
    time_t from_en_datetime(const std::string& s);


    std::string makeSlug(const std::string& s);

    SOCKET_id getSocketId();

    IThreadNameController* getIThreadNameController();




    Utils_local local;
    void registerIface(const VERSION_id& vid,const SERVICE_id& id, Ifaces::Base* p);
    Ifaces::Base* queryIface(const SERVICE_id& id);

    void registerITest(const VERSION_id& vid,const SERVICE_id& id, itest_static_constructor p);

    std::map<SERVICE_id, itest_static_constructor> getAllITests();


    Mutex m_app_name_mx;
    std::string m_app_name;
    const std::string app_name()
    {
        M_LOCK(m_app_name_mx);
        return m_app_name;
    }
    void set_app_name(const std::string& an)
    {
        M_LOCK(m_app_name_mx);
        m_app_name=an;
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

    struct __sockIdGen
    {
        std::atomic<int16_t> gen;
        __sockIdGen()
        {
            gen=0L;
        }
        SOCKET_id get()
        {
            // M_LOCK(this);
            SOCKET_id id;
            CONTAINER(id)=++gen;
            return id;
        }



    };
    __sockIdGen sockIdGen;


    void registerPlugingInfo(const VERSION_id& version, const char* pluginFileName, PluginType pt, const SERVICE_id &id, const char* name, const std::set<EVENT_id>& evts);
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

    std::string filesDir();
    void setFilesDir(const std::string& s);
    std::string m_files_dir;


    Utils_local *getLocals();
    struct __instances: public Mutexable
    {
        std::set<IInstance*> container;
        void clear()
        {
            std::set<IInstance*> ins;

            {
                M_LOCK(this);
                ins=container;
                container.clear();
            }
            for(auto z: ins)
            {
                printf(BLUE("deleting instance"));
                delete z;
            }

        }
    };
    __instances instances;
    std::set<IInstance*> getInstances();
    void registerInstance(IInstance *i);
    void unregisterInstance(IInstance *i);
    IInstance* createNewInstance(const std::string& name);

    bool m_isTerminating;
    void setTerminate();
    bool isTerminating();

    void load_plugins_info(const std::set<std::string>& bases);

    REF_getter<_addrInfos> m_addrInfos;
    REF_getter<_addrInfos> getAddrInfos() {
        return m_addrInfos;
    }

    struct _registered_dlls: public Mutexable
    {
#ifdef _WIN32
        std::set<HMODULE> registered_dlls;
#else
        std::set<void*> registered_dlls;
#endif
        void clear()
        {
            M_LOCK(this);
            for(auto h: registered_dlls)
            {
#ifdef _WIN32
                FreeLibrary(h);
#else
                dlclose(h);
#endif

            }
            registered_dlls.clear();

        }
    };
    _registered_dlls registered_dlls;


    std::set<pollable*> mx_pollables;
    Mutex mutex_pollables;
    void poll() {
        std::set<pollable*> p;
        {
            M_LOCK(mutex_pollables);
            p=mx_pollables;
        }
        for(auto &z :p)
            z->poll();
    }
    void addPollable(pollable* p) {
        M_LOCK(mutex_pollables);
        mx_pollables.insert(p);

    }
    void removePollable(pollable* p)
    {
        M_LOCK(mutex_pollables);
        mx_pollables.erase(p);

    }
    void clearPollable()
    {

        M_LOCK(mutex_pollables);
        mx_pollables.clear();
    }

    struct _logPrefix: public Mutexable
    {
        std::map<pthread_t,std::deque<std::string> > container;
    };
    _logPrefix logPrefix;
    void pushLogPrefix(const std::string& l)
    {
        M_LOCK(logPrefix);
        logPrefix.container[pthread_self()].push_back(l);

    }
    void popLogPrefix()
    {
        M_LOCK(logPrefix);
        logPrefix.container[pthread_self()].pop_back();
    }
    std::deque<std::string> getLogPrefix()
    {
        M_LOCK(logPrefix);
        return logPrefix.container[pthread_self()];
    }



};
#endif

