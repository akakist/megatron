#include "st_rsa_impl.h"
#include "IUtils.h"
#include "configDB.h"
#ifdef _WIN32
#include <malloc.h>
#endif
#include "mutexInspector.h"
#include "st_rsa.h"
#include "st_malloc.h"

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

//    unsigned long   e = RSA_F4;
    int             ret = 0;
    BIGNUM          *bne = NULL;
    bne = BN_new();
    ret = BN_set_word(bne,RSA_F4);
    if(ret != 1) {
        BN_free(bne);
        throw CommonError("BN_set_word");
    }

    rsa_xxx = RSA_new();
    ret = RSA_generate_key_ex(rsa_xxx, bits, bne, NULL);
    if(ret != 1) {
        BN_free(bne);
        auto err=ERR_get_error();
        throw CommonError("RSA_generate_key_ex %d",err);
    }
    BN_free(bne);


    XPASS;
}

void st_rsa_impl::initFromPublicKey(const std::string &priv_key)
{

    XTRY;
    const unsigned char * p = ( const unsigned char *)priv_key.data();
    rsa_xxx = d2i_RSA_PUBKEY(&rsa_xxx, &p, static_cast<long>(priv_key.size()));
    if (!rsa_xxx)
    {
        throw CommonError("d2i_RSAPublicKey %s",_DMI().c_str());
    }
    XPASS;;
}
int st_rsa_impl::initFromPublicKey_int(const std::string &priv_key)
{

    XTRY;
    const unsigned char * p = ( const unsigned char *)priv_key.data();
    rsa_xxx = d2i_RSA_PUBKEY(&rsa_xxx, &p, static_cast<long>(priv_key.size()));
    if (!rsa_xxx)
    {
        return 1;
        throw CommonError("d2i_RSAPublicKey %s",_DMI().c_str());
    }
    XPASS;;
    return 0;
}


void st_rsa_impl::initFromPrivateKey(const std::string &priv_key)
{

    XTRY;
    const unsigned char *p = (const unsigned char *)priv_key.data();
    rsa_xxx = d2i_RSAPrivateKey(NULL, &p, static_cast<long>(priv_key.size()));
    if (!rsa_xxx)
    {
        throw CommonError("d2i_RSAPrivateKey"+_DMI());
    }

    XPASS;
}
int st_rsa_impl::initFromPrivateKey_int(const std::string &priv_key)
{

    XTRY;
    const unsigned char *p = (const unsigned char *)priv_key.data();
    rsa_xxx = d2i_RSAPrivateKey(NULL, &p, static_cast<long>(priv_key.size()));
    if (!rsa_xxx)
    {
        return 1;
        throw CommonError("d2i_RSAPrivateKey"+_DMI());
    }

    XPASS;
    return 0;
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
int st_rsa_impl::privateDecrypt(const std::string & m,std::string& out)
{

    XTRY;

    if (!rsa_xxx) throw CommonError("!rsa_xxx");
    return decrypt(m,out, RSA_private_decrypt);
    XPASS;
}
std::string st_rsa_impl::publicDecrypt(const std::string & m)
{

    XTRY;

    if (!rsa_xxx) throw CommonError("!rsa_xxx");
    return decrypt(m, RSA_public_decrypt);
    XPASS;
}
int st_rsa_impl::publicDecrypt(const std::string & m,std::string& out)
{

    XTRY;

    if (!rsa_xxx) throw CommonError("!rsa_xxx");
    return decrypt(m,out, RSA_public_decrypt);
    XPASS;
}

std::string st_rsa_impl::publicEncrypt(const std::string & m)
{

    XTRY;

    if (!rsa_xxx) throw CommonError("!rsa_xxx");
    return encrypt(m, RSA_public_encrypt);
    XPASS;
}
int st_rsa_impl::publicEncrypt(const std::string & m,std::string& out)
{

    XTRY;

    if (!rsa_xxx) throw CommonError("!rsa_xxx");
    return encrypt(m,out, RSA_public_encrypt);
    XPASS;
}

std::string st_rsa_impl::privateEncrypt(const std::string & m)
{

    XTRY;

    if (!rsa_xxx) throw CommonError("!rsa_xxx");
    return encrypt(m, RSA_private_encrypt);
    XPASS;
}
int st_rsa_impl::privateEncrypt(const std::string & m,std::string& out)
{

    XTRY;

    if (!rsa_xxx) throw CommonError("!rsa_xxx");
    return encrypt(m, out, RSA_private_encrypt);
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
int st_rsa_impl::encrypt(const std::string & ms,std::string&out, int (*func) (int, const unsigned char *, unsigned char *, RSA *, int))
{

    XTRY;

    if (!rsa_xxx) throw CommonError("!rsa_xxx");
    int rsize = size()-RSA_PKCS1_PADDING_SIZE;
//    std::string out;
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
            return 1;
            throw CommonError("rsa error %s",errbuf);
        }
        out+=std::string((char*)outBuf,ret);
    }
//    return out;
    return 0;
    XPASS;
}

