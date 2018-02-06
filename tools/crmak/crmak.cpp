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
//#include "str_lib.h"

#if defined(__arm__)
#define CPUTYPE "arm"
#elif defined(__avr32__)
#define CPUTYPE "avr32"
#elif defined(__i386__)
#define CPUTYPE "i386"
#elif defined(__ia64__)
#define CPUTYPE "ia64"
#elif defined(__hppa__)
#define CPUTYPE "hppa"
#elif defined(__powerpc64__)
#define CPUTYPE "powerpc64"
#elif defined(__mips__)
#elif defined(__powerpc__)
#define CPUTYPE "powerpc"
#elif defined(__s390__)
#define CPUTYPE "s390"
#elif defined(__sparc__)
#define CPUTYPE "sparc"
#elif defined(__x86_64__)
#define CPUTYPE "x86-64"
#elif defined(__alpha__)
#define CPUTYPE "alpha"
#elif defined(__sh__)
#define CPUTYPE "sh"
#else
//#error "Unknown CPU architecture."
#endif

#if defined(__linux__)
#define OSTYPE "linux"
#elif defined(__FreeBSD__)
#define OSTYPE "FreeBSD"
#else
//#error "Unknown OS"
#endif
#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)

#if defined(__GNUC__)
#if defined(__GNUC_PATCHLEVEL__)
#define COMPILER CPUTYPE "_" "GCC" " " STRINGIFY(__GNUC__) "_" STRINGIFY(__GNUC_MINOR__) "_" STRINGIFY(__GNUC_PATCHLEVEL__)
#else
#define COMPILER CPUTYPE "_" "GCC" " " STRINGIFY(__GNUC__) "_" STRINGIFY(__GNUC_MINOR__)
#endif
#else
#define COMPILER "Unknown"
#endif

std::string load_file(const std::string & fn);
void processSources( FILE* fm);

using namespace std;
set<string> m_includes;
set<string> targets;
set<string> m_sources;
map <string, string> conf;
std::string m_cflags_include;
vector<string> m_outD,m_outR;
map <string,string> m_srcFiles;
vector<string> CC;
vector<string> C;
set < string > splitTOKs(const char *seps, const char *src);
vector < string > splitTOK(const char *seps, const char *src);
string find_srcfile(const string& s);

vector < string > splitTOK(const char *seps, const char *src);
string replace_vals(const string& regex,const string& repl, const string &src);
bool is_file_exists(const string &pathname);
string join(const string &pattern, const vector<string> & arr);
string join(const char* pattern, const vector<string> & arr);
void check_path_wfn(const string & _pathname);
std::string ext(const std::string& fn)
{
    vector<string>v=splitTOK(".",fn.c_str());
    if(v.size()) return v[v.size()-1];
    return "";
}
std::string itostring(int i)
{

    char ss[200];
    std::string s;
    ::snprintf(ss, sizeof(ss) - 1, "%d", i);
    s = ss;
    return s;

}
std::string rmSlash(const std::string& s)
{
    std::string out;
    for(size_t i=0; i<s.size(); i++)
    {
        if(s[i]=='/' || s[i]=='.')
            out+="_";
        else out+=s[i];
    }
    return out;
}



