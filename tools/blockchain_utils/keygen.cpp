#include <string>
#include <vector>
#include <QString>
#include "st_rsa_impl.h"
#include "commonError.h"
#include <sys/timeb.h>
#include "json/json.h"

uint64_t getNow()
{

    uint64_t now;
#ifndef _WIN32
    timespec tstmp;
    if(clock_gettime(CLOCK_REALTIME,&tstmp))
    {
        throw CommonError("clock_gettime: errno %d",errno);
    }
    uint64_t _1=tstmp.tv_sec;
    _1*=1000000;
    uint64_t _2=tstmp.tv_nsec;
    _2/=1000;

    now=_1;
    now+=_2;
    //Integer v;
    //v.set(now);
    return now;
#else
    timeb tb;
    ftime(&tb);
    uint64_t add=tb.millitm;
    add*=1000;
    now=tb.time;
    now*=1000000;
    now+=add;
    return now;
#endif

}

static char    six2pr[64] =
{
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
};

static unsigned char pr2six[256];


/*--- function HTUU_encode -----------------------------------------------
 *
 *   Encode a single line of binary data to a standard format that
 *   uses only printing ASCII characters (but takes up 33% more bytes).
 *
 *    Entry    bufin    points to a buffer of bytes.  If nbytes is not
 *                      a multiple of three, then the byte just beyond
 *                      the last byte in the buffer must be 0.
 *             nbytes   is the number of bytes in that buffer.
 *                      This cannot be more than 48.
 *             bufcoded points to an output buffer.  Be sure that this
 *                      can hold at least 1 + (4*nbytes)/3 characters.
 *
 *    Exit     bufcoded contains the coded line.  The first 4*nbytes/3 bytes
 *                      contain printing ASCII characters representing
 *                      those binary bytes. This may include one or
 *                      two '=' characters used as padding at the end.
 *                      The last byte is a zero byte.
 *             Returns the number of ASCII characters in "bufcoded".
 */
static int
Ns_HtuuEncode(unsigned char *bufin, unsigned int nbytes, char * bufcoded)
{

    /* ENC is the basic 1 character encoding function to make a char printing */
#define ENC(c) six2pr[c]

    char  *outptr = bufcoded;
    unsigned int    i;

    for (i = 0; i < nbytes; i += 3)
    {
        /* c1 */
        *(outptr++) = ENC(*bufin >> 2);
        /* c2 */
        *(outptr++) = ENC(((*bufin << 4) & 060) | ((bufin[1] >> 4) & 017));
        /* c3 */
        *(outptr++) = ENC(((bufin[1] << 2) & 074) | ((bufin[2] >> 6) & 03));
        /* c4 */
        *(outptr++) = ENC(bufin[2] & 077);

        bufin += 3;
    }

    /*
     * If nbytes was not a multiple of 3, then we have encoded too many
     * characters.  Adjust appropriately.
     */
    if (i == nbytes + 1)
    {
        /* There were only 2 bytes in that last group */
        outptr[-1] = '=';
    }
    else if (i == nbytes + 2)
    {
        /* There was only 1 byte in that last group */
        outptr[-1] = '=';
        outptr[-2] = '=';
    }
    *outptr = '\0';
    return (outptr - bufcoded);
}

/*--- function HTUU_decode ------------------------------------------------
 *
 *  Decode an ASCII-encoded buffer back to its original binary form.
 *
 *    Entry    bufcoded    points to a uuencoded string.  It is
 *                         terminated by any character not in
 *                         the printable character table six2pr, but
 *                         leading whitespace is stripped.
 *             bufplain    points to the output buffer; must be big
 *                         enough to hold the decoded std::string (generally
 *                         shorter than the encoded string) plus
 *                         as many as two extra bytes used during
 *                         the decoding process.
 *             outbufsize  is the maximum number of bytes that
 *                         can fit in bufplain.
 *
 *    Exit     Returns the number of binary bytes decoded.
 *             bufplain    contains these bytes.
 */
