#include "st_rsa_impl.h"
#include "configDB.h"
#ifdef WIN32
#include <malloc.h>
#endif
#include "mutexInspector.h"
#include "st_rsa.h"
#include "rsa-pub_512_1387663237.h"

#ifdef _MSC_VER
#pragma comment(lib, "libeay32.lib")
#pragma comment(lib, "ssleay32.lib")
#endif
#ifdef ________________


typedef struct
{
    int ver;                    /*!<  always 0          */
    int len;                    /*!<  size(N) in chars  */

    mpi N;                      /*!<  public modulus    */
    mpi E;                      /*!<  public exponent   */

    mpi D;                      /*!<  private exponent  */
    mpi P;                      /*!<  1st prime factor  */
    mpi Q;                      /*!<  2nd prime factor  */
    mpi DP;                     /*!<  D % (P - 1)       */
    mpi DQ;                     /*!<  D % (Q - 1)       */
    mpi QP;                     /*!<  1 / (Q % P)       */

    mpi RN;                     /*!<  cached R^2 mod N  */
    mpi RP;                     /*!<  cached R^2 mod P  */
    mpi RQ;                     /*!<  cached R^2 mod Q  */

    int padding;                /*!<  1.5 or OAEP/PSS   */
    int hash_id;                /*!<  hash identifier   */
    int (*f_rng)(void *);       /*!<  RNG function      */
    void *p_rng;                /*!<  RNG parameter     */
}

#endif

void st_rsa_impl::generate_key(const int bits)
{

    XTRY;

    unsigned long   e = RSA_F4;
    int             ret = 0;
    BIGNUM          *bne = NULL;
    bne = BN_new();
    ret = BN_set_word(bne,e);
    if(ret != 1) {
        throw CommonError("BN_set_word");
    }

    rsa_xxx = RSA_new();
    ret = RSA_generate_key_ex(rsa_xxx, bits, bne, NULL);
    if(ret != 1) {
        throw CommonError("RSA_generate_key_ex");
    }
    BN_free(bne);


    XPASS;
}

void st_rsa_impl::initFromPublicKey(const std::string &priv_key)
{

    XTRY;
    const unsigned char * p = ( const unsigned char *)priv_key.data();
    rsa_xxx = d2i_RSA_PUBKEY(&rsa_xxx, &p, priv_key.size());
    if (!rsa_xxx)
    {
        throw CommonError("d2i_RSAPublicKey %s",_DMI().c_str());
    }
    XPASS;;
}


void st_rsa_impl::initFromPrivateKey(const std::string &priv_key)
{

    XTRY;
    const unsigned char *p = (const unsigned char *)priv_key.data();
    rsa_xxx = d2i_RSAPrivateKey(NULL, &p, priv_key.size());
    if (!rsa_xxx)
    {
        throw CommonError("d2i_RSAPrivateKey"+_DMI());
    }

    XPASS;
}

std::string st_rsa_impl::getPublicKey()
{

    XTRY;
    if (!rsa_xxx) throw CommonError("!rsa_xxx");

    std::string s;
    char b[10000];
    unsigned char *p = (unsigned char *) b;
    int len = 0;
    if (rsa_xxx && p)
    {
        len = i2d_RSA_PUBKEY(rsa_xxx, &p);
        if (len < 0)
            return "";
        std::string s((char *) b, len);
        return s;
    }
    else
    {
        throw CommonError("i2d_RSAPublicKey %s %d %s",__FILE__,__LINE__,_DMI().c_str());
    }
    return s;
    XPASS;
}

std::string st_rsa_impl::getPrivateKey()
{

    XTRY;
    if (!rsa_xxx) throw CommonError("!rsa_xxx");

    std::string s;
    char b[10000];
    unsigned char *p = (unsigned char *) b;
    int len = 0;
    if (rsa_xxx && p)
    {
        len = i2d_RSAPrivateKey(rsa_xxx, &p);
        if (len < 0)
            return "";
        std::string s((char *) b, len);
        return s;
    }
    else
    {
        throw CommonError("i2d_RSAPrivateKey");
    }
    return s;
    XPASS;
}

std::string st_rsa_impl::privateDecrypt(const std::string & m)
{

    XTRY;

    if (!rsa_xxx) throw CommonError("!rsa_xxx");
    return decrypt(m, RSA_private_decrypt);
    XPASS;
}
std::string st_rsa_impl::publicDecrypt(const std::string & m)
{

    XTRY;

    if (!rsa_xxx) throw CommonError("!rsa_xxx");
    return decrypt(m, RSA_public_decrypt);
    XPASS;
}

std::string st_rsa_impl::publicEncrypt(const std::string & m)
{

    XTRY;

    if (!rsa_xxx) throw CommonError("!rsa_xxx");
    return encrypt(m, RSA_public_encrypt);
    XPASS;
}

