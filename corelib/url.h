#ifndef _________________URL____H
#define _________________URL____H
#include <string>

/// url parser

struct Url
{

    std::string protocol;
    std::string host;
    std::string host_port;
    std::string path;
    std::string filename;
    std::string dirname;
    std::string params;
    std::string user;
    std::string pass;
    std::string port;
    std::string dump();
    void clear();
    void parse(const std::string& u);

};
#endif
