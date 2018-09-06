#ifndef _______________MEGATRONCLIENT____H
#define _______________MEGATRONCLIENT____H
//#include "IConfigObj.h"
#include <stdlib.h>
#include <string>
//#include "IInstance.h"

class IInstance;
class IUtils;
namespace Megatron {

IUtils *initMegatron(int argc, char **argv);
IInstance *createInstance(const std::string& config);
void setFilesDir(IUtils* iu,const std::string &fdir);
void poll(IInstance* instance);

}


#endif
