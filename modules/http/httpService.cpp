#define _FILE_OFFSET_BITS 64
#define _LARGEFILE64_SOURCE

#include <stdlib.h>
#ifndef _WIN32
#include <unistd.h>
#endif
#include "errno.h"
#include "httpService.h"


#include "route_t.h"


#include "version_mega.h"
#include "VERSION_id.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include "st_malloc.h"
#include <time.h>
#include "st_FILE.h"

#include "tools_mt.h"
#include "Events/System/Net/socket/AddToListenTCP.h"
#include "Events/System/Net/socket/ConnectFailed.h"
#include "Events/System/Net/rpc/IncomingOnAcceptor.h"
#include "events_http.hpp"

extern char mime_types[];
extern int mime_types_sz;
std::string datef(const time_t &__t);



UnknownBase* HTTP::Service::construct(const SERVICE_id &id, const std::string& nm,IInstance* _if)
{
    return new Service(id,nm,_if);
}


HTTP::Service::Service(const SERVICE_id& id, const std::string&nm, IInstance* _if):
    UnknownBase(nm),Broadcaster(_if),
    ListenerBuffered1Thread(this,nm,_if->getConfig(),id,_if),
    Logging(this,ERROR_log,_if),_stuff(new __http_stuff)
{
    m_maxPost=_if->getConfig()->get_int64_t("max_post",1000000,"");
    {
        {
            M_LOCK(mx);
            mx.docUrls=_if->getConfig()->get_stringset("doc_urls","/pics,/html,/css","");
            mx.documentRoot=_if->getConfig()->get_string("document_root","./www","");
        }
        try {
            std::string body;
            body=std::string(mime_types,mime_types_sz);

            {
                M_LOCK(mx);
                mx.mime_types.clear();
            }
            std::vector<std::string> v=iUtils->splitString("\r\n",body);
            for (size_t i=0; i<v.size(); i++)
            {
                if (v[i].size())
                {
                    if (v[i][0]=='#') continue;
                }
                std::deque<std::string> dq=iUtils->splitStringDQ("\t ",v[i]);
                if (dq.size())
                {
                    std::string typ=dq[0];
                    dq.pop_front();
                    while (dq.size())
                    {
                        
                        {
                            M_LOCK(mx);
                            mx.mime_types[dq[0]]=typ;
                        }
                        dq.pop_front();
                    }
                }
            }
        }
        catch(...) {}
    }
}

bool HTTP::Service::on_Connected(const socketEvent::Connected*)
{
    return true;

}
bool HTTP::Service::on_NotifyBindAddress(const socketEvent::NotifyBindAddress*e)
{
    M_LOCK(mx);
    mx.bind_addrs.insert(e->esi->local_name);
    return true;
}


bool HTTP::Service::on_DoListen(const httpEvent::DoListen* e)
{

    SOCKET_id newid=iUtils->getSocketId();
    msockaddr_in sa=e->addr;
    //sa.setSocketId(newid);
    DBG(log(TRACE_log,"on_DoListen %s",e->route.dump().c_str()));
    sendEvent(ServiceEnum::Socket,new socketEvent::AddToListenTCP(newid,sa,"HTTP",false,NULL,e->route));

    return true;
}

bool HTTP::Service::on_startService(const systemEvent::startService*)
{
    return true;
}
bool HTTP::Service::on_GetBindPortsREQ(const httpEvent::GetBindPortsREQ *e)
{
    M_LOCK(mx);
    passEvent(new httpEvent::GetBindPortsRSP(mx.bind_addrs,poppedFrontRoute(e->route)));
    return true;
}

