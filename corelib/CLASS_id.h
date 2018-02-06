#ifndef __________CLASS_id___H
#define __________CLASS_id___H
#include <stdlib.h>
#ifdef __ZDEBUG__
struct CLASS_id
{
    size_t container;
    CLASS_id(size_t i):container(i) {}

};
inline int operator<(const CLASS_id&a, const CLASS_id&b)
{
    return a.container<b.container;
}
#else
typedef size_t CLASS_id;
#endif
#endif
