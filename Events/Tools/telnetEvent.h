#pragma once


#include "EVENT_id.h"
#include "SERVICE_id.h"
namespace ServiceEnum
{
    const SERVICE_id Telnet (genum_Telnet);

}







namespace telnetEventEnum
{
    const EVENT_id RegisterType(genum_telnetRegisterType);
    const EVENT_id RegisterCommand(genum_telnetRegisterCommand);
    const EVENT_id Reply(genum_telnetReply);
    const EVENT_id CommandEntered(genum_telnetCommandEntered);
}




namespace telnetEvent
{

    class Reply: public Event::NoPacked
    {
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        Reply(const SOCKET_id& s,const std::string &_buffer, const route_t& r)
            :NoPacked(telnetEventEnum::Reply,r),socketId(s),buffer(_buffer) {}
        SOCKET_id socketId;
        std::string buffer;
        void jdump(Json::Value &) const
        {
        }

    };




    class RegisterType: public Event::NoPacked
    {
        /**
        *   Регистрация типа параметра.
        *   Направление - от клиента
        *   \param name наименование параметра, которое потом будет использоваться в командах.
        *   \param pattern regexp pattern
        *   \param help строка которая будет выводиться в хелпе для типа.
        */
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        RegisterType(const std::string &_name, const std::string& _pattern, const std::string& _help, const route_t& r)
            :NoPacked(telnetEventEnum::RegisterType,r),name(_name),pattern(_pattern),help(_help) {}

        std::string name,pattern,help;

        void jdump(Json::Value &) const
        {
        }



    };




    class RegisterCommand: public Event::NoPacked
    {
        /**
        *   Регистрация команды
        *   Направление - от клиента
        */
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        RegisterCommand(const std::string &_directory, const std::string& _cmd, const std::string& _help,const std::string &_params, const route_t&r):
            NoPacked(telnetEventEnum::RegisterCommand,r),directory(_directory),cmd(_cmd),help(_help),params(_params) {}
        std::string directory;
        std::string cmd;
        std::string help;
        std::string params;
        void jdump(Json::Value &) const
        {
        }

    };





    class CommandEntered: public Event::NoPacked
    {
        /**
        *   Регистрация команды
        *   Направление - от сервера
        *   \param socketId  идентидификатор соединения
        *   \param tokens массив принятых слов
        */
    public:
        static Base* construct(const route_t &)
        {
            return NULL;
        }
        CommandEntered(const SOCKET_id& s,const std::deque<std::string> &_tokens, const route_t& r)
            :NoPacked(telnetEventEnum::CommandEntered,r),socketId(s),tokens(_tokens) {}

        SOCKET_id socketId;
        std::deque<std::string> tokens;

        void jdump(Json::Value &) const
        {
        }

    };



};