std::string st_rsa_impl::privateEncrypt(const std::string & m)
{

    XTRY;

    if (!rsa_xxx) throw CommonError("!rsa_xxx");
    return encrypt(m, RSA_private_encrypt);
    XPASS;
}
std::string st_rsa_impl::encrypt(const std::string & ms, int (*func) (int, const unsigned char *, unsigned char *, RSA *, int))
{

    XTRY;

    if (!rsa_xxx) throw CommonError("!rsa_xxx");
    int rsize = size()-RSA_PKCS1_PADDING_SIZE;
    std::string out;
    char  outBuf[size()];

    size_t i=0;
    for (i = 0; i < ms.size(); i += rsize)
    {
        char inBuf[size()];
        size_t sz=rsize;
        if (i+rsize>ms.size()) sz=ms.size()-i;
        memcpy(inBuf, (unsigned char *) &ms.data()[i],sz);
        int ret=func(sz, (unsigned char*)inBuf, (unsigned char*)outBuf, rsa_xxx, RSA_PKCS1_PADDING);
        if (ret==-1)
        {
            char errbuf[1024];
            ERR_error_string_n(ERR_get_error(),errbuf,sizeof(errbuf));
            throw CommonError("rsa error %s",errbuf);
        }
        out+=std::string((char*)outBuf,ret);
    }
    return out;
    XPASS;
}

std::string st_rsa_impl::decrypt(const std::string & m, int (*func) (int, const unsigned char *, unsigned char *, RSA *, int))
{

    XTRY;
    if (!rsa_xxx) throw CommonError("!rsa_xxx");
    if (m.size()%size()!=0)  throw CommonError("rsa: invalid buffer size"+_DMI());

    int rsize = size();
    char inBuf[size()],outBuf[size()];
    std::string out;

    size_t i=0;
    for (i = 0; i < m.size(); i += rsize)
    {
        memcpy(inBuf, (unsigned char *) &m.data()[i],rsize);
        int ret=func(rsize, (unsigned char *)inBuf, (unsigned char *)outBuf, rsa_xxx, RSA_PKCS1_PADDING);
        if (ret==-1)
        {
            char errbuf[1024];
            ERR_error_string_n(ERR_get_error(),errbuf,sizeof(errbuf));
            throw CommonError("rsa error %s %s",errbuf,_DMI().c_str());
        }
        out+=std::string((char*)outBuf,ret);
    }
    return out;
    XPASS;
}
st_rsa_impl::st_rsa_impl()
{
    rsa_xxx = NULL;

}
st_rsa_impl::~st_rsa_impl()
{
    if (rsa_xxx)
    {
        RSA_free(rsa_xxx);
    }
}
int st_rsa_impl::size()
{


    if (!rsa_xxx) throw CommonError("!rsa_xxx");
    return RSA_size(rsa_xxx);
}


st_AES_impl::st_AES_impl():inited(false)
{


    XTRY;
    memset(&keydec,0,sizeof(keydec));
    memset(&keyenc,0,sizeof(keyenc));
    XPASS;
}

void st_AES_impl::init(const std::string &key)
{

    XTRY;
    std::string keyBuff;
    while(keyBuff.size()<16)
    {
        keyBuff+=key;
    }

    if (AES_set_encrypt_key((unsigned char*)keyBuff.data(),128,&keyenc))throw CommonError("AES_set_encrypt_key");
    if (AES_set_decrypt_key((unsigned char*)keyBuff.data(),128,&keydec))throw CommonError("AES_set_decrypt_key");
    inited=true;
    XPASS;
}

