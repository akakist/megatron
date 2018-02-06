#ifndef ________________dfsCAPS____H
#define ________________dfsCAPS____H

#include <vector>
#include "SOCKET_id.h"
struct CapsItem
{
    msockaddr_in listenAddr;
    std::string caps;
};
inline outBuffer & operator<< (outBuffer& b,const CapsItem &s)
{
    b<<s.listenAddr<<s.caps;
    return b;
}
inline inBuffer & operator>> (inBuffer& b,  CapsItem &s)
{
    b>>s.listenAddr>>s.caps;
    return b;
}


#endif
