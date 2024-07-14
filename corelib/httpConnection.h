#pragma once
#include <map>
#include <vector>
#include <string>
#include "REF.h"
#include "epoll_socket_info.h"
//#include "SOCKET_id.h"
#include <deque>
#include "IInstance.h"
//#include "broadcaster.h"


/**
    HTTP::Request, HTTP::Response is used in http service
*/


typedef int (*__url_read)(long _fd, unsigned char *bufferForOutput, int _bufSizeRequested);
typedef int64_t (*__url_seek)(long _fd, int64_t pos, int whence);
typedef int (*__url_close)(long _fd);
typedef long (*__url_open)(const char* fn, int flags);


namespace HTTP
{
    class Service;
    std::string get_name_of_http_code(int code);
    std::string make_http_header();
    std::string make_http_header(const int code);
    std::string make_http_header(std::map<std::string,std::string> &pr);
    std::string make_http_header(const int code,std::map<std::string,std::string> &pr);

    class IoProtocol
    {
    public:
        __url_open m_open;
        __url_read m_read;
        __url_seek m_seek;
        __url_close m_close;
        IoProtocol():m_open(NULL),m_read(NULL),m_seek(NULL),m_close(NULL)
        {

        }

    };

    class Request:public Refcountable
    {

    public:
        std::map<std::string,std::string> headers;
        std::map<std::string,std::string> in_cookies;
        std::map<std::string,std::string> params;
        std::map<std::string,std::vector<std::string> > v_params;/** for multiple select */
        std::string peer_ipaddress;

        std::string METHOD;
        std::string url;
//        std::string original_url;
        std::string original_params;
        std::string postContent;
        std::deque<std::string> vurl;

        Request();
        void split_params(const std::string & s);


        std::set<int> parse_state;
        time_t m_last_io_time;


        struct _fileresponse: public Refcountable
        {
            bool hasRange;
            bool headerSent;
            int64_t written_bytes;
            int64_t startb, endb, contentLength;
            std::string fileName;
            std::string extension;

        private:
            long m_fd;
            bool m_closed;
        public:
            void set_fd(const long &fd) {
                m_fd=fd;
            }
            long get_fd()
            {
                return m_fd;
            }
            int64_t fileSize;
            IoProtocol io_protocol;

            bool closed()
            {
                return m_closed;
            }
            void close()
            {
                m_closed=true;
            }

            _fileresponse():hasRange(false),headerSent(false),written_bytes(0),startb(0),endb(0),contentLength(0),
                m_fd(-1),m_closed(false),fileSize(-1LL) {}
            ~_fileresponse();
        };
        REF_getter<_fileresponse> fileresponse;
        bool isKeepAlive;
        bool sendRequestIncomingIsSent;

    private:
    };
    class Response
    {
    public:
        Response(IInstance* _ins);
        void makeResponse(const REF_getter<epoll_socket_info>& esi);
        void makeResponsePersistent(const REF_getter<epoll_socket_info>& esi);
        int http_code;
        std::string http_content_type;
        std::string content;
        std::string http_charset;
        std::map<std::string,std::string> http_header_out;
        std::map<std::string,std::string> out_cookies;
        bool allow_build_response;
        std::string build_html_response();
        std::string build_html_response_wo_content_length();

        void redirect(const std::string &url);


        void makePersistent();
//    Response();
        virtual ~Response();

        IInstance *iInstance;
    };
};