std::string st_AES_impl::encrypt(const std::string& buf_)
{

    XTRY;
    if (!inited)
    {
        throw CommonError("st_AES not inited "+_DMI());
        //exit(0);
    }

    std::string buf=buf_;
    int l=0x10;
    if (buf.size()%AES_BLOCK_SIZE!=0)
    {
        l=AES_BLOCK_SIZE - (buf.size()%AES_BLOCK_SIZE);
    }
    for (int i=0; i<l; i++)
    {
        buf+=l;
    }
    std::string out;
    for (unsigned i=0; i<buf.size(); i+=AES_BLOCK_SIZE)
    {
        int l=AES_BLOCK_SIZE;
        if (i+l>buf.size()) l=buf.size()-i;
        unsigned char ib[AES_BLOCK_SIZE];
        unsigned char ob[AES_BLOCK_SIZE];
        memset(ib,0,sizeof(ib));
        memset(ob,0,sizeof(ob));
        memcpy(ib,&buf.data()[i],l);
        AES_ecb_encrypt(ib,ob,&keyenc,AES_ENCRYPT);
        out+=std::string((char*)ob,AES_BLOCK_SIZE);
    }
    return out;
    XPASS;
}
void  st_AES_impl::encrypt(unsigned char*buf, size_t size)
{
    if (!inited)
    {
        //     throw CommonError("st_AES not inited "+_DMI());
        exit(0);
    }
    if(size%AES_BLOCK_SIZE)     throw CommonError("if(size mod AES_BLOCK_SIZE)"+_DMI());
    for (unsigned i=0; i<size; i+=AES_BLOCK_SIZE)
    {
        unsigned char ib[AES_BLOCK_SIZE];
        unsigned char ob[AES_BLOCK_SIZE];
        memset(ib,0,sizeof(ib));
        memset(ob,0,sizeof(ob));
        memcpy(ib,&buf[i],AES_BLOCK_SIZE);
        AES_ecb_encrypt(ib,ob,&keyenc,AES_ENCRYPT);
        memcpy(&buf[i],ob,AES_BLOCK_SIZE);
    }
}
void st_AES_impl::decrypt(unsigned char*buf, size_t size)
{

    XTRY;
    if (!inited)
    {
        throw CommonError("st_AES not inited "+_DMI());
        //exit(0);
    }

    if (size%AES_BLOCK_SIZE!=0) throw CommonError("buf.size() mod AES_BLOCK_SIZE!=0"+_DMI());

    for (unsigned i=0; i<size; i+=AES_BLOCK_SIZE)
    {
        unsigned char ib[AES_BLOCK_SIZE];
        unsigned char ob[AES_BLOCK_SIZE];
        memset(ib,0,sizeof(ib));
        memset(ob,0,sizeof(ob));
        {
            memcpy(ib,&buf[i],AES_BLOCK_SIZE);
        }
        {
            AES_ecb_encrypt(ib,ob,&keydec,AES_DECRYPT);
        }
        memcpy(&buf[i],ob,AES_BLOCK_SIZE);
    }
    XPASS;

}

std::string st_AES_impl::decrypt(const std::string& buf)
{

    XTRY;
    if (!inited)
    {
        throw CommonError("st_AES not inited "+_DMI());
        //exit(0);
    }

    if (buf.size()%AES_BLOCK_SIZE!=0) throw CommonError("buf.size() mod AES_BLOCK_SIZE!=0"+_DMI());
    std::string out;
    for (unsigned i=0; i<buf.size(); i+=AES_BLOCK_SIZE)
    {

        unsigned char ib[AES_BLOCK_SIZE];
        unsigned char ob[AES_BLOCK_SIZE];
        memset(ib,0,sizeof(ib));
        memset(ob,0,sizeof(ob));
        memcpy(ib,&buf.data()[i],AES_BLOCK_SIZE);
        AES_ecb_encrypt(ib,ob,&keydec,AES_DECRYPT);
        out+=std::string((char*)ob,AES_BLOCK_SIZE);
    }
    if (out.size())
    {

        char n=out[out.size()-1];
        out=out.substr(0,out.size()-n);
    }
    return out;
    XPASS;
}

std::string st_AES_impl::generateRandomKey()
{

    XTRY;
    unsigned char rjbuf[AES_BLOCK_SIZE];
    if (RAND_bytes(rjbuf,AES_BLOCK_SIZE)==0)throw CommonError("RAND_bytes"+_DMI());

    std::string key=std::string((char*)rjbuf,sizeof(rjbuf));
    return key;
    XPASS;
}


st_AES_impl::~st_AES_impl() {}

C_ssl::C_ssl()
{

}

std::string C_ssl::md5(const std::string & s)
{

    unsigned char p[MD5_DIGEST_LENGTH + 10];
    ::MD5((unsigned char *)s.data(), s.size(), (unsigned char *)p);
    std::string ret((char *)p, MD5_DIGEST_LENGTH);
    return ret;

}
std::string C_ssl::sha1(const std::string & s)
{
    unsigned char p[SHA_DIGEST_LENGTH+ 10];
    ::SHA1((unsigned char *)s.data(), s.size(), (unsigned char *)p);
    std::string ret((char *)p, SHA_DIGEST_LENGTH);
    return ret;

}
std::string C_ssl::sha256(const std::string & s)
{
    unsigned char p[SHA256_DIGEST_LENGTH+ 10];
    ::SHA256((unsigned char *)s.data(), s.size(), (unsigned char *)p);
    std::string ret((char *)p, SHA256_DIGEST_LENGTH);
    return ret;

}
std::string C_ssl::sha512(const std::string & s)
{
    unsigned char p[SHA512_DIGEST_LENGTH+ 10];
    ::SHA512((unsigned char *)s.data(), s.size(), (unsigned char *)p);
    std::string ret((char *)p, SHA512_DIGEST_LENGTH);
    return ret;

}

std::string C_ssl::rand_bytes(int n)
{

    unsigned char buffer[n+20];
    RAND_bytes(buffer,n);
    return std::string((char*)buffer,n);

}
int C_ssl::rand_int()
{

    std::string s=rand_bytes(sizeof(int));
    int *z=(int*)s.data();
    return *z;
}
