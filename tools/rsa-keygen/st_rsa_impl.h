#ifndef ___________________ST_RSA_IMPL_H
#define ___________________ST_RSA_IMPL_H

#include <string>
#include <stdint.h>
#include <openssl/err.h>
#include <openssl/rsa.h>
#include <openssl/md5.h>
#include <openssl/aes.h>
#include <openssl/x509.h>
#include <openssl/rand.h>

class st_rsa_impl
{
    std::string encrypt(const std::string &m, int (*func)(int, const unsigned char*, unsigned char*, RSA*,int));
    std::string decrypt(const std::string &m, int (*func)(int, const unsigned char*, unsigned char*, RSA*,int));
public:
    RSA *rsa_xxx;
    st_rsa_impl();
    ~st_rsa_impl();
    int size();
    void generate_key(int b);
    std::string privateEncrypt(const std::string &m);
    std::string privateDecrypt(const std::string &m);
    void initFromPrivateKey(const std::string &pk);
    std::string getPrivateKey();
    std::string publicEncrypt(const std::string &m);
    std::string publicDecrypt(const std::string &m);
    void initFromPublicKey(const std::string &pk);
    std::string getPublicKey();

};
class st_AES_impl
{
public:
    AES_KEY keydec,keyenc;
    bool inited;
    st_AES_impl();
    ~st_AES_impl();
    void init(const std::string &key);
    std::string encrypt(const std::string& buf_);
    std::string decrypt(const std::string& buf);
    std::string generateRandomKey();
    void encrypt(unsigned char*buf, size_t size);
    void decrypt(unsigned char*buf, size_t size);

};
class C_ssl
{
public:

    /// получить md5 cksum
    std::string md5(const std::string & s);
    std::string rand_bytes(int n);


    std::string sha1(const std::string & s);
    std::string sha256(const std::string & s);
    std::string sha512(const std::string & s);
    C_ssl();

};



#endif // ST_RSA_IMPL_H