void read_config(char*in_fname, FILE* fmake)
{
    FILE *f = fopen(in_fname, "rb");
    char s2[100000];
    string line;
    vector<string> lines;
    while(!feof(f))
    {
        char *str=fgets(s2,sizeof(s2),f);
        if(str)
        {
            for(size_t i=0; i<strlen(str); i++)
            {
                if(str[i]==13)str[i]=0;
                if(str[i]==10)str[i]=0;
            }
            if(strlen(str))
            {
                if(str[strlen(str)-1]=='\\')
                {
                    str[strlen(str)-1]=0;
                    line+=str;
                    //printf("\n%s\n",line.c_str());
                }
                else
                {
                    line+=str;
                    lines.push_back(line);
                    line="";
                }
            }
            else
            {
                lines.push_back(line);
                line=str;
            }
        }
    }
    lines.push_back(line);

    for(size_t i=0; i<lines.size(); i++)
    {

        if(lines[i].size())
        {
            if(lines[i][0]=='@')
            {
                vector<string> v;
                int pos=lines[i].find("=",0);
                if(pos==-1)continue;
                v.push_back(lines[i].substr(0,pos));
                v.push_back(lines[i].substr(pos+1,lines[i].size()));

                if(v.size()==2)
                {
                    if(v[0]=="@CC")
                    {
                        CC=splitTOK(" \t",v[1].c_str());
                        char s[22000];
                        for(size_t i=0; i<CC.size(); i++)
                        {
                            snprintf(s,sizeof(s),"%s -dumpmachine > x.dumpmachine",CC[i].c_str());
                            system(s);
                            string cpu=load_file("x.dumpmachine");
                            unlink("x.dumpmachine");
                            fprintf(fmake,"\nCPU%d=%s\nCC%d=%s\n",i,cpu.c_str(),i,CC[i].c_str());

                        }
                    }
                    else  if(v[0]=="@C")
                    {
                        C=splitTOK(" \t",v[1].c_str());
                        for(size_t i=0; i<C.size(); i++)
                        {
                            fprintf(fmake,"\nC%d=%s\n",i,C[i].c_str());

                        }
                    }
                    else if(v[0]=="@SRC")
                    {
                        m_sources=splitTOKs(" \t",v[1].c_str());
                        //for(size_t i=0;i<CC.size();i++)
                        processSources(fmake);
                    }
                    else if(v[0]=="@INCLUDES")
                    {
                        printf("\n@INCLUDES %s %d\n",__FILE__,__LINE__);
                        m_includes=splitTOKs(" \t",v[1].c_str());
                        for (set<string>::iterator i=m_includes.begin(); i!=m_includes.end(); i++)
                        {
                            m_cflags_include+=" -I"+*i;
                        }
                        fprintf(fmake,"INCLUDES=%s\n",m_cflags_include.c_str());
                    }
                    else if(v[0]=="@OBJD")
                    {

                        m_outD=splitTOK(" \t,:",v[1].c_str());
                        //printf("m_outD seted %s",m_outD.c_str());
                        for(size_t i=0; i<m_outD.size(); i++)
                        {
                            char s[300];
#ifdef WIN32
                            FILE *tmp=fopen("mkk","wb");
                            fprintf(tmp,"mkdir -p %s",m_outD[i].c_str());
                            fclose(tmp);
                            system("sh mkk");
#else
                            snprintf(s,sizeof(s),"mkdir -p %s",m_outD[i].c_str());
                            system(s);
#endif
                            fprintf(fmake,"\nOBJD%d=%s\n",i,m_outD[i].c_str());
                            fprintf(fmake,"\n%s:\n\tmkdir -p %s\n\n",m_outD[i].c_str(),m_outD[i].c_str());
                        }
                    }
                    else if(v[0]=="@OBJR")
                    {
                        m_outR=splitTOK(" \t,:",v[1].c_str());
                        //printf("m_outD seted %s",m_outD.c_str());
                        for(size_t i=0; i<m_outR.size(); i++)
                        {
                            char s[300];
#ifdef WIN32
                            FILE *tmp=fopen("mkk","wb");
                            fprintf(tmp,"mkdir -p %s",m_outR[i].c_str());
                            fclose(tmp);
                            system("sh mkk");
#else
                            snprintf(s,sizeof(s),"mkdir -p %s",m_outR[i].c_str());
                            system(s);
#endif
                            fprintf(fmake,"\nOBJR%d=%s\n",i,m_outR[i].c_str());
                            fprintf(fmake,"\n%s:\n\tmkdir -p %s\n\n",m_outR[i].c_str(),m_outR[i].c_str());
                        }
                    }
                    else if(v[0]=="@MAIN")
                    {
                        fprintf(fmake,"\nMAIN=%s\n",v[1].c_str());
                    }
                }
            }
            else
            {
                fprintf(fmake,"%s\n",lines[i].c_str());
                continue;
            }
        }
        else
        {
            fprintf(fmake,"\n");
            continue;
        }
    }
    bool err=false;
    for(set<string>::iterator i=m_sources.begin(); i!=m_sources.end(); i++)
    {
        if(i->size()<3)continue;
        string src=find_srcfile(*i);
        if(src.size()==0)
        {
            fprintf(stderr,"ERROR file %s not found\n",i->c_str());
            err=true;
        }
    }
    if(err) exit(1);


}

string find_srcfile(const string& s)
{
    if(m_srcFiles.find(s)!=m_srcFiles.end())
        return m_srcFiles[s];

    for(set<string>::iterator i=m_includes.begin(); i!=m_includes.end(); i++)
    {
        string pn=*i+"/"+s;
        struct stat st;
        if(!stat(pn.c_str(),&st))
        {
            m_srcFiles[s]=pn;
            return pn;
        }
        else {}
    }
    return "";
}
string dependNameDebug(int compiler,const string &s)
{
    if(compiler>=m_outD.size())
    {
        printf("error: if(compiler>=m_outD.size())\n");
        exit(1);
    }
    return m_outD[compiler]+"/"+rmSlash(s)+"."+itostring(compiler)+".d";
}

string dependNameRelease(int compiler,const string &s)
{
    if(compiler>=m_outR.size())
    {
        printf("error: if(compiler>=m_outR.size())\n");
        exit(1);
    }
    return m_outR[compiler]+"/"+rmSlash(s)+"."+itostring(compiler)+".d";
}