bool HTTP::Service::handleEvent(const REF_getter<Event::Base>& evt)
{
    auto &ID=evt->id;

    if( socketEventEnum::Accepted==ID)
        return on_Accepted((const socketEvent::Accepted*)evt.operator->());
    if( socketEventEnum::StreamRead==ID)
        return on_StreamRead((const socketEvent::StreamRead*)evt.operator->());
    if( socketEventEnum::Connected==ID)
        return on_Connected((const socketEvent::Connected*)evt.operator->());
    if( socketEventEnum::NotifyBindAddress==ID)
        return on_NotifyBindAddress((const socketEvent::NotifyBindAddress*)evt.operator->());
    if( socketEventEnum::NotifyOutBufferEmpty==ID)
        return on_NotifyOutBufferEmpty((const socketEvent::NotifyOutBufferEmpty*)evt.operator->());
    if( httpEventEnum::DoListen==ID)
        return(this->on_DoListen((const httpEvent::DoListen*)evt.operator->()));
    if( httpEventEnum::RegisterProtocol==ID)
        return(this->on_RegisterProtocol((const httpEvent::RegisterProtocol*)evt.operator->()));
    if( httpEventEnum::GetBindPortsREQ==ID)
        return(this->on_GetBindPortsREQ((const httpEvent::GetBindPortsREQ*)evt.operator->()));
    if( systemEventEnum::startService==ID)
        return on_startService((const systemEvent::startService*)evt.operator->());
    if( rpcEventEnum::IncomingOnAcceptor==ID)
    {
        rpcEvent::IncomingOnAcceptor *E=(rpcEvent::IncomingOnAcceptor *)evt.operator ->();
        auto IDA=E->e->id;
        if(httpEventEnum::GetBindPortsREQ==IDA)
        {
            const httpEvent::GetBindPortsREQ *e=(const httpEvent::GetBindPortsREQ *)E->e.operator ->();
            M_LOCK(mx);
            passEvent(new httpEvent::GetBindPortsRSP(mx.bind_addrs,poppedFrontRoute(e->route)));
            return true;
        }
        return false;
    }

    return false;
}

