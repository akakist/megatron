#ifndef _______________MEGATRON____H
#define _______________MEGATRON____H
#include "IConfigObj.h"
#include <stdlib.h>
#include "IInstance.h"
struct megatron
{
    IConfigObj* config;
    IInstance *iInstance;
//    bool runned;
    //void stop();
    ~megatron();
    megatron();
    void run(
        int argc, char* argv[]
    );
    //static void run_test(int argc, char *argv[]);
    //static void load_plugins_info(const std::set<std::string>& bases);

    //void close();
#if !defined __MOBILE__

    static bool findProcess(const char* process);
#endif

};

#endif
