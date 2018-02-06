#include <string>
#include <vector>
#include <map>
#include <stdio.h>
#include <sys/stat.h>
#include <regex.h>
#include <unistd.h>
#include <set>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <deque>
std::string load_file(const std::string & fn);
void processSources( FILE* fm);

using namespace std;
std::string cmdline;
set<string> m_includes;
set<string> targets;
set<string> m_sources;
map <string, string> conf;
std::string m_cflags_include;
vector<string> m_outD,m_outR;
map <string,string> m_srcFiles;
vector<string> CC;
set < string > splitTOKs(const char *seps, const char *src);
vector < string > splitTOK(const char *seps, const char *src);
string find_srcfile(const string& s);
std::string strupper(const std::string &s);
std::string strlower(const std::string & s);

//string exename(const string &s,const string& ext);
//string relexename(const string &s,const string& ext);
vector < string > splitTOK(const char *seps, const char *src);
string replace_vals(const string& regex,const string& repl, const string &src);
bool is_file_exists(const string &pathname);
string join(const string &pattern, const vector<string> & arr);
string join(const char* pattern, const vector<string> & arr);
void check_path_wfn(const string & _pathname);
std::set<std::string> baseClasses;
std::string itostring(int i)
{

    char ss[200];
    std::string s;
    ::snprintf(ss, sizeof(ss) - 1, "%d", i);
    s = ss;
    return s;

}
std::string trim(const std::string &s)
{
    std::deque<char> d;
    for (unsigned k=0; k<s.size(); k++)
    {
        d.push_back(s[k]);
    }
    while (d.size())
    {
        if (isspace(*d.begin()) || *d.begin()=='-') d.pop_front();
        else break;
    }
    while (d.size())
    {
        if (isspace(*d.rbegin())|| *d.rbegin()=='-') d.pop_back();
        else break;
    }
    std::string ret;
    for (unsigned i=0; i<d.size(); i++)
        ret+=d[i];
    return ret;
}
std::string rtrim(const std::string &s)
{
    std::deque<char> d;
    for (unsigned k=0; k<s.size(); k++)
    {
        d.push_back(s[k]);
    }
    while (d.size())
    {
        if (isspace(*d.rbegin())|| *d.rbegin()=='-') d.pop_back();
        else break;
    }
    std::string ret;
    for (unsigned i=0; i<d.size(); i++)
        ret+=d[i];
    return ret;
}
void replace(string&s,const char old,const char newc)
{
    for(size_t i=0; i<s.size(); i++)
    {
        if(s[i]==old)s[i]=newc;
    }
}
std::string removeSpaces(const std::string& s)
{
    std::string r;
    for(size_t i=0; i<s.size(); i++)
    {
        if(!isspace(s[i])) r+=s[i];
    }
    return r;
}
std::string h_body,cpp_body,enum_body,reg_body;

