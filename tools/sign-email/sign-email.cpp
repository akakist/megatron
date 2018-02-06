#include <stdio.h>
//#include "st_stuff.h"
//#include "str_lib.h"
//#include "mysql_extens.h"
#include <time.h>
#include <set>
#include "st_rsa.h"
//#include "st_FILE.h"
#include <vector>

std::string bin2hex(const std::string & in)
{
    std::string out = "";
    const unsigned char *p = (unsigned char *)in.c_str();
    for (unsigned int i = 0; i < in.size(); i++) {
        char s[40];
        ::snprintf(s, sizeof(s) - 1, "%02x", p[i]);
        out += s;
    }
    return out;
}

std::string join(const char* pattern,const std::vector<std::string>& arr)
{

    std::string ret;
    if (arr.size() > 1) {
        unsigned int i;
        for (i = 0; i < arr.size() - 1; i++)
            ret += arr[i] + pattern;
        ret += arr[arr.size() - 1];
    }
    else if (arr.size() == 1) {
        ret += arr[0];
    }
    return ret;

}

std::string bin2c(const std::string & in)
{
    std::string out = "";
    std::vector<std::string> v;
    const unsigned char *p = (unsigned char *)in.c_str();
    for (unsigned int i = 0; i < in.size(); i++) {
        char s[40];
        ::snprintf(s, sizeof(s) - 1, "0x%02x", p[i]);
        v.push_back(s);
    }
    return join(",",v);
}
extern "C" unsigned char ___rsa_private_key[];
extern "C" unsigned char ___rsa_private_key_sz;

int main(int argc, char **argv)
//main()
{
//    try{
    std::string email=argv[1];

    st_rsa rsa;
    std::string key((char*)___rsa_private_key,___rsa_private_key_sz);
    rsa.initFromPrivateKey(key);
    std::string enc=rsa.privateEncrypt(email);
    std::string out=email+":"+bin2hex(enc);
    printf("<--\n%s\n-->\n",out.c_str());
}