bool HTTP::Service::on_Accepted(const socketEvent::Accepted* evt)
{
    _stuff->insert(evt->esi->m_id,new HTTP::Request());
    _stuff->add(ServiceEnum::HTTP,evt->esi);
    return true;
}
bool HTTP::Service::on_StreamRead(const socketEvent::StreamRead* evt)
{
    REF_getter<HTTP::Request> W=_stuff->getRequestOrNull(evt->esi->m_id);
    if (!W.valid())
    {
        _stuff->insert(evt->esi->m_id,new HTTP::Request());
        _stuff->add(ServiceEnum::HTTP,evt->esi);
        W=_stuff->getRequestOrNull(evt->esi->m_id);
        if(!W.valid())
        {
            throw CommonError("invalid behaviour %s %d",__FILE__,__LINE__);
        }
    }

    W->m_last_io_time=time(NULL);
    if (W->parse_state.count(1)==0)
    {
        std::string head;
        {
            M_LOCK(evt->esi->m_inBuffer);
            if (!W.___ptr->__gets$(head,"\r\n\r\n", evt->esi->m_inBuffer._mx_data))
            {
                return true;
            }
        }
        std::deque<std::string> dq=iUtils->splitStringDQ("\r\n",head);
        if (dq.size())
        {

            std::string fl=dq[0];
            dq.pop_front();
            std::string::size_type pom = fl.find(" ", 0);
            if (pom == std::string::npos)
            {
                return true;
            }
            (std::string&)W->METHOD =iUtils->strupper(fl.substr(0, pom));

            if (W->METHOD != "GET" && W->METHOD != "POST")
            {
                return true;
            }

            W->url = fl.substr(pom + 1, fl.find(" ", pom + 1) - pom - 1);
            W->url = iUtils->unescapeURL(W->url);
            if (W->url.find("..", 0) != std::string::npos)
            {
                return true;
            }
        }
        while (dq.size())
        {
            std::string	line=dq[0];
            dq.pop_front();
            std::string::size_type pop = line.find(":", 0);
            if (pop == std::string::npos)
            {
                return true;
            }
            std::string k=iUtils->strupper(line.substr(0, pop));
            std::string v=line.substr(pop + 2, line.length() - pop - 2);
            W->headers[k]=v;
        }
        if (W->headers.count("COOKIE"))
        {

            std::vector <std::string> v = iUtils->splitString("; ", W->headers["COOKIE"]);
            for (unsigned int i = 0; i < v.size(); i++)
            {
                std::string q = v[i];
                if (q == " ")
                {
                    continue;
                }
                size_t z;
                z = q.find("=", 0);
                if (z == std::string::npos)
                {
                    continue;
                }
                std::string k=q.substr(0, z);
                std::string v=q.substr(z + 1, q.size() - z - 1);
                W->in_cookies[k] = v;
            }
        }
        W->original_url=W->url;
        {
            
            std::string::size_type qp = W->url.find("?", 0);
            if (qp != std::string::npos)
            {
                W->original_params=W->url.substr(qp + 1, W->url.size() - qp - 1);
                W->split_params(W->original_params);
                W->url = W->url.substr(0, qp);
            }
        }

        W->parse_state.insert(1);
    }

    if (W->parse_state.count(2)==0)
    {
        
        if (W->METHOD == "POST")
        {
            
            if (W->headers["CONTENT-TYPE"].find("multipart/form", 0) == std::string::npos)
            {
                
                size_t clen = atoi(W->headers["CONTENT-LENGTH"].c_str());
                if (clen <= 0 || clen > m_maxPost)
                {
                    return true;
                }
                {
                    M_LOCK(evt->esi->m_inBuffer);
                    if (!W->__readbuf$(W->postContent,clen, evt->esi->m_inBuffer._mx_data)) return true;
                }
                W->split_params(W->postContent);
            }
            else
            {
                

                //Multipart form
                const std::string &t= W->headers["CONTENT-TYPE"];
                std::string::size_type pz = t.find("boundary=", 0);
                if (pz == std::string::npos)
                {

                    return true;
                }
                std::string bound = "--" + t.substr(pz + 9, t.length() - pz - 9);
                std::string ebound = bound + "--";
                std::string sbuf;
                {
                    
                    {
                        M_LOCK(evt->esi->m_inBuffer);
                        if (!W->__gets$(sbuf ,ebound, evt->esi->m_inBuffer._mx_data)) return true;
                    }

                    while (sbuf.size())
                    {

                        
                        std::string s;
                        std::string::size_type pos=sbuf.find(bound);
                        if (pos==std::string::npos)
                        {
                            s=sbuf;
                            sbuf="";
                        }
                        else
                        {
                            s=sbuf.substr(0,pos);
                            sbuf=sbuf.substr(pos+bound.size(),sbuf.size()-pos-bound.size());
                        }
                        if (s.size()>1)
                        {
                            if (s[0]=='\r'&&s[1]=='\n') s=s.substr(2,s.size()-2);
                        }
                        if (s.size())
                        {
                            
                            std::map<std::string,std::string> lparams;
                            std::string content;
                            while (1)
                            {
                                
                                std::string sloc;
                                std::string::size_type pp=s.find("\r\n");
                                if (pp==std::string::npos)
                                {
                                    logErr2("bad multipart %s %d",__FILE__,__LINE__);
                                    return true;
                                }
                                sloc=s.substr(0,pp);
                                s=s.substr(pp+2,s.size()-pp-2);
                                if (!sloc.size())
                                {
                                    content=s.substr(0,s.size()-2);
                                    break;
                                }
                                std::string::size_type pz=sloc.find(": ");
                                if (pz==std::string::npos)
                                {
                                    logErr2("bad multipart %s %d",__FILE__,__LINE__);
                                    return  true;
                                }
                                std::string key=sloc.substr(0,pz);
                                std::string val=sloc.substr(pz+2,sloc.size()-pz-2);
                                lparams[key]=val;
                            }
                            if (lparams.count("Content-Disposition"))
                            {
                                
                                std::string cd=lparams["Content-Disposition"];
                                std::vector<std::string> flds;
                                while (cd.size())
                                {
                                    std::string::size_type ps=cd.find("; ");
                                    if (ps==std::string::npos)
                                    {
                                        flds.push_back(cd);
                                        cd="";
                                    }
                                    else
                                    {
                                        flds.push_back(cd.substr(0,ps));
                                        cd=cd.substr(ps+2,cd.size()-2);
                                    }
                                }
                                std::string name;
                                std::string filename;
                                for (unsigned i=0; i<flds.size(); i++)
                                {
                                    std::string::size_type pzz=flds[i].find("=");
                                    if (pzz!=std::string::npos)
                                    {
                                        std::string key=flds[i].substr(0,pzz);
                                        std::string val=flds[i].substr(pzz+1,flds[i].size()-1);
                                        if (val.size()>2)
                                        {
                                            if (val[0]=='\"' && val[val.size()-1]=='\"') val=val.substr(1,val.size()-2);
                                        }
                                        if (val=="\"\"") val="";
                                        if (key=="name") name=val;
                                        if (key=="filename") filename=val;
                                    }
                                }
                                W->params[name]=content;
                                if (filename.size())
                                    W->params[name+"_FILENAME"]=filename;
                                for (std::map<std::string,std::string>::const_iterator i=lparams.begin(); i!=lparams.end(); i++)
                                {
                                    std::string q = iUtils->strupper(i->first);
                                    std::string k=name + "_" + q;
                                    W->params[k] = i->second;
                                }
                            }
                        }
                    }
                }
            }
        }
        W->parse_state.insert(2);
    }
    if(W->headers.count("CONNECTION"))
    {
        if(iUtils->strlower(W->headers["CONNECTION"])=="keep-alive")
        {
            W->fileresponse->isKeepAlive=true;
        }
        else
        {
            W->fileresponse->isKeepAlive=false;
        }
    }
    else
    {
    }

    std::set<std::string> docUrls;
    std::string documentRoot;
    std::map<std::string,HTTP::IoProtocol> protocols;
    {
        M_LOCK(mx);
        docUrls=mx.docUrls;
        documentRoot=mx.documentRoot;
        protocols=mx.protocols;
    }
    for (std::set<std::string>::const_iterator i=docUrls.begin(); i!=docUrls.end(); i++)
    {
        
        if (W->url.substr(0, i->size()) == *i)
        {

            
            {
                std::string fn=documentRoot + W->url;
                std::string ext = iUtils->extractFileExt(fn);
                W->fileresponse->fileName=fn;
                W->fileresponse->extension=ext;
                send_other_from_disk_ext(evt->esi, W, fn, ext);
                return true;
            }
        }

    }
#ifdef KALL
    if(W->url.substr(0,7)=="/files/")
    {
        IDFSContent *fc=static_cast<IDFSContent*>(iInstance->getServiceOrCreate(ServiceEnum::DFSContent)->cast(UnknownCast::IDFSContent));
        std::string body;
        if(fc->vf_get_http_file(W,body))
        {
            std::string exten = iUtils->extractFileExt(W->url);
            std::string out;
            out+="HTTP/1.1 200 OK\r\n";
            out+="Server: nginx/1.2.6 (Ubuntu)\r\n";
            {
                M_LOCK(mx);
                std::map<std::string,std::string>::iterator i=mx.mime_types.find(exten);
                if(i!=mx.mime_types.end())
                    out+="Content-Type: "+i->second+"\r\n";
                else
                    out+="Content-Type: text/plain\r\n";
            }
            out+="Date: "+datef(time(NULL))+"\r\n";
            out+="Content-Length: "+iUtils->toString(int64_t(body.size()))+"\r\n";
            out+="Accept-Ranges: bytes\r\n";
            out+="Connection: close\r\n";
            out+="\r\n";
            out+=body;
            evt->esi->write_(out);
            return true;
        }
        else
        {
            std::string a("HTTP/1.1 404 Not Found\r\n");
            a.append("Server: nginx/1.2.6 (Ubuntu)\r\n");
            a.append("Connection: close\r\n");
            a.append("\r\n");
            evt->esi->write_(a);
            return true;
        }
    }
#endif

    for(std::map<std::string,HTTP::IoProtocol>::iterator i=protocols.begin(); i!=protocols.end(); i++)
    {
        if (W->url.substr(0, i->first.size()) == i->first)
        {
            std::string dfsUrl=W->url.substr(i->first.size(),W->url.size()-i->first.size());


            std::string::size_type pz = dfsUrl.rfind(".", dfsUrl.size());
            std::string ext;
            std::string fn;
            if (pz != std::string::npos)
            {
                ext=dfsUrl.substr(pz + 1, dfsUrl.size() - pz - 1);
                dfsUrl=dfsUrl.substr(0, pz);

            }

            dfsUrl=iUtils->hex2bin(dfsUrl);
            W->fileresponse->fileName=dfsUrl;
            W->fileresponse->extension=ext;
            W->fileresponse->io_protocol=i->second;
            send_other_from_disk_ext(evt->esi, W, dfsUrl, ext);
            return true;
        }

    }
    passEvent(new httpEvent::RequestIncoming(W,evt->esi,evt->route));
    return  true;
}

