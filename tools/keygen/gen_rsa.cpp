#include <stdio.h>
//#include "st_stuff.h"
//#include "str_lib.h"
//#include "mysql_extens.h"
#include <time.h>
#include <set>
#include "st_rsa.h"
//#include "st_stuff.h"
#include <vector>
#include <openssl/ssl.h>

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

int main(int argc, char **argv)
//main()
{
//    try{
    st_rsa rsa;
    rsa.generate_key(256);
    FILE* f1=fopen("rsa-pub.cpp","wb");
    FILE* f2=fopen("rsa-priv.cpp","wb");

    std::string k1=rsa.pub_key_get();
    std::string k2=rsa.priv_key_get();




    fprintf(f1,"extern \"C\" unsigned char ___rsa_public_key[];\n");
    fprintf(f2,"extern \"C\" unsigned char ___rsa_private_key[];\n");
    fprintf(f1,"extern \"C\" unsigned char ___rsa_public_key_sz;\n");
    fprintf(f2,"extern \"C\" unsigned char ___rsa_private_key_sz;\n");


    fprintf(f1,"unsigned char ___rsa_public_key[]={%s};\n",bin2c(k1).c_str());
    fprintf(f2,"unsigned char ___rsa_private_key[]={%s};\n",bin2c(k2).c_str());
    fprintf(f1,"unsigned char ___rsa_public_key_sz=%ld;\n",k1.size());
    fprintf(f2,"unsigned char ___rsa_private_key_sz=%ld;\n",k2.size());
    fclose(f1);
    fclose(f2);

//	fwrite(k1.data(),k1.size(),1,f1.f);
//	fwrite(k2.data(),k2.size(),1,f2.f);
//    }STDCATCH;

}


void st_rsa_cpp() {}
void st_rsa::init_from_pub_key()
{

    const unsigned char *p = (const unsigned char *)pub_key.data();
    rsa_xxx = d2i_RSAPublicKey(NULL, &p, pub_key.size());
    if (!rsa_xxx) {
        throw cError("i2d_RSAPublicKey");
    }
}
void st_rsa::init_from_priv_key()
{

    const unsigned char *p = (const unsigned char *)priv_key.data();
    rsa_xxx = d2i_RSAPrivateKey(NULL, &p, priv_key.size());
    if (!rsa_xxx) {
        throw cError("i2d_RSAPrivateKey");
    }
}
string st_rsa::private_key()
{

    string s;
    st_malloc ppp(10000);
    char *b = (char *) ppp.buf;
    unsigned char *p = (unsigned char *) b;
    int len = 0;
    if (rsa_xxx && p) {
        len = i2d_RSAPrivateKey(rsa_xxx, &p);
        if (len < 0)
            return "";
        string s((char *) b, len);
        return s;
    } else {
        throw cError("i2d_RSAPrivateKey");
    }
    return s;
}

string st_rsa::public_key()
{

    string s;

    st_malloc b(10000);
    unsigned char *p = (unsigned char *) b.buf;

    int len = 0;
    if (rsa_xxx && p) {
        len = i2d_RSAPublicKey(rsa_xxx, &p);
        if (len < 0)
            return "";
        string s((char *) b.buf, len);
        return s;
    } else {
        throw cError("i2d_RSAPublicKey");
    }
    return s;
}
void st_rsa::generate_key(const int b)
{

    bits = b;
    rsa_xxx = RSA_generate_key(bits, RSA_F4, NULL, NULL);
    if (!rsa_xxx)
        throw cError("RSA_generate_key");
    pub_key = public_key();

    priv_key = private_key();
}


string st_rsa::private_decrypt(const enc_string & m)
{

    return decrypt(m, RSA_private_decrypt);
}

string st_rsa::public_decrypt(const enc_string & m)
{

    return decrypt(m, RSA_public_decrypt);
}

enc_string st_rsa::public_encrypt(const string & m)
{

    return encrypt(m, RSA_public_encrypt);
}

enc_string st_rsa::private_encrypt(const string & m)
{

    return encrypt(m, RSA_private_encrypt);
}

enc_string st_rsa::encrypt(const string & ms, int (*func) (int, const unsigned char *, unsigned char *, RSA *, int))
{
    int rsize = size();
    string mm = Base64Encode(ms);
    st_malloc vp(mm.size() + rsize);
    char *p = (char *) vp.buf;
    DUMP("new");
    memcpy(p, mm.data(), mm.size());
    int i;
    if(!rsa_xxx) throw cError("!rsa_xxx");
    for (i = 0; i < mm.size(); i += rsize) {
        func(rsize, (unsigned char *) &p[i], (unsigned char *) &p[i], rsa_xxx, RSA_NO_PADDING);
    }
    enc_string out;
    out.original_size = mm.size();
    out.buf = string((char *) p, i);
    return out;
}

string st_rsa::decrypt(const enc_string & m, int (*func) (int, const unsigned char *, unsigned char *, RSA *, int))
{

    st_malloc vp(m.buf.size() + 1000);
    {
        char *p = (char *) vp.buf;
        {
            memcpy(p, m.buf.data(), m.buf.size());
            {
                int rsize = size();

                if(!rsa_xxx) throw cError("!rsa_xxx");
                {
                    for (int i = 0; i < m.buf.size(); i += rsize) {
                        func(rsize, (unsigned char *) &p[i], (unsigned char *) &p[i], rsa_xxx, RSA_NO_PADDING);
                    }

                    string out(p, m.original_size);
                    string out1 = Base64Decode(out);
                    return out1;
                }
            }
        }
    }
    return "";
}
st_rsa::st_rsa():Mutex("st_rsa")
{
    rsa_xxx = NULL;
}
st_rsa::~st_rsa()
{

    if (rsa_xxx)
        RSA_free(rsa_xxx);
}
int st_rsa::size()
{

    if(!rsa_xxx) throw cError("!rsa_xxx");
    return RSA_size(rsa_xxx);
}
string st_rsa::pub_key_get()
{

    return pub_key;
}
void st_rsa::pub_key_set(const string &s)
{

    pub_key=s;
}
string st_rsa::priv_key_get()
{

    return priv_key;
}
void st_rsa::priv_key_set(const string &s)
{

    priv_key=s;
}

out_oscar_buffer & operator<< (out_oscar_buffer& b,const enc_string &s)
{
    b<<s.original_size<<s.buf;
    return b;

}
oscar_buffer & operator>> (oscar_buffer& b,  enc_string &s)
{
    b>>s.original_size>>s.buf;
    return b;
}
st_malloc::st_malloc(unsigned long size)
{
    buf = malloc(size + 1);
    if(buf==NULL)throw cError("st_malloc failed");
    DUMP("malloc");
}

st_malloc::~st_malloc()
{
    free(buf);
    DUMP("free");
}

