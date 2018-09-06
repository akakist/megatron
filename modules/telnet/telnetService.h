#ifndef _____telnet___SERVICE______H
#define _____telnet___SERVICE______H
#include "unknown.h"
#include "SOCKET_id.h"


#include "broadcaster.h"
#include "listenerBuffered1Thread.h"

#include "version_mega.h"

#include "logging.h"
#include "socketsContainer.h"
#include "Events/Tools/telnet/RegisterCommand.h"
#include "Events/Tools/telnet/RegisterType.h"
#include "Events/Tools/telnet/Reply.h"
#include "Events/System/Run/startService.h"
#include "Events/System/Net/socket/Connected.h"
#include "Events/System/Net/socket/StreamRead.h"
#include "Events/System/Net/socket/Accepted.h"
#include "Events/System/Net/socket/NotifyBindAddress.h"



namespace Telnet
{

    class Node;
    class Session:public Refcountable, public Mutexable
    {


    public:
        Session(const REF_getter<Node> &N);

        REF_getter<Node> mx_defaultNode;
        std::string mx_current_command_line;
        std::vector<std::string> mx_command_history;
        int curpos;
        int command_history_pos;
        bool insertMode;
        int width,height;
        Json::Value jdump();
        REF_getter<Node> defaultNode()
        {
            M_LOCK(this);
            return mx_defaultNode;
        }
        void defaultNode(const REF_getter<Node> &n)
        {
            M_LOCK(this);
            mx_defaultNode=n;
        }

        bool on_TKEY_BS();

    };


    class _Command: public Refcountable
    {
    public:
        const route_t dstService;
        const std::deque<std::string> cmd;
        const std::deque<std::string> params;
        const std::string help;
        Json::Value jdump();
        _Command(const route_t& _svs, const std::deque<std::string>& _cmd, const std::deque<std::string>&_pars, const std::string& _help)
            : dstService(_svs), cmd(_cmd),params(_pars),help(_help)
        {}

    };
    class Node: public Refcountable, public Mutexable
    {
        std::map<std::string,REF_getter<Node> > m_children;
        std::map<std::deque<std::string>,REF_getter<_Command> > m_commands;
    public:
        Node(const std::string& _name, Node* _parent, const std::string& _help):name(_name), help(_help),parent(_parent) {}
        const std::string name;
        const std::string help;
        Json::Value jdump();
        Node* const parent;
        REF_getter<Node> getSubDir(const std::string& dir);
        void addChildNode(const std::string& section,const REF_getter<Node>&n);
        void addCommand(const std::deque<std::string>& cmd,const REF_getter<_Command>&n);
        std::map<std::string,REF_getter<Node> > children();
        std::map<std::deque<std::string>,REF_getter<_Command> > commands();
        std::string path();
    };


    class CommandEntries: public Mutexable
    {
        REF_getter<Node> m_root;
        std::map<std::string/*name*/,std::pair<std::string/*pattern*/,std::string/*help*/> > mx_types;
    public:
        CommandEntries(): m_root(new  Node("",NULL,"Top level commands")) {}

        REF_getter<Node> root();
        Json::Value jdump();
        void registerType(const std::string& _name, const std::string& _pattern, const std::string& _help);
        void registerDirectory(const std::deque<std::string> &d, const std::string& name, const std::string& help);
        void registerCommand(const std::deque<std::string> &d, const std::string& cmd, const std::string& help,const route_t& dst, const std::string &params);
        std::string getRegex(const std::string &param);
        std::map<std::string,std::string> getHelpOnTypes();
        std::map<std::string,std::string> getHelpOnRe();

    };

    class Service:
        private UnknownBase,
        Broadcaster,
        private ListenerBuffered1Thread,
        public Mutexable,
        Logging
    {
        class __telnet_stuff: public SocketsContainerBase
        {
            Mutex m_lock;
            std::map<SOCKET_id,REF_getter<Telnet::Session> > sessions;
        public:
            __telnet_stuff(): SocketsContainerBase("__telnet_stuff") {}
            void erase(const SOCKET_id& id);
            REF_getter<Telnet::Session> get(const SOCKET_id& id);
            void insert(const SOCKET_id& id,const REF_getter<Telnet::Session> &C);
            std::map<SOCKET_id,REF_getter<Telnet::Session> > getContainer();
            void on_delete(const REF_getter<epoll_socket_info>&esi, const std::string& reason);
            void on_mod_write(const REF_getter<epoll_socket_info>&) {}
            void clear()
            {
                SocketsContainerBase::clear();
                {
                    M_LOCK(m_lock);
                    sessions.clear();
                }

            }
        };

        bool on_RegisterType(const telnetEvent::RegisterType*);
        bool on_RegisterCommand(const telnetEvent::RegisterCommand*);
        bool on_Reply(const telnetEvent::Reply*);



        CommandEntries cmdEntries;

        bool on_StreamRead(const socketEvent::StreamRead* evt);
        bool on_Accepted(const socketEvent::Accepted* evt);
        bool on_NotifyBindAddress(const socketEvent::NotifyBindAddress*);

        bool on_startService(const systemEvent::startService*);

        void doListen();

        void processCommand(const REF_getter<Telnet::Session>&__S, const REF_getter<epoll_socket_info>&esi);
        void prompt(const REF_getter<Telnet::Session>& W, const REF_getter<epoll_socket_info>&esi);
        std::string promptString(const REF_getter<Telnet::Session>& W);

        bool paramMatch(const std::string & re, const std::string & exp);


        std::set<msockaddr_in> m_bindAddr;
        std::string m_deviceName;
        //bool m_disabled;
    public:
        static UnknownBase* construct(const SERVICE_id &id, const std::string& nm,IInstance* ifa)
        {
            XTRY;
            return new Service(id,nm,ifa);
            XPASS;
        }
        bool handleEvent(const REF_getter<Event::Base>& e);

        Service(const SERVICE_id& id, const std::string& nm,IInstance* ifa);
        ~Service() {
            stuff->clear();
        }

        REF_getter<__telnet_stuff> stuff;
        IInstance *iInstance;

    };
};
#endif