std::string HTTP::Service::get_mime_type(const std::string& mime) const
{
    M_LOCK(mx);
    std::map<std::string,std::string>::const_iterator i=mx.mime_types.find(mime);
    if (i==mx.mime_types.end()) return "";
    else return i->second;
}
void HTTP::__http_stuff::on_delete(const REF_getter<epoll_socket_info>&esi, const std::string& )
{
    
    M_LOCK(m_lock);
    std::map<SOCKET_id,REF_getter<HTTP::Request> >::iterator i=container.find(esi->m_id);
    if (i==container.end()) throw CommonError("__http_stuff::on_delete: not found %s %d",__FILE__,__LINE__);
    container.erase(esi->m_id);
}

REF_getter<HTTP::Request> HTTP::__http_stuff::getRequestOrNull(const SOCKET_id& id)
{
    
    M_LOCK(m_lock);
    std::map<SOCKET_id,REF_getter<HTTP::Request> >::iterator i=container.find(id);
    if (i!=container.end()) return i->second;
    return NULL;
}
void HTTP::__http_stuff::insert(const SOCKET_id& id,const REF_getter<HTTP::Request> &C)
{
    
    {
        M_LOCK(m_lock);
        container.insert(std::make_pair(id,C));
    }

}
bool HTTP::Service::on_NotifyOutBufferEmpty(const socketEvent::NotifyOutBufferEmpty* e)
{
    
    S_LOG("on_NotifyOutBufferEmpty");
    DBG(log(TRACE_log,"on_NotifyOutBufferEmpty %s",e->route.dump().c_str()));

    REF_getter<HTTP::Request> W=_stuff->getRequestOrNull(e->esi->m_id);
    if(!W.valid())
    {
        
        e->esi->close("HTTP::Request not exists");
        return true;
    }
    REF_getter<HTTP::Request::_fileresponse> F=W->fileresponse;
    if(!F.valid()) throw CommonError("if(!F.valid()) %s %d",__FILE__,__LINE__);
    if(F->fileSize==0)
    {
        e->esi->close("HTTPService: on_NotifyOutBufferEmpty: F->fileSize==0");
        return true;
    }
    if(F->fileSize==-1)
    {
        e->esi->close("HTTPService: on_NotifyOutBufferEmpty: F->fileSize==-1");
        return true;
    }
    if(F->contentLength<=F->written_bytes)
    {
        e->esi->close("HTTPService: on_NotifyOutBufferEmpty: F->contentLength>=F->written_bytes");
        return true;
    }
    int64_t offset=F->startb+F->written_bytes;
    {
        size_t bufsize=0x10000;
        st_malloc buf(bufsize);
        {
            
            int64_t readSize=bufsize;
            if(F->written_bytes+readSize > F->contentLength)
            {
                readSize=F->contentLength-F->written_bytes;
            }

            if(offset+readSize>F->fileSize)
            {
                readSize=F->fileSize-offset;
            }

            int64_t res=-1;
            if(W->fileresponse->io_protocol.m_read)
            {
                
                res=F->io_protocol.m_read(F->get_fd(),(unsigned char*)buf.buf,(size_t)readSize);
            }
            else
            {
                
                res=read(F->get_fd(),(char*)buf.buf,(size_t)readSize);
            }

            if(res==-1)
            {
                
                e->esi->close("HTTPService: on_NotifyOutBufferEmpty: read returns -1");
                return true;
            }

            if(res==0)
            {
                
                e->esi->close("HTTPService: on_NotifyOutBufferEmpty: read returns 0 (EOF)");
                return true;
            }

            e->esi->write_((char*)buf.buf,res);
            F->written_bytes+=res;
            return true;
        }
    }
    return true;
}

