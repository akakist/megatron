#pragma once
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
    int privateEncrypt(const std::string &in, std::string& out);

    std::string privateDecrypt(const std::string &m);
    int privateDecrypt(const std::string &in, std::string& out);

    void initFromPrivateKey(const std::string &priv_key);
    int initFromPrivateKey_int(const std::string &priv_key);

    void initFromPrivateKeyFile(const std::string &fn);
    int initFromPrivateKeyFile_int(const std::string &fn);

    std::string getPrivateKey();
    std::string getPrivateKey_int();

    std::string publicEncrypt(const std::string &m);
    int publicEncrypt(const std::string &m, std::string& out);

    std::string publicDecrypt(const std::string &m);
    int publicDecrypt(const std::string &in,std::string& out);

    void initFromPublicKey(const std::string &pk);
    int initFromPublicKey_int(const std::string &pk);
    void initFromPublicKeyFile(const std::string &fn);
    int initFromPublicKeyFile_int(const std::string &fn);
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
    REF_getter<refbuffer> encrypt(const REF_getter<refbuffer>& buf_);
    REF_getter<refbuffer> decrypt(const REF_getter<refbuffer>& buf);
    std::string generateRandomKey();
};




