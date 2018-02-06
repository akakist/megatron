#ifndef _______TELNETSERVICE____EVENT_______HB
#define _______TELNETSERVICE____EVENT_______HB

#include "___telnetEvent.h"

namespace telnetEvent
{

class RegisterCommand: public Event::NoPacked
{
    /**
    *   Регистрация команды
    *   Направление - от клиента
    *   \param cmd команда, может состоять из нескольких слов
    *   \param help строка которая будет выводиться в хелпе для типа.
    *   \param directory родительская директория, разбитая на элементы
    *   \param params  строка со списком имен регекспов параметров
    */
public:
    static Base* construct(const route_t &)
    {
        return NULL;
    }
    RegisterCommand(const std::string &_directory, const std::string& _cmd, const std::string& _help,const std::string &_params, const route_t&r):
        NoPacked(telnetEventEnum::RegisterCommand,"telnetRegisterCommand",r),directory(_directory),cmd(_cmd),help(_help),params(_params) {}
    std::string directory;
    std::string cmd;
    std::string help;
    std::string params;
    void jdump(Json::Value &) const
    {
    }

};



};

#endif
