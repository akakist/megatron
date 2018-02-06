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


int main()
{
    std::string fb=load_file("z");
    std::set<time_t> hrs;
    std::vector<string> vl=splitTOK("\r\n",fb.c_str());
    for(int i=0; i<vl.size(); i++)
    {
        std::vector<string> vs=splitTOK("|",vl[i].c_str());
        if(vs.size()>2)
        {
            printf("%s\n",vs[2].c_str());
            std::vector<std::string> vd=splitTOK(" :-",vs[2].c_str());
            if(vd.size()>=6)
            {
                struct tm t;
                memset(&t,0,sizeof(t));
                t.tm_year=atoi(vd[0].c_str())-1900;
                t.tm_mon=atoi(vd[1].c_str())-1;
                t.tm_mday=atoi(vd[2].c_str());
                t.tm_hour=atoi(vd[3].c_str())-1;
//		t.tm_min=atoi(vd[4].c_str())-1;
//		t.tm_sec=atoi(vd[6].c_str())-1;
                time_t tt=mktime(&t);
//		printf("%s\n",ctime(&t));
                for(time_t i=tt-2*3600; i<=tt+2*3600; i+=3600)
                {
                    hrs.insert(i);
                }
            }
        }
    }
    printf("total %d hrs\n",hrs.size());
    for(typeof(hrs.begin()) i=hrs.begin(); i!=hrs.end(); i++)
    {
        time_t t=*i;
        //    printf("-> %s\n", ctime(&t));
    }
}