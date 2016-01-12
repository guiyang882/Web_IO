#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <thread>
#include <string>
#include <vector>
#include <numeric>

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

typedef struct _RequestHeader{
    string send_type = "GET";
    string prefix = "/";
    string host;
    string port;
    string connection = "keep-alive";
    string cache_control = "max-age=0";
    string accept = "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8";
    string user_agent = "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Ubuntu Chromium/47.0.2526.73 Chrome/47.0.2526.73 Safari/537.36";
    string accept_encoding = "gzip, deflate, sdch";
    string accept_language = "en-US,en;q=0.8";
    string referer;
    string full_header;// save the complete header
}RequestHeader;

unsigned char ToHex(unsigned char x);
unsigned char FromHex(unsigned char x);
std::string UrlEncode(const std::string& str);
std::string UrlDecode(const std::string& str);


string doubleToString(double d);



#endif // UTILS_H
