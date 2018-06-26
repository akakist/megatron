
#ifndef ST_MALLOC_H
#define ST_MALLOC_H
#include <stdio.h>
#include "commonError.h"
#ifndef __MACH__
#include <malloc.h>
#endif
/// stacked malloc, automatically call free() on destructor


struct st_malloc
{
private:
    st_malloc(const st_malloc&);             // Not defined
    st_malloc& operator=(const st_malloc&);  // Not defined
public:
    void *buf;
    st_malloc(size_t size)
    {
        buf = malloc(size + 1);
        if (buf==NULL)throw CommonError("st_malloc failed");
    }
    ~st_malloc()
    {
        if (buf)
        {
            free(buf);
            buf=NULL;
        }
    }

};
/*struct st_malloc_free
{
private:
    st_malloc_free(const st_malloc_free&);             // Not defined
    st_malloc_free& operator=(const st_malloc_free&);  // Not defined
public:
    unsigned char *buf;
    st_malloc_free(unsigned char* b):buf(b)
    {
    }
    ~st_malloc_free()
    {
        if (buf)
        {
            free(buf);
            buf=NULL;
        }
    }

};*/


#endif


