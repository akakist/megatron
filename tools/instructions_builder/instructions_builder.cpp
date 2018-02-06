#include <string>
#include <string>
#include <vector>
#include <map>
#include <stdio.h>
#include <sys/stat.h>
#include "regex.h"
#include <unistd.h>
#include <set>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "regex.h"

using namespace std;
vector < string > splitTOK(const char *seps, const char *src)
{
    vector < string > res;
    char *s = strdup((char *) src);
    char *token = strtok(s, seps);
    while (token != NULL) {
        res.push_back(token);
        token = strtok(NULL, seps);
    }
    free(s);
    return res;
}
std::string load_file(const std::string & fn)
{

    std::string res;
#ifdef _MSC_VER
    struct _stat  st;
#else
    struct stat  st;
#endif
#ifdef _MSC_VER
    if (_stat(fn.c_str(), &st))
#else
    if (stat(fn.c_str(), &st))
#endif
    {
        fprintf(stderr,"cannot stat file %s\n",fn.c_str());
        //exit(1);
    }
    FILE *f=fopen(fn.c_str(), "rb");
    if (f)
    {
        unsigned char *b=(unsigned char *) malloc(st.st_size);
        if (b==NULL)
        {
            fprintf(stderr,"malloc error on file %s\n",fn.c_str());
            //exit(1);
        }
        if (fread(b,1,st.st_size,f)!=st.st_size)
        {
            fclose(f);
            free(b);
            fprintf(stderr,"fread error on file %s\n",fn.c_str());
            //exit(1);
        }
        std::string al((char *)b, st.st_size);
        fclose(f);
        free(b);
        res=al;
        return res;
    }
    fprintf(stderr,"call invalid error on file %s\n",fn.c_str());
    return "";
    //exit(1);
}


int main(int argc, char* argv[])
{
//    std::string fb=load_file(argv[1]);
    std::string fb=load_file("test.txt");
    std::vector<string> vl=splitTOK("\r\n",fb.c_str());

    bool wz=false;
    bool lft=false;
    std::string lft_doc;
    std::string wz_doc;
    for(int i=0; i<vl.size(); i++)
    {
        std::vector<std::string> vw=splitTOK(" \t",vl[i].c_str());
        bool print=true;
        if(vw.size())
        {

            if(vw[0]=="@WZ")
            {
                wz=true;
                lft=false;
                vw.erase(vw.begin());
            }
            else if(vw[0]=="@LFT")
            {
                wz=false;
                lft=true;
                vw.erase(vw.begin());
            }
            else if(vw[0]=="@ALL")
            {
                wz=true;
                lft=true;
                vw.erase(vw.begin());
            }


        }
        if(vw.size()==2)
        {
            if(vw[0]=="@@LFT")
            {
                lft_doc+="<img src='"+vw[1]+"'>\n<p>\n";
                print=false;
            }
            else if(vw[0]=="@@WZ")
            {
                wz_doc+="<img src='"+vw[1]+"'>\n<p>\n";
                print=false;

            }
        }
        if(print)
        {
            for(int x=0;x<vw.size();x++)
            {
                if(wz)
                    wz_doc+=vw[x]+" ";
                if(lft)
                    lft_doc+=vw[x]+" ";
            }
        }
    }

    std::string head=load_file("head.html");
    std::string tail=load_file("tail.html");
    wz_doc=head+wz_doc+tail;
    lft_doc=head+lft_doc+tail;
    FILE *f_wz=fopen("wz.html","wb");
    fprintf(f_wz,"%s",wz_doc.c_str());

    FILE *f_lft=fopen("lft.html","wb");
    fprintf(f_lft,"%s",lft_doc.c_str());

    fclose(f_wz);
    fclose(f_lft);


}