std::string st_rsa_impl::decrypt(const std::string & m, int (*func) (int, const unsigned char *, unsigned char *, RSA *, int))
{

    XTRY;
    if (!rsa_xxx) throw CommonError("!rsa_xxx");
    if (m.size()%size()!=0)  throw CommonError("rsa: invalid buffer size buffer size %d, rsa size %d %s",m.size(),this->size(),_DMI().c_str());

    int rsize = size();
    char inBuf[size()];
    char outBuf[size()];
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
            throw CommonError("rsa error %s",errbuf);
        }
        out+=std::string((char*)outBuf,ret);
    }
    return out;
    XPASS;
}
int st_rsa_impl::decrypt(const std::string & m, std::string&out, int (*func) (int, const unsigned char *, unsigned char *, RSA *, int))
{

    XTRY;
    if (!rsa_xxx) throw CommonError("!rsa_xxx");
    if (m.size()%size()!=0)  throw CommonError("rsa: invalid buffer size buffer size %d, rsa size %d %s",m.size(),this->size(),_DMI().c_str());

    int rsize = size();
    char inBuf[size()];
    char outBuf[size()];
//    std::string out;

    size_t i=0;
    for (i = 0; i < m.size(); i += rsize)
    {
        memcpy(inBuf, (unsigned char *) &m.data()[i],rsize);
        int ret=func(rsize, (unsigned char *)inBuf, (unsigned char *)outBuf, rsa_xxx, RSA_PKCS1_PADDING);
        if (ret==-1)
        {
            char errbuf[1024];
            ERR_error_string_n(ERR_get_error(),errbuf,sizeof(errbuf));
            return 1;
            throw CommonError("rsa error %s",errbuf);
        }
        out+=std::string((char*)outBuf,ret);
    }
//    return out;
    return 0;
    XPASS;
}

st_rsa_impl::st_rsa_impl()
{
    rsa_xxx = nullptr;

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
    if(key.empty())
        throw CommonError("password size == 0");
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

void _AES_ecb_encryptD(const unsigned char *in, unsigned char *out,
                       const AES_KEY *key, const int enc)
{
    AES_ecb_encrypt(in,out, key, enc);
}
void _AES_ecb_encryptE(const unsigned char *in, unsigned char *out,
                       const AES_KEY *key, const int enc)
{
    AES_ecb_encrypt(in,out, key, enc);
}

REF_getter<refbuffer> st_AES_impl::encrypt(const REF_getter<refbuffer> &buf_)
{

    XTRY;
    if (!inited)
    {
        throw CommonError("st_AES not inited "+_DMI());
    }


    const REF_getter<refbuffer>& buf=buf_;
    int l=0x10;
    if (buf->size_%AES_BLOCK_SIZE!=0)
    {
        l=AES_BLOCK_SIZE - (buf->size_%AES_BLOCK_SIZE);
    }
    std::string addl;
    for (int i=0; i<l; i++)
    {
        addl+=l;
    }
    if(buf->capacity - buf->size_ >= addl.size())
    {
        memcpy(&buf->buffer[buf->size_],addl.data(),addl.size());
        buf->size_+=addl.size();
    }
    else
        throw CommonError("if(buf->capacity - buf->size >= addl.size())");

    REF_getter<refbuffer> o=new refbuffer;
    o->buffer=(uint8_t*)malloc(buf->size_);
    if(!o->buffer)
        throw std::runtime_error("alloc error");
    o->size_=buf->size_;
    o->capacity=buf->size_;
    //st_malloc m(buf.size());
    //uint8_t *p=(uint8_t *)o->buffer;
    memset(o->buffer,0,o->size_);
    for (unsigned i=0; i<buf->size_; i+=AES_BLOCK_SIZE)
    {
        unsigned char *ib=(uint8_t*)buf->buffer+i;
        unsigned char *ob=o->buffer+i;
        _AES_ecb_encryptE(ib,ob,&keyenc,AES_ENCRYPT);
    }
    return o;
    XPASS;
}

REF_getter<refbuffer> st_AES_impl::decrypt(const REF_getter<refbuffer> &buf)
{

    XTRY;
    if (!inited)
    {
        throw CommonError("st_AES not inited "+_DMI());
    }


    if (buf->size_%AES_BLOCK_SIZE!=0) throw CommonError("buf.size() mod AES_BLOCK_SIZE!=0"+_DMI());


    REF_getter<refbuffer> o=new refbuffer;

    o->buffer=(uint8_t*)malloc(buf->size_);
    if(!o->buffer)
        throw std::runtime_error("alloc error");
    o->size_=buf->size_;
    o->capacity=buf->size_;

    for (unsigned i=0; i<buf->size_; i+=AES_BLOCK_SIZE)
    {

        unsigned char *ib=buf->buffer+i;
        unsigned char *ob=o->buffer+i;
        _AES_ecb_encryptD(ib,ob,&keydec,AES_DECRYPT);
    }
    if (buf->size_)
    {

        char n=o->buffer[o->size_-1];
        o->size_-=n;
        return o;
    }
    throw CommonError("empty AES buffer");
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
void C_ssl::rand_bytes(uint8_t* p, int n)
{
    RAND_bytes(p,n);
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
