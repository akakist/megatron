#ifndef ________________ISSLAV___G11
#define ________________ISSLAV___G11
#include <stdint.h>
#include "ifaces.h"
#include <string>
#include <sys/types.h>

/// interfaces/wrapper to access OpenSSL

class I_st_rsa_impl
{
public:
    virtual int size()=0;
    virtual void generate_key(int b)=0;
    virtual std::string privateEncrypt(const std::string &m)=0;
    virtual std::string privateDecrypt(const std::string &m)=0;
    virtual void initFromPrivateKey(const std::string &pk)=0;
    virtual std::string getPrivateKey()=0;
    virtual std::string publicEncrypt(const std::string &m)=0;
    virtual std::string publicDecrypt(const std::string &m)=0;
    virtual void initFromPublicKey(const std::string &pk)=0;
    virtual std::string getPublicKey()=0;
    virtual ~I_st_rsa_impl() {}

};

class I_st_AES_impl
{
public:
    virtual void init(const std::string &key)=0;
    virtual std::string encrypt(const std::string& buf_)=0;
    virtual std::string decrypt(const std::string& buf)=0;
    virtual std::string generateRandomKey()=0;
    virtual  ~I_st_AES_impl() {}
    virtual void encrypt(unsigned char*buf, size_t size)=0;
    virtual void decrypt(unsigned char*buf, size_t size)=0;

};

class I_ssl: public Ifaces::Base
{
public:
    virtual I_st_rsa_impl *rsa_impl()=0;
    virtual I_st_AES_impl *aes_impl()=0;

    /// получить md5 cksum
    virtual std::string md5(const std::string & s)=0;
    virtual std::string sha1(const std::string & s)=0;
    virtual std::string sha256(const std::string & s)=0;
    virtual std::string sha512(const std::string & s)=0;
    virtual std::string rand_bytes(int n)=0;
    virtual void rand_bytes(uint8_t* p, int n)=0;
    virtual int rand_int()=0;

};

#endif
