#ifndef ____FILENAME_HH
#define ____FILENAME_HH
#include <string>
#ifdef QT5
#include <QString>
#endif
struct FileName
{
#ifdef QT5
    QString s;
#else
    std::string s;
#endif
};
#endif