string objNameDebug(int compiler,const string &s)
{
    if(compiler>=m_outD.size())
    {
        printf("error: if(compiler>=m_outD.size())\n");
        exit(1);
    }
    return m_outD[compiler]+"/"+rmSlash(s)+"."+itostring(compiler)+".o";
}
string objNameRelease(int compiler,const string &s)
{
    if(compiler>=m_outR.size())
    {
        printf("error: if(compiler>=m_outR.size())\n");
        exit(1);
    }
    return m_outR[compiler]+"/"+rmSlash(s)+"."+itostring(compiler)+".o";
}
void processSources(FILE* fm)
{
    for(int I=0; I<CC.size(); I++)
    {
        std::map<int,std::string> r_obj,d_obj;
        for(std::set<std::string>::iterator i=m_sources.begin(); i!=m_sources.end(); i++)
        {
            d_obj[I]+=m_outD[I]+"/"+rmSlash(*i)+"."+itostring(I)+".o ";
            r_obj[I]+=m_outR[I]+"/"+rmSlash(*i)+"."+itostring(I)+".o ";
        }
        fprintf(fm,"\nDEBUG_OBJS%d=%s\n",I,d_obj[I].c_str());
        fprintf(fm,"\nRELEASE_OBJS%d=%s\n",I,r_obj[I].c_str());


//  printf("\ndirs: %s %s\n",m_outD.c_str(),m_outR.c_str());
        for(set<string>::iterator i=m_sources.begin(); i!=m_sources.end(); i++)
        {
            string src=find_srcfile(*i);
            if(src.size()==0)
                fprintf(stderr,"ERROR file %s not found\n",i->c_str());

	    std::string cmd;
	    std::string _ext=ext(src);
	    if(_ext=="cpp")
        	cmd=(string)"c++ -std=c++11 -D__STDC_CONSTANT_MACROS -fPIE  "+m_cflags_include+" -MM "+src+" -MT "+objNameDebug(I,*i)+" -o "+dependNameDebug(I,*i);
    	    else 
    		cmd=(string)"c++  -D__STDC_CONSTANT_MACROS -fPIE  "+m_cflags_include+" -MM "+src+" -MT "+objNameDebug(I,*i)+" -o "+dependNameDebug(I,*i);
            printf("%s\n",cmd.c_str());
            fprintf(fm,"\n%s:\n\t%s\n\n",dependNameDebug(I,*i).c_str(),cmd.c_str());
            system(cmd.c_str());

	    if(_ext=="cpp")
            cmd=(string)"c++ -std=c++11 -D__STDC_CONSTANT_MACROS -fPIE "+m_cflags_include+" -MM "+src+" -MT "+objNameRelease(I,*i)+" -o "+dependNameRelease(I,*i);
	    else 
            cmd=(string)"c++  -D__STDC_CONSTANT_MACROS -fPIE "+m_cflags_include+" -MM "+src+" -MT "+objNameRelease(I,*i)+" -o "+dependNameRelease(I,*i);
            printf("%s\n",cmd.c_str());
            fprintf(fm,"\n%s:\n\t%s\n\n",dependNameRelease(I,*i).c_str(),cmd.c_str());
            system(cmd.c_str());

            cmd=load_file(dependNameDebug(I,*i));
            if(ext(*i)=="cpp")
                cmd+=(string)"\t $(CC"+itostring(I)+") -std=c++11 $(INCLUDES) $(D"+itostring(I)+"_CFLAGS) "+find_srcfile(*i)+" -o "+objNameDebug(I,*i)+"\n\n";
            else if(ext(*i)=="c")
                cmd+=(string)"\t $(C"+itostring(I)+") $(INCLUDES) $(D"+itostring(I)+"_CFLAGS) "+find_srcfile(*i)+" -o "+objNameDebug(I,*i)+"\n\n";
            fprintf(fm,"%s",cmd.c_str());

            cmd=load_file(dependNameRelease(I,*i));
            if(ext(*i)=="cpp")
                cmd+=(string)"\t $(CC"+itostring(I)+") -std=c++11 $(INCLUDES) $(R"+itostring(I)+"_CFLAGS) "+find_srcfile(*i)+" -o "+objNameRelease(I,*i)+"\n\n";
            else if(ext(*i)=="c")
                cmd+=(string)"\t $(C"+itostring(I)+") $(INCLUDES) $(R"+itostring(I)+"_CFLAGS) "+find_srcfile(*i)+" -o "+objNameRelease(I,*i)+"\n\n";
            fprintf(fm,"%s",cmd.c_str());
        }

    }



}
int main(int argc, char **argv)
{
    /* if(argc!=3)
    {
     printf("usage: crmak in out\n");
     exit(0);
    }*/
    FILE *fm=fopen("GNUmakefile","wb");
#ifdef _WIN32
    read_config("__c_mingw32.mkc",fm);
#else
    read_config("__c.mkc",fm);
#endif


// fprintf(fm,"Makefile: __c.mk\n\t crmak\n\n");


    return 0;
}
