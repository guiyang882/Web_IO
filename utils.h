#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <sstream>
#include <thread>
#include <string>
#include <vector>

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/in.h>

#include <netdb.h>
#include <fcntl.h>
#include <ctype.h>

#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

using namespace std;

typedef struct _Request{
    string url;
    string ip;
    unsigned int port;
    int sleep_time;//ms, which means to sleep after close the connection
    int num_thread;// how many thread
    int num_visit;// for each thread, the visit number
    vector<double> resp_time;
}Request;

/*
GET / HTTP/1.1
Host: www.csdn.net
Connection: keep-alive
Cache-Control: max-age=0
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*\/*;q=0.8
Upgrade-Insecure-Requests: 1
User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Ubuntu Chromium/47.0.2526.73 Chrome/47.0.2526.73 Safari/537.36
Accept-Encoding: gzip, deflate, sdch
Accept-Language: en-US,en;q=0.8
*/

/*
POST /myproject/infoUpdate HTTP/1.1
Host: 192.168.52.250:8088
User-Agent: Mozilla/5.0 (Windows NT 6.1; rv:14.0) Gecko/20100101 Firefox/14.0.1
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*\/*;q=0.8
Accept-Language: zh-cn,zh;q=0.8,en-us;q=0.5,en;q=0.3
Accept-Encoding: gzip, deflate
Connection: keep-alive
Content-Type: application/x-www-form-urlencoded
Content-Length: 24

infoId=3&currentPage=1
*/
typedef struct _RequestHeader{
    string send_type = "GET";
    string prefix = "/";
    string host;
    string port;
    string connection = "keep-alive";
    string cache_control = "max-age=0";
    string accept = "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8";
    int upg_insecure_req = 1;
    string user_agent = "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Ubuntu Chromium/47.0.2526.73 Chrome/47.0.2526.73 Safari/537.36";
    string accept_encoding = "gzip, deflate, sdch";
    string accept_language = "en-US,en;q=0.8";
    string referer;
    string full_header;// save the complete header
}RequestHeader;

/*
GET http://www.baidu.com/ HTTP/1.1
Accept: html/text
Host: 61.135.169.121:80
Connection: Close
*/
void instance_req_header(RequestHeader& reqHeader,string host,string port,string prefix="/",string send_type="GET",string post_data = "");

void get_url_ip(string url,vector<string>& res);
/*
 * sockfd表示TCP链接的套接字，url请求服务的相对或者绝对地址，fun_type请求方法，accept_type接受类型，
 * ip,port请求的服务器的地址和端口，file_loc下载文件存放位置，body请求的主体，
 * connection_type用来指定 connection的类型
*/
void send_and_recv(int sockfd,RequestHeader& req,string save_file);
void demo_download_baidu();
void demo_login_csdn();
bool instance_req(Request& req,string url,int sleep_time = 50,int n_thread = 50,int n_visit = 100);
void single_connection(Request& req);
bool pressure_testing(string url,int sleep_time = 50,int n_thread = 50,int n_visit = 100);


unsigned char ToHex(unsigned char x);
unsigned char FromHex(unsigned char x);
std::string UrlEncode(const std::string& str);
std::string UrlDecode(const std::string& str);

#endif // UTILS_H

