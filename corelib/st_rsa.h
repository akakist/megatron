#ifndef ____________ST_RSA__H
#define ____________ST_RSA__H
#include <string>
#include "ISSL.h"
typedef struct rsa_st RSA;

/// wrapper for RSA tools in OpenSSL

class st_rsa
{
    I_st_rsa_impl * impl;
public:
    st_rsa();
    ~st_rsa();

    int size();
    void generate_key(int b);
    std::string privateEncrypt(const std::string &m);
    std::string privateDecrypt(const std::string &m);
    void initFromPrivateKey(const std::string &pk);
    void initFromPrivateKeyFile(const std::string &fn);
    std::string getPrivateKey();
    std::string publicEncrypt(const std::string &m);
    std::string publicDecrypt(const std::string &m);
    void initFromPublicKey(const std::string &pk);
    void initFromPublicKeyFile(const std::string &fn);
    std::string getPublicKey();
};

/// wrapper for AES tools in OpenSSL

class st_AES
{
    I_st_AES_impl* impl;
public:
    st_AES();
    virtual ~st_AES();
    void init(const std::string &key);
    std::string encrypt(const std::string& buf_);
    std::string decrypt(const std::string& buf);
    void encrypt(unsigned char*buf, size_t size);
    void decrypt(unsigned char*buf, size_t size);
    std::string generateRandomKey();
};

#endif