static int
Ns_HtuuDecode(char * bufcoded, unsigned char * bufplain, int outbufsize)
{

    /* single character decode */
#define DEC(c) pr2six[(int)c]
#define MAXVAL 63

    static int      first = 1;

    int             nbytesdecoded, j;
     char  *bufin = bufcoded;
     unsigned char *bufout = bufplain;
     int    nprbytes;

    /*
     * If this is the first call, initialize the mapping table. This code
     * should work even on non-ASCII machines.
     */
    if (first)
    {
        first = 0;
        for (j = 0; j < 256; j++)
            pr2six[j] = MAXVAL + 1;

        for (j = 0; j < 64; j++)
            pr2six[(int) six2pr[j]] = (unsigned char) j;
    }

    /* Strip leading whitespace. */

    while (*bufcoded == ' ' || *bufcoded == '\t')
        bufcoded++;

    /*
     * Figure out how many characters are in the input buffer. If this would
     * decode into more bytes than would fit into the output buffer, adjust
     * the number of input bytes downwards.
     */
    bufin = bufcoded;
    while (pr2six[(int) *(bufin++)] <= MAXVAL) {};
    nprbytes = bufin - bufcoded - 1;
    nbytesdecoded = ((nprbytes + 3) / 4) * 3;
    if (nbytesdecoded > outbufsize)
    {
        nprbytes = (outbufsize * 4) / 3;
    }
    bufin = bufcoded;

    while (nprbytes > 0)
    {
        *(bufout++) = (unsigned char) (DEC(*bufin) << 2 | DEC(bufin[1]) >> 4);
        *(bufout++) = (unsigned char) (DEC(bufin[1]) << 4 |
                                       DEC(bufin[2]) >> 2);
        *(bufout++) = (unsigned char) (DEC(bufin[2]) << 6 | DEC(bufin[3]));
        bufin += 4;
        nprbytes -= 4;
    }

    if (nprbytes & 03)
    {
        if (pr2six[(int) bufin[-2]] > MAXVAL)
        {
            nbytesdecoded -= 2;
        }
        else
        {
            nbytesdecoded -= 1;
        }
    }
    return (nbytesdecoded);
}


std::string Base64Encode(const std::string &str)
{
    size_t len=str.size()*2;
    char *out=(char*)malloc(len+10);
    ::memset(out,0,len+2);
    int outlen=Ns_HtuuEncode((unsigned char*)str.data(),str.size(),out);
    std::string ret(out,outlen);
    free(out);
    return ret;
}
std::string Base64Decode(const std::string &str)
{

    if(str.size()==0) return "";
    size_t len=str.size();
    char *out=(char*)malloc(len+10);
    if(!out)
        throw CommonError("!out size %d %s %d",str.size(),__FILE__,__LINE__);
    ::memset(out,0,len+2);
    int outlen=Ns_HtuuDecode((char*)str.data(),(unsigned char*)out,len);
    std::string ret(out,outlen);
    free(out);
    return ret;
}
std::string bin2hex(const std::string & in)
{
    std::string out = "";
    const unsigned char *p = (unsigned char *)in.data();
    for (unsigned int i = 0; i < in.size(); i++)
    {
        char s[40];
        ::snprintf(s, sizeof(s) - 1, "%02x", p[i]);
        out += s;
    }
    return out;
}
std::string hex2bin(const std::string &s)
{
    std::string out="";
    char *p=(char*)s.data();
    size_t sz=s.size();
    for (size_t i=0; i<sz; i+=2)
    {
        char ss[3]= {0};
        ::memcpy(ss,&p[i],2);
        unsigned char c=(unsigned char)strtol(ss,NULL,16);
        out+=std::string((char*)&c,1);
    }
    return out;
}

std::string join(const char* pattern,const std::vector<std::string>& arr)
{

    std::string ret;
    if (arr.size() > 1)
    {
        unsigned int i;
        for (i = 0; i < arr.size() - 1; i++)
            ret += arr[i] + pattern;
        ret += arr[arr.size() - 1];
    }
    else if (arr.size() == 1)
    {
        ret += arr[0];
    }
    return ret;

}

static std::string bin2c(const std::string & in)
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
struct BC_UID
{
private:
    std::string buf;
public:
    std::string toHex() const
    {
        return bin2hex(buf);
    }
    std::string toBase64() const
    {
        return Base64Encode(buf);
    }
    void fromHex(const std::string& hex)
    {
        buf=hex2bin(hex);
    }
    void fromBase64(const std::string& b64)
    {
        buf=Base64Decode(b64);
    }
    void fromBin(const std::string& bin)
    {
        buf=bin;
    }
};

