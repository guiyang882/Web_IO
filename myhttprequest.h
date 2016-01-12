#ifndef MYHTTPREQUEST_H
#define MYHTTPREQUEST_H

#include "utils.h"
#include <thread>
#include <mutex>
#include <algorithm>

enum LOG_LEVEL{
    LOG_DEBUG,
    LOG_INFO,
    LOG_ERROR
};

class MyHttpRequest
{
public:
    MyHttpRequest() {
        set_debug_path("debug.log");
        set_info_path("info.log");
        set_error_path("error.log");
    }

    ~MyHttpRequest() {
        out_debug.close();
        out_info.close();
        out_error.close();
    }

    void set_debug_path(string path) {
        log_file_debug = path;
        out_debug.open(log_file_debug,ios_base::out);
        if(out_debug.is_open() == false){
            cout << "Open debug file error !" << endl;
            exit(0);
        }
    }

    void set_info_path(string path) {
        log_file_info = path;
        out_info.open(log_file_info,ios_base::out);
        if(out_info.is_open() == false){
            cout << "Open info file error !" << endl;
            exit(0);
        }
    }

    void set_error_path(string path) {
        log_file_error = path;
        out_error.open(log_file_error,ios_base::out);
        if(out_error.is_open() == false){
            cout << "Open error file error !" << endl;
            exit(0);
        }
    }

public:
    bool instance_req(string url,int sleep_time = 50,int n_thread = 50,int n_visit = 100);
    bool instance_req_header(string host,int port,string prefix="/",string send_type="GET",string post_data = "");

    void get_url_ip(string url,vector<string>& res);
    void send_and_recv(int sockfd, string save_file);

    bool pressure_testing(string url,int sleep_time = 50,int n_thread = 10,int n_visit = 50);

    bool download_url(string host,string url,string save_file);
    void single_connection(int id);

public:
    void demo_download_baidu();
    void demo_get_query();
    void demo_send_type(string send_type);

protected:
    void write_log(LOG_LEVEL log_level,string info){
        std::lock_guard<std::mutex> lck (write_mtx);

        struct tm *ptr;
        time_t lt;
        lt =time(NULL);
        ptr=gmtime(&lt);
        string now_time(asctime(ptr));

        string head_format;
        switch (log_level) {
        case LOG_DEBUG:
            head_format = "DEBUG: " + now_time + " ";
            out_debug << head_format + info << endl;
            break;
        case LOG_ERROR:
            head_format = "ERROR: " + now_time + " ";
            out_error << head_format + info << endl;
            break;
        case LOG_INFO:
            head_format = "INFO: " + now_time + " ";
            out_info << head_format + info << endl;
            break;
        }
    }

private:
    Request req;
    RequestHeader reqHeader;
    string log_file_debug;
    string log_file_info;
    string log_file_error;

    fstream out_error;
    fstream out_info;
    fstream out_debug;

    std::mutex write_mtx;

    vector<vector<double> > resp_time;
};

#endif // MYHTTPREQUEST_H
