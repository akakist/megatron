#ifndef ____NEIGBORS___H
#define ____NEIGBORS___H
#include <set>
#include <map>
#include <time.h>
#include "mutexable.h"
#include "msockaddr_in.h"
#include "mutexInspector.h"
#include"DBH.h"

#include "st_FILE.h"
namespace dfsReferrer
{
    class _neighbours: public Mutexable /// sosedi referrers
    {
    public:
//#ifdef __ANDROID__
        std::set<msockaddr_in>sas;
//#endif
        _neighbours();
        void remove(const msockaddr_in& sa);
        std::vector<msockaddr_in> getAllAndClear();
        void add(const msockaddr_in &sa, int pingTime);
    };


}
#endif
