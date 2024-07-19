#pragma once

#include <stdint.h>
#include "commonError.h"
#include "ioBuffer.h"
#include <openssl/rsa.h>
#include <openssl/md5.h>
#include <openssl/aes.h>
#include <openssl/x509.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include "ISSL.h"
#include "mutexable.h"

class st_rsa_impl: public I_st_rsa_impl
{
    std::string encrypt(const std::string &m, int (*func)(int, const unsigned char*, unsigned char*, RSA*,int));
    std::string decrypt(const std::string &m, int (*func)(int, const unsigned char*, unsigned char*, RSA*,int));
    int encrypt(const std::string &m, std::string&out, int (*func)(int, const unsigned char*, unsigned char*, RSA*,int));
    int decrypt(const std::string &m, std::string&out, int (*func)(int, const unsigned char*, unsigned char*, RSA*,int));
public:
    RSA *rsa_xxx;
    st_rsa_impl();
    ~st_rsa_impl();
    int size();
    void generate_key(int b);
    std::string privateEncrypt(const std::string &m);
    std::string privateDecrypt(const std::string &m);
    std::string publicEncrypt(const std::string &m);
    std::string publicDecrypt(const std::string &m);

    int privateEncrypt(const std::string &m,std::string& out);
    int privateDecrypt(const std::string &m,std::string& out);
    int publicEncrypt(const std::string &m,std::string& out);
    int publicDecrypt(const std::string &m,std::string& out);

    void initFromPrivateKey(const std::string &pk);
    void initFromPublicKey(const std::string &pk);
    int initFromPrivateKey_int(const std::string &pk);
    int initFromPublicKey_int(const std::string &pk);
    std::string getPrivateKey();
    std::string getPublicKey();

};
class st_AES_impl: public I_st_AES_impl
{
public:
    AES_KEY keydec,keyenc;
    bool inited;
    st_AES_impl();
    ~st_AES_impl();
    void init(const std::string &key);
    REF_getter<refbuffer> encrypt(const REF_getter<refbuffer>& buf_);
    REF_getter<refbuffer> decrypt(const REF_getter<refbuffer>& buf);
    std::string generateRandomKey();

};
class C_ssl: public I_ssl
{
public:
    I_st_rsa_impl *rsa_impl()
    {
        return new st_rsa_impl;
    }
    I_st_AES_impl *aes_impl()
    {
        return new st_AES_impl;
    }

    /// получить md5 cksum
    std::string md5(const std::string & s);
    std::string rand_bytes(int n);
    void rand_bytes(uint8_t* p, int n);

    int rand_int();

    std::string sha1(const std::string & s);
    std::string sha256(const std::string & s);
    std::string sha512(const std::string & s);

    C_ssl();
    ~C_ssl() {}

};