void processColumns(const std::string& section,std::map<int,std::vector<std::string> > &columns, const std::string& comment)
{
    printf("\nProcess columns\n");
    std::string className,tlvType,classType,descr,msg, className_orig;
    std::set<std::string> scope;
    for(std::map<int,std::vector<std::string> >::iterator it=columns.begin(); it!=columns.end(); it++)
    {
        if(it->first==0)
        {
            for(size_t i=0; i<it->second.size(); i++)
                it->second[i]=trim(it->second[i]);

            className=join(" ",it->second);
            className=trim(className);
            className_orig=className;
            replace(className,' ','_');
            replace(className,'+','_');
            replace(className,'-','_');
            replace(className,'/','$');
            replace(className,'(','$');
            replace(className,')','$');
            replace(className,',','_');
            replace(className,'.','_');
            replace(className,'=','e');
            printf("\n classname %s",className.c_str());
        }
        if(it->first==1)
        {
            for(size_t i=0; i<it->second.size(); i++)
                it->second[i]=trim(it->second[i]);

            tlvType=join("",it->second);
            tlvType=trim(tlvType);
            replace(tlvType,' ','_');
            replace(tlvType,'-','_');
            replace(tlvType,'+','_');
//            replace(tlvType,'/','$');
//            replace(tlvType,'(','$');
//            replace(tlvType,')','$');
            replace(tlvType,',','_');
            printf("\n tlvType %s",tlvType.c_str());
        }
        if(it->first==2)
        {
            for(size_t i=0; i<it->second.size(); i++)
                it->second[i]=trim(it->second[i]);

            classType=join("",it->second);
            classType=trim(classType);
            printf("\n classType %s",classType.c_str());
        }
        if(it->first==3)
        {
            for(size_t i=0; i<it->second.size(); i++)
                it->second[i]=rtrim(it->second[i]);
            descr=join("\n",it->second);
            printf("\n descr\n%s",descr.c_str());
        }
        if(it->first==4)
        {
            msg=join(",",it->second);
            msg=removeSpaces(trim(msg));
            std::vector<std::string> m=splitTOK(",",msg.c_str());
            std::set<std::string> s;
            for(size_t a=0; a<m.size(); a++)
            {
                if(m[a]=="DSx-REQ")
                {
                    s.insert("DSA-REQ");
                    s.insert("DSC-REQ");
                    s.insert("DSD-REQ");
                }
                else if(m[a]=="DSx-RSP")
                {
                    s.insert("DSA-RSP");
                    s.insert("DSC-RSP");
                    s.insert("DSD-RSP");
                }
                else if(m[a]=="DSx-ACK")
                {
                    s.insert("DSA-ACK");
                    s.insert("DSC-ACK");
                }
                else s.insert(m[a]);
            }
            m.clear();
            for(typeof(s.begin())it=s.begin(); it!=s.end(); it++)
            {
                m.push_back(*it);
            }

            msg=join(",",m);
            printf("\n msg\n%s",msg.c_str());


        }
        if(it->first==5)
        {
            std::string str=join(" ",it->second);
            str=removeSpaces(trim(str));
            std::vector<std::string> m=splitTOK(" ,",str.c_str());
            for(size_t i=0; i<m.size(); i++)
            {
                scope.insert(strupper(m[i]));
            }
//            msg=join(",",m);
//            printf("\n555555555 msg\n%s",msg.c_str());


        }
    }
    if(!scope.count("ALL")&& !scope.count("OFDMA")) return;
    if(className.size())
    {
        std::string baseClass;
        if(classType=="1")
        {
            baseClass="byte8";
        }
        else if(classType=="2")
        {

            baseClass="byte16";
        }
        else if(classType=="4")
        {
            baseClass="byte32";
        }
        else if(classType=="3")
        {
            baseClass="byte24";
        }
        else if(classType=="TLVlist")
        {

            baseClass=classType;
        }
        else if(baseClasses.count(classType))
        {
            baseClass=classType;
        }
        else baseClass="WimaxBufferContainer";
        std::string classId="_"+section+"_"+className;
        classId=removeSpaces(trim(classId));
        replace(classId,'.','_');
        char buf[10000];
        std::vector<std::string> commonlines;
        commonlines.push_back((std::string)"        "+className+"(uint8_t id): "+baseClass+"(id) {}\n");
        commonlines.push_back((std::string)"        static WimaxBufferContainer* construct(uint8_t id) {return new "+className+"(id);}\n");
        commonlines.push_back((std::string)"        const char* className() const {return \""+section+" "+className_orig+"\";}\n");
        commonlines.push_back((std::string)"        int classId() const  {return ClassID::"+classId+";}\n");
        commonlines.push_back((std::string)"        enum { eClassId=ClassID::"+classId+"};\n");
        commonlines.push_back((std::string)"        static const char* info(){return \""+section+" "+className_orig+"\";}\n");
        commonlines.push_back((std::string)"        static const char* regcommand(){return \""+msg+"|"+tlvType+"\";}\n");

        if(baseClass!="WimaxBufferContainer")
        {
            std::vector<std::string> vcomment=splitTOK("\n\r",comment.c_str());
            std::vector<std::string> vdescr=splitTOK("\n\r",descr.c_str());
            std::string zcomment,zdescr;
            for(size_t i=0; i<vcomment.size(); i++)
                zcomment+="///\\brief "+vcomment[i]+"\n";
            for(size_t i=0; i<vdescr.size(); i++)
                zdescr+="///\\brief "+vdescr[i]+"\n";

            h_body+=     "///"+section+"\n"+zcomment+zdescr+

                         "    class "+className+": public "+baseClass+"\n"
                         "    {\n"
                         "    /*\n"
                         +std::string(section+"\n"+comment+"\n"+descr)+"\n"
                         "    */\n"
                         "    public:\n"
                         +join("",commonlines)+
                         "    };\n\n";
        }
        else
        {
            h_body+=
                "    class "+className+": public WimaxBufferContainer\n"
                "    {\n"
                "    /*\n"
                ""+section+"\n\n"
                "length:"+classType+"\n\n\n"

                ""+comment+"\n"+descr+"\n"
                "    */\n"
                "    public:\n"
                +join("",commonlines)+
                "        void parse(inBitStream& ){throw CommonError(\"parse: unimplemented %s %d\",__FILE__,__LINE__);}\n"
                "        void compose(outBitStream& ) {throw CommonError(\"compose: unimplemented %s %s %d\",className(),__FILE__,__LINE__);}\n"
                "    };\n";

        }

        enum_body+=classId+",\n";
        snprintf(buf,sizeof(buf),
                 msg.c_str(),tlvType.c_str(),section.c_str(),className_orig.c_str(),className.c_str());
        reg_body+=(std::string)
                  "   {MUTEX_INSPECTOR; wimax->registerParser(WimaxTLV::"+className+"::regcommand(),WimaxTLV::"+className+"::info(),WimaxTLV::"+className+"::construct);}\n\n";


    }
}
void appendfile(const std::string& fn, const std::string& buf)
{
    FILE* f=fopen(fn.c_str(),"a");
    fputs(buf.c_str(),f);
    fclose(f);
}