std::string createDocumentId(const BC_UID& uid)
{
    char s[200];
    snprintf(s,sizeof(s),"%016lx",getNow());
    std::string documentId=uid.toHex()+s;
    return documentId;

}
struct attachment
{
    std::string url;
    std::string md5_content_of_url;
    std::string comments;
};
Json::Value makeEmission(const std::string& creditor_uid, const std::string &debetor_uid, double amount, const std::string & comments, const std::vector<attachment>& attachments)
{
    Json::Value j;
    j["blockType"]="emission";
    j["debetor_uid:hex"]=bin2hex(debetor_uid);
    j["creditor_uid:hex"]=bin2hex(creditor_uid);
    j["amount"]=amount;
    j["comments"]=comments;
    for(auto& z: attachments)
    {
        Json::Value x;
        x["comments"]=z.comments;
        x["md5_content_of_url:hex"]=bin2hex(z.md5_content_of_url);
        x["url"]=z.url;
        j["attachments"].append(x);
    }


}
//void makeInvoice(const std::string)
Json::Value createDocument(const Json::Value &j,const st_rsa_impl& rsa)
{
    Json::FastWriter w;
    std::string buf=w.write(j);
    Json::Value out;
    out["document"]=j;
    out["document:Json.FastWrite:sha512:rsa.privateEncrypt:hex"]=bin2hex(rsa.privateEncrypt(C_ssl::sha512(buf)));
    return out;

}
Json::Value makeInvoice(const BC_UID& receiver, const BC_UID& payer, const std::string & comment, const std::vector<attachment>& atttachments)
{

}
Json::Value makeRegistration(const std::string& firstName, const std::string &middleName, const std::string& lastName, const std::string& address, const std::string& keyStorePassword)
{
    Json::Value j;
    j["blockType"]="registration";
    j["lastName"]=lastName;
    j["firstName"]=firstName;
    j["middleName"]=middleName;
    j["address"]=address;
    st_rsa_impl rsa;
    rsa.generate_key(1024);
    std::string pubkey=rsa.getPublicKey();
    j["pub_key:hex"]=bin2hex(pubkey);
    BC_UID uid;
    uid.fromBin(C_ssl::md5(pubkey));
    j["uid:hex"]=uid.toHex();
    std::string documentId=createDocumentId(uid);
    j["documentId:hex"]=documentId;

    Json::Value out=createDocument(j,rsa);

    {

        Json::Value j;
        j["lastName"]=lastName;
        j["firstName"]=firstName;
        j["middleName"]=middleName;
        j["address"]=address;
        std::string uid=C_ssl::md5(pubkey);
        j["uid:hex"]=bin2hex(uid);

        Json::Value jkeyStruct;


        std::string priv_key=rsa.getPrivateKey();
        std::string password=keyStorePassword;
        st_AES_impl aes;
        aes.init(password);
        jkeyStruct["priv_key:aes:hex"]=bin2hex(aes.encrypt(priv_key));

        jkeyStruct["priv_key:md5:hex"]=bin2hex(C_ssl::md5(priv_key));
        jkeyStruct["pub_key:hex"]=bin2hex(pubkey);

        jkeyStruct["registration"]=j;

        jkeyStruct["blockType"]="private keystore";


        //printf("%s\n",jkeyStruct.toStyledString().c_str());
        std::string fn=bin2hex(uid)+".bks";
        FILE *f=fopen(fn.c_str(),"wb");
        fprintf(f,"%s",jkeyStruct.toStyledString().c_str());
        fclose(f);
    }

    return out;
}

int main(int argc, char** argv)
{
    if(argc>1)
    {
        if(std::string(argv[1])=="-r");
        Json::Value z=makeRegistration("John","Theodor","Miller","ave  29","gaga1234");

        printf("%s\n",z.toStyledString().c_str());
    }

    printf("now %016lx\n",getNow());
    printf("now %016lx\n",getNow());
    printf("now %016lx\n",getNow());
    printf("now %lx\n",getNow());
    printf("now %lx\n",getNow());
    printf("now %lx\n",getNow());
    printf("now %lx\n",getNow());


}
