#include <iostream>
#include <thread>
#include <unistd.h>
#include <string>
#include <vector>

#include "utils.h"
#include "myhttprequest.h"
#include "cmdline.h"

using namespace std;

int main(int argc, char *argv[])
{
    cmdline::parser a;
    a.add<string>("host", 'h', "host name", false, "www.baidu.com");
    a.add<int>("port", 'p', "port number", false, 80, cmdline::range(1, 65535));
    a.add<int>("delay", 'd', "each connect delay time ms", false, 50, cmdline::range(1, 65535));
    a.add<int>("n_users", 'n', "user number", false, 50, cmdline::range(1, 2000));
    a.add<int>("n_visit", 'v', "each user visit number", false, 100, cmdline::range(1, 65535));
    a.add<string>("url", 'u', "url address", false, "http://www.baidu.com");
    a.add<string>("savefile", 's', "download file save path", false, "temp.html");
    a.add<string>("type", 't', "data send type", false, "GET", cmdline::oneof<string>("GET","POST"));
    a.add<string>("mode",'m',"pressure tesing mode",true,"connect",cmdline::oneof<string>("connect","download","sendtype"));
    a.parse_check(argc, argv);

    string mode = a.get<string>("mode");
    MyHttpRequest obj;
    if(mode == "connect"){
        string host = a.get<string>("host");
        int port = a.get<int>("port");
        int n_users = a.get<int>("n_users");
        int n_visit = a.get<int>("n_visit");
        int delay = a.get<int>("delay");
        obj.pressure_testing(host,delay,n_users,n_visit);
    }
    if(mode == "download"){
        string host = a.get<string>("host");
        string url = "http://blog.csdn.net/huangxy10/article/details/7662322";
        url = a.get<string>("url");
        string save_file = a.get<string>("savefile");
        obj.download_url(host,url,save_file);
    }
    if(mode == "sendtype"){
        
    }

    /*
  MyHttpRequest obj;

  obj.demo_download_baidu();
  obj.demo_get_query();
  */
    /*
   *
  obj.demo_send_type("GET");
  obj.demo_send_type("POST");
  */
    return 0;
}
