#include <string>
#include <vector>
#include <QString>
#include "st_rsa_impl.h"
#include "commonError.h"
#include <sys/timeb.h>
//#include "../../lib/jsoncpp/include/json/json.h"

uint64_t getNow()
{

    uint64_t now;
#ifdef __linux__
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
    Integer v;
    v.set(now);
    return v;
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

int main(int argc, char** argv)
{
    Json::Value j;
//    st_rsa_impl s;

//    s.generate_key(512);
    if(argc!=3)
    {
        printf("usage %s bits comment\n",argv[0]);
    }
    int bits=atoi(argv[1]);
    std::string comment=argv[2];
    st_rsa_impl rsa;
    rsa.generate_key(bits);
    time_t t=time(NULL);
    std::string fix=QString("%1_%2_%3").arg(comment.c_str()).arg(bits).arg(t).toStdString();

    std::string pub_h_fn="rsa-pub_"+fix+".h";
    std::string priv_h_fn="rsa-priv_"+fix+".h";

    std::string k_pub=rsa.getPublicKey();
    std::string k_priv=rsa.getPrivateKey();



    char h_pub[100000];
    snprintf(h_pub,sizeof(h_pub),
             "#ifndef _________%spub__H\n"
             "#define _________%spub__H\n"
             "static unsigned char ___rsa_public_key_%s[]={%s};\n"
             "static unsigned int ___rsa_public_key_%s_sz=%ld;\n"
             "#endif\n",fix.c_str(),fix.c_str(),
             fix.c_str(),bin2c(k_pub).c_str(),
             fix.c_str(),k_pub.size()
            );
    char h_priv[100000];
    snprintf(h_priv,sizeof(h_priv),
             "#ifndef _________%spriv__H\n"
             "#define _________%spriv__H\n"
             "static unsigned char ___rsa_private_key_%s[]={%s};\n"
             "static unsigned int ___rsa_private_key_%s_sz=%ld;\n"
             "#endif\n",fix.c_str(),fix.c_str(),
             fix.c_str(),bin2c(k_priv).c_str(),
             fix.c_str(),k_priv.size()
            );

    FILE* f1=fopen(pub_h_fn.c_str(),"wb");
    FILE* f2=fopen(priv_h_fn.c_str(),"wb");
    std::string dd1=Base64Encode(k_priv);
    std::string dd2=Base64Encode(k_pub);
    fputs(h_pub,f1);
    fputs(h_priv,f2);



    fclose(f1);
    fclose(f2);
    printf("keygen done\n%s\n%s\n",dd1.c_str(),dd2.c_str());


    char s[500];
    for(int i=0;i<sizeof(s);i++)
        s[i]=rand();
    std::string ss=std::string(s,sizeof(s));
    st_rsa_impl r;
    r.initFromPrivateKey(k_priv);

    int nc=20000;
    std::string enc=r.privateEncrypt(ss);

    //time_t start=time(NULL);
    uint64_t start=getNow();
    for(int i=0;i<nc;i++)
    {
        //if(i%100==0)
          //  printf("%d\n",time(NULL));
        std::string dec=r.publicDecrypt(enc);
        if(dec!=ss)
        {
            printf("failed\n");
        }
    }
    uint64_t end=getNow();
    printf("end %ld\n",end-start);
    printf("1000/sec %f\n",float(end-start)/1000000.0/float(nc/1000));


}
