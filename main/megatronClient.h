#ifndef _______________MEGATRONCLIENT____H
#define _______________MEGATRONCLIENT____H
#include <stdlib.h>
#include <string>

class IInstance;
class IUtils;
namespace Megatron {

    IUtils *initMegatron(int argc, char **argv, const std::string& filesDir);
    IInstance *createInstance(const std::string& config);
//void setFilesDir(IUtils* iu,const std::string &fdir);

}


#endif