void genclasses(const char * _fn)
{
    std::string fn;
    if(_fn)fn=_fn;
//    fn="/home/bsr/DTK_obd/Projects/802.16e/Soft/core-bsr/modules/wimax-parser/dcd.txt";
    std::string body=load_file(fn);

    int pz=fn.rfind('.');
    std::string h_fn=fn.substr(0,pz)+".h";
//    std::string he_fn=fn.substr(0,pz)+"-extra.h";
    std::string enum_fn=fn.substr(0,pz)+".enum.h";
    std::string reg_fn=fn.substr(0,pz)+".cpp";

    std::string h_def=h_fn;
    replace(h_def,'.','_');
    replace(h_def,'/','_');
    replace(h_def,'-','_');

    /*    std::string he_def=he_fn;
        replace(he_def,'.','_');
        replace(he_def,'/','_');
        replace(he_def,'-','_');
    */
    std::string reg_func="reg_"+fn.substr(0,pz);
    replace(reg_func,'.','_');

    printf("%s",h_fn.c_str());
    //return;
    std::vector<std::string> v=splitTOK("\r\n",body.c_str());
    std::string section;
    std::string block;
    std::vector<int> tabstops;
    std::map<int,std::vector<std::string> > columns;
    std::string comment;
    bool comment_mode=false;
    for(size_t i=0; i< v.size(); i++)
    {

        if(v[i].size()>1)
        {

            if(v[i].substr(0,2)=="!g")
            {
                if(tabstops.size())
                {
                    processColumns(section,columns,comment);
                    tabstops.clear();
                    columns.clear();
                }

            }
            else if(v[i].substr(0,2)=="!$")
            {
                section=trim(v[i].substr(2,v[i].size()-2));
                printf("\nsection:%s",section.c_str());

                comment="";
            }
            else
            {
                if(v[i].substr(0,2)=="!<")
                {
                    printf("start comment mode\n");
                    comment_mode=true;
                    v[i][0]=' ';
                    v[i][1]=' ';
                }
                if(v[i].substr(0,2)=="!>")
                {
                    printf("end comment mode\n");
                    comment_mode=false;
                    v[i][0]=' ';
                    v[i][1]=' ';
                }
                int pz=v[i].find("!!");
                if(pz!=-1)
                {
                    printf("setting tabstops\n");
                    v[i][pz+0]=' ';
                    v[i][pz+1]=' ';

                    tabstops.push_back(pz);
                    for(size_t j=0; j<v[i].size(); j++)
                    {
                        if(v[i][j]=='@')
                        {
                            v[i][j]=' ';
                            tabstops.push_back(j);
                        }
                    }
                    tabstops.push_back(v[i].size());
                }
                //else
                if(comment_mode) comment+=v[i]+"\n";
                else
                {
                    if(tabstops.size())
                    {
                        for(size_t t=1; t<tabstops.size(); t++)
                        {
                            int start=tabstops[t-1];
                            int len=tabstops[t]-start;
                            if(v[i].size()>start)
                            {
                                //if(v[i].size())
                                columns[t-1].push_back(v[i].substr(start,len));
                            }
                        }
                    }
                }
            }
        }
    }

    appendfile(h_fn,
               "/* automatically generated source by command "+cmdline+". DO NOT EDIT THIS FILE!!! */\n"
               "#ifndef __________________MM__TLV___H"+h_def+"\n"
               "#define __________________MM__TLV___H"+h_def+"\n"
               "#include \"wimaxBufferContainer.h\"\n"
               "#include \"mm_classId.h\"\n"
               "#include \"tlvBase.h\"\n"
               "#include \"mutexInspector.h\"\n"
               "namespace WimaxTLV\n"
               "{\n"

               +   h_body+

               "}\n"
               "#endif\n"
              );
    appendfile(enum_fn,"/* automatically generated source by command "+cmdline+". DO NOT EDIT THIS FILE!!! */\n" + enum_body);
    appendfile(reg_fn,
               "/* automatically generated source by command "+cmdline+". DO NOT EDIT THIS FILE!!! */\n"
               "#include \"mm_tlv.h\"\n"
               "#include \"mutexInspector.h\"\n"
               "#include \"IInstance.h\"\n"
               "#include \""+h_fn+"\"\n"
               "#include \"IWimax.h\"\n"
               "void "+reg_func+"(IWimax* wimax)\n"
               "{XTRY;\n"
               +reg_body+
               "XPASS;}\n"
              );
}










int main(int argc, char **argv)
{
    std::vector<std::string> v;
    for(int i=0; i<argc; i++)
    {
        v.push_back(argv[i]);
    }
    cmdline=join(" ",v);
    std::string bc=load_file("./baseClasses.txt");
    baseClasses=splitTOKs("\r\n ",bc.c_str());
    genclasses(argv[1]);


//    fprintf(fm,"Makefile: __c.mk\n\t crmak\n\n");


    return 0;
}