void registerHTTPModule(const char* pn)
{
    if(pn)
    {
        iUtils->registerPlugingInfo(COREVERSION,pn,IUtils::PLUGIN_TYPE_SERVICE,ServiceEnum::HTTP,"HTTP");
    }
    else
    {
        iUtils->registerService(COREVERSION,ServiceEnum::HTTP,HTTP::Service::construct,"HTTP");
        regEvents_http();
    }
}

std::string datef(const time_t &__t)
{
    time_t t=__t-100000;
    static char const * wkday[]        = {"Sun","Mon" ,"Tue" ,"Wed", "Thu" ,"Fri" , "Sat"};
    static char const* month[]        = {"Jan" , "Feb" , "Mar" , "Apr"
                                         , "May" , "Jun" , "Jul" , "Aug"
                                         , "Sep" , "Oct" , "Nov" , "Dec"
                                        };

    char outstr[200];
    struct tm tt=*localtime(&t);
    snprintf(outstr,sizeof(outstr),"%s, %02d %s %d %02d:%02d:%02d GMT",
             wkday[tt.tm_wday%7],tt.tm_mday,month[tt.tm_mon%12],tt.tm_year+1900,tt.tm_hour,tt.tm_min,tt.tm_sec);
    return outstr;
}

/** -1 error*/ int HTTP::Service::send_other_from_disk_ext(const REF_getter<epoll_socket_info>&esi,const REF_getter<HTTP::Request>&W,const std::string & fn,const std::string& exten)
{
    

    W->m_last_io_time=time(NULL);

    REF_getter<HTTP::Request::_fileresponse> F=W->fileresponse;
    F->fileName=fn;
    F->extension=exten;

    {
        if(F->io_protocol.m_open)
        {
            F->set_fd(F->io_protocol.m_open(fn.c_str(),O_RDONLY));
        }
        else
        {
            F->set_fd(::open(fn.c_str(),O_RDONLY));
        }
        if(F->get_fd()==-1)
        {
            std::string a("HTTP/1.1 404 Not Found\r\n");
            a.append("Server: nginx/1.2.6 (Ubuntu)\r\n");
            a.append("Connection: close\r\n");
            a.append("\r\n");
            esi->write_(a);
            return -1;
        }

        if(F->io_protocol.m_seek)
        {

            F->fileSize=F->io_protocol.m_seek(F->get_fd(),0,SEEK_END);

        }
        else
        {
#ifdef __MACH__
            F->fileSize=lseek(F->get_fd(),0,SEEK_END);
#else
            F->fileSize=lseek64(F->get_fd(),0,SEEK_END);
#endif

        }
        if(F->fileSize==-1)
        {

            std::string a("HTTP/1.1 500 Internal Server Error\r\n");
            a.append("Server: nginx/1.2.6 (Ubuntu)\r\n");
            a.append("Connection: close\r\n");
            a.append("\r\n");
            esi->write_(a);

            return -1;
        }
    }

    F->contentLength=0;
    if (!W->headers.count("RANGE"))
    {

        F->startb=0;
        F->endb=W->fileresponse->fileSize-1;
        F->written_bytes=0;
        F->hasRange=false;

    }
    else
    {

        F->startb=0;
        F->endb=F->fileSize-1;
        F->hasRange=true;
        std::string s = W->headers["RANGE"];
        size_t n = s.find("=", 0);
        if (n != std::string::npos)
        {
            F->hasRange= true;
            std::vector <std::string> v = iUtils->splitString("-=", s.substr(n, s.size() - n));
            if (v.size() >= 1)
            {
                F->startb = atoll(v[0].c_str());
            }
            if (v.size() == 2)
            {
                F->endb  = atoll(v[1].c_str());
            }
            else
            {
                F->endb=F->fileSize-1;
            }
        }
    }

    if (F->startb > F->fileSize || F->startb < 0)
        F->startb = 0;


    if (F->endb > F->fileSize || F->endb < 0)
        F->endb = F->fileSize-1;


    if (F->endb < F->startb)
    {
        F->endb = F->fileSize-1;
        F->startb = 0;
    }

    if(F->endb==0)
        F->endb=F->fileSize-1;

    F->contentLength=F->endb-F->startb+1;


    time_t last_modified;
    {
        M_LOCK(lastModified);
        if(lastModified.container.count(W->url))
            last_modified=lastModified.container[W->url];
        else
        {
            last_modified=time(NULL);
            lastModified.container[W->url]=last_modified;
        }

    }

    std::vector<std::string> out;
    if(F->hasRange)
        out.push_back("HTTP/1.1 206 Partial Content\r\n");
    else
        out.push_back("HTTP/1.1 200 OK\r\n");
    out.push_back("Server: nginx/1.2.6 (Ubuntu)\r\n");

    out.push_back("Date: "+datef(time(NULL))+"\r\n");

    {
        M_LOCK(mx);
        std::map<std::string,std::string>::iterator i=mx.mime_types.find(exten);
        if(i!=mx.mime_types.end())
            out.push_back("Content-Type: "+i->second+"\r\n");
        else
            out.push_back("Content-Type: text/plain\r\n");
    }


    out.push_back("Content-length: "+iUtils->toString(int64_t(F->contentLength))+"\r\n");

    out.push_back("Last-Modified: "+datef(last_modified)+"\r\n");

    if(F->isKeepAlive)
        out.push_back("Connection: keep-alive\r\n");
    else
        out.push_back("Connection: close\r\n");


    if(F->hasRange)
    {
        out.push_back("Content-Range: bytes "
                      +  iUtils->toString((int64_t)F->startb) + "-"
                      +  iUtils->toString((int64_t)F->endb) + "/"
                      +  iUtils->toString((int64_t)F->fileSize)+"\r\n");
    }
    else
    {
        out.push_back("Accept-Ranges: bytes\r\n");
    }


    out.push_back("\r\n");

    for(size_t i=0; i<out.size(); i++)
    {
        esi->write_(out[i]);
    }


    F->headerSent=true;

    if(F->io_protocol.m_seek)
    {

        F->io_protocol.m_seek(F->get_fd(),F->startb,SEEK_SET);

    }
    else
    {
#ifdef __MACH__
        lseek(F->get_fd(),F->startb,SEEK_SET);
#else
        lseek64(F->get_fd(),F->startb,SEEK_SET);
#endif

    }

    return 0;
}

