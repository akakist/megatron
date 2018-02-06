#ifndef __________mutex_inspector_entry_h
#define __________mutex_inspector_entry_h
struct mutex_inspector_entry
{
    struct iint
    {
        iint()
        {
        }
        std::string file;
        int line;
    };
    const char * f;
    const char * func;

    int l;
    time_t t;
    std::string __s;
    mutex_inspector_entry():f(NULL),func(NULL),l(0),t(0) {}
};

#endif
