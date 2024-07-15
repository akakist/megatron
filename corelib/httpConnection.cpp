#include "IUtils.h"
#include "splitStr.h"
#include <unistd.h>
#include <sstream>
#define _FILE_OFFSET_BITS 64
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif

#include <stdlib.h>
#ifndef _WIN32
#include <sys/socket.h>
#endif
#include <string>
#include "httpConnection.h"
//#include "Events/System/Net/socket/Write.h"


std::string HTTP::get_name_of_http_code(int code)
{
    std::string res;
    switch (code)
    {
    case 200:
        res="OK";
        break;
    case 203:
        res="Non-Authoritative Information";
        break;
    case 206:
        res="Partial Content";
        break;
    case 302:
        res="Found";
        break;
    case 303:
        res="See Other";
        break;
    case 400:
        res="Bad Request";
        break;
    case 401:
        res="Unauthorized";
        break;
    case 402:
        res="Payment Required";
        break;
    case 403:
        res="Forbidden";
        break;
    case 404:
        res="Not Found";
        break;
    case 405:
        res="Method Not Allowed";
        break;
    case 406:
        res="Not Acceptable";
        break;
    case 500:
        res="Internal Server Error";
        break;
    case 501:
        res="Not Implemented";
        break;
    case 502:
        res="Bad Gateway";
        break;
    case 503:
        res="Service Unavailable";
        break;
    case 505:
        res="HTTP Version not supported";
        break;
    default:
        res="Unknow";
        break;
        break;
    }
    return res;
}
void HTTP::Request::split_params(const std::string & s)
{
    std::deque <std::string> pr = splitStr("&", s);
    for (unsigned int i = 0; i < pr.size(); i++)
    {
        std::string p = pr[i];
        size_t pp = p.find("=", 0);
        if (pp == std::string::npos)
        {
            continue;
        }
//        std::string val = iUtils->unescapeURL(p.substr(pp + 1, p.length() - pp - 1));
        std::string val = p.substr(pp + 1, p.length() - pp - 1);
        std::string key = p.substr(0, pp);
        params[key] = val;
        v_params[key].push_back(val);
    }
}
std::string HTTP::make_http_header()
{
    std::map<std::string,std::string> pr;
    return make_http_header(200,pr);
}

std::string HTTP::make_http_header(const int code)
{
    std::map<std::string,std::string> pr;
    return make_http_header(code,pr);
}

std::string HTTP::make_http_header(std::map<std::string,std::string> &pr)
{
    return make_http_header(200,pr);
}

std::string HTTP::make_http_header(const int code,std::map<std::string,std::string> &pr)
{
    std::string a("HTTP/1.1 ");
    std::string res=get_name_of_http_code(code);
    a+=std::to_string(code)+" "+res+"\r\n";
    if(!pr.count(("Server")))
        a+="Server: servak\r\n";
    if (!pr.count("Connection"))
    {
        pr["Connection"]="close";
    }
    for (auto& i:pr)
    {
        a+=i.first+": "+i.second+"\r\n";
    }
    return a;
}
std::string HTTP::Response::build_html_response()
{
//    std::string ret;
    std::ostringstream ret;
    ret << "HTTP/1.1 " << std::to_string(http_code) << " " << HTTP::get_name_of_http_code(http_code) << "\r\n";
    if (http_content_type != "")
    {
        std::string r = http_content_type;
        if (http_charset.size())
            r += "; charset: " + http_charset;
        http_header_out["Content-Type"] = r;
    }
    if (!http_header_out.count("Connection"))
        http_header_out["Connection"] = "close";

    http_header_out["Server"] = "Web Server";
    http_header_out["Content-Length"] = std::to_string(content.size());

    for (auto& i: http_header_out)
    {
        ret << i.first << ": " << i.second << "\r\n";
    }
    if (out_cookies.size())
    {
        for (auto& i: out_cookies)
        {
            ret<< "Set-Cookie: " << i.first;
            ret<< "=";
            ret<< i.second;
            ret<< "; path=/\r\n";

        }
    }

    ret << "\r\n";
    ret << content;
    return ret.str();
}

std::string HTTP::Response::build_html_response_wo_content_length()
{
    std::string ret;
    ret += "HTTP/1.1 " + std::to_string(http_code) + " " + HTTP::get_name_of_http_code(http_code) + "\r\n";
    if (http_content_type != "")
    {
        std::string r = http_content_type;
        if (http_charset.size())
            r += "; charset: " + http_charset;
        http_header_out["Content-Type"] = r;
    }
    if (http_header_out.find("Connection") == http_header_out.end())
        http_header_out["Connection"] = "close";

    http_header_out["Server"] = "Web Server";

    for (auto& i: http_header_out)
    {
        ret += i.first + ": " + i.second + "\r\n";
    }
    if (out_cookies.size())
    {
        for (auto& i: out_cookies)
        {
            std::string r;
            r += "Set-Cookie: "+i.first;
            r += "=";
            r += i.second;
            r += "; path=/\r\n";
            ret+=r;

        }
    }

    ret += "\r\n";
    ret += content;
    return ret;
}

void HTTP::Response::redirect(const std::string &url)
{
    http_code=302;
    http_header_out["Location"]=url;
    return;
}
HTTP::Response::~Response()
{

}




HTTP::Request::Request()
    :
    m_last_io_time(time(NULL)),
    fileresponse(NULL)
    ,isKeepAlive(false),
    sendRequestIncomingIsSent(false)
{
}
HTTP::Response::Response(IInstance* _ins)
    :http_code(200),http_content_type("text/html"),allow_build_response(true),iInstance(_ins)
{
}


void HTTP::Response::makeResponse(const REF_getter<epoll_socket_info>& esi)
{
    std::string out = build_html_response();
    esi->markedToDestroyOnSend_=true;
    esi->write_(out);

}
void HTTP::Response::makeResponsePersistent(const REF_getter<epoll_socket_info> &esi)
{
    std::string out = build_html_response_wo_content_length();
    esi->write_(out);
}


HTTP::Request::_fileresponse::~_fileresponse()
{
    if(m_fd!=-1)
    {
        if(io_protocol.m_close)
        {
            io_protocol.m_close(m_fd);
        }
        else
        {
            ::close(m_fd);
        }
        m_fd=-1;
    }
}
