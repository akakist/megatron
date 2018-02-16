#include "js_utils.h"
#include "mutexInspector.h"
#define line_width 140
Json::Value SplitToStringArray(const std::string& buf)
{

    Json::Value ret;
    std::string s=buf;
    while(s.size())
    {
        size_t l=line_width<s.size()?line_width:s.size();
        std::string sl=s.substr(0,l);
        s=s.substr(l,s.size()-l);
        ret.append(sl);
    }
    return ret;
}
std::string JoinArrayToString(const Json::Value &v)
{

    std::string out;
    if(v.isArray())
    {
        for(size_t i=0; i<v.size(); i++)
        {
            if(!v[i].isString())
                throw CommonError("if(!v[i].isString())"+_DMI());
            out+=v[i].asString();
        }
    }
    else
        throw CommonError("!if(v.isArray())"+_DMI());
    return out;
}
