#include "myhttprequest.h"

bool MyHttpRequest::instance_req(string url, int sleep_time, int n_thread, int n_visit ){
    vector<string> res_ip;
    get_url_ip(url,res_ip);
    if(res_ip.size() == 0)
        return false;
    req.ip = res_ip[0];
    req.num_thread = n_thread;
    req.num_visit = n_visit;
    req.url = url;
    req.port = 80;
    req.sleep_time = sleep_time;
    req.resp_time.clear();
    return true;
}

bool MyHttpRequest::instance_req_header(string host, int port, string prefix, string send_type, string post_data){
    if(send_type == "POST"){
        reqHeader.send_type = "POST";
        if(post_data.size() == 0){
            return false;
        }
    }else{
        reqHeader.send_type = "GET";
    }

    if(prefix == "/"){
        reqHeader.prefix = "/";
    }else{
        reqHeader.prefix = prefix;
    }

    reqHeader.host = host;
    if(port>0){
        reqHeader.full_header = reqHeader.send_type + " " +
                reqHeader.prefix + " HTTP/1.1\r\nHost: " +
                reqHeader.host + ":"+to_string(port)+"\r\nConnection: Close\r\n";
    }else{
        reqHeader.full_header = reqHeader.send_type + " " +
                reqHeader.prefix + " HTTP/1.1\r\nHost: " +
                reqHeader.host +"\r\nConnection: Close\r\n";
    }

    reqHeader.full_header = reqHeader.full_header + "User-Agent: " + reqHeader.user_agent + "\r\n";
    if(send_type == "POST"){
        stringstream ss;
        int post_len = post_data.size();
        ss << post_len;
        string str_len = ss.str();
        reqHeader.full_header = reqHeader.full_header + "Content-Type: application/x-www-form-urlencoded\r\nContent-Length: " + str_len + "\r\n\r\n" + post_data;
    }
    if(send_type == "GET"){
        reqHeader.full_header += "\r\n";
    }
    return true;
}

void MyHttpRequest::get_url_ip(string url,vector<string>& res){
    res.clear();
    char **pptr;
    struct hostent *hptr;
    char str[32];
    /* 取得命令后第一个参数，即要解析的域名或主机名 */
    /* 调用gethostbyname()。调用结果都存在hptr中 */
    if((hptr = gethostbyname(url.c_str()) ) == NULL){
        cout << "gethostbyname error for host:" <<  url << endl;
        write_log(LOG_ERROR,"gethostbyname error for host:" + url);
        return ; /* 如果调用gethostbyname发生错误，返回1 */
    }
    /* 将主机的规范名打出来 */
    cout << "official hostname: " << hptr->h_name << endl;
    write_log(LOG_INFO,"official hostname: "+string(hptr->h_name));
    /* 主机可能有多个别名，将所有别名分别打出来 */
    for(pptr = hptr->h_aliases; *pptr != NULL; pptr++){
        cout << "alias: " << *pptr << endl;
        write_log(LOG_INFO,"alias: " + string(*pptr));
    }
    /* 根据地址类型，将地址打出来 */
    switch(hptr->h_addrtype)
    {
    case AF_INET:
    case AF_INET6:
        pptr=hptr->h_addr_list;
        /* 将刚才得到的所有地址都打出来。其中调用了inet_ntop()函数 */
        for(;*pptr!=NULL;pptr++){
            const char* t_ip = inet_ntop(hptr->h_addrtype, *pptr, str, sizeof(str));
            res.push_back(string(t_ip));
            cout << " address: " << t_ip << endl;
        }
        break;
    default:
        cout << "unknown address type" << endl;
        break;
    }
}

void MyHttpRequest::send_and_recv(int sockfd,string save_file){
    write(sockfd, reqHeader.full_header.c_str(), reqHeader.full_header.size());
    cout << reqHeader.full_header << endl;
    cout << "\t***********************Request Header************************" << endl;

    char * response = (char *) malloc (1024 * sizeof(char));
    if(save_file.size() != 0) {
        int file = open(save_file.c_str(), O_CREAT|O_RDWR);
        int length;
        do {
            length = read(sockfd, response, 1024);
            char * loc = strstr(response, "\r\n\r\n"); //截获返回头部，以\r\n\r\n为标识
            if(loc) {
                int loci = loc - response + 4;
                write(1, response, loci);//如果是响应头部就打印至屏幕
                write(file, loc, length - loci);//如果是响应主体就写入文件
            } else {
                write(file, response, length);
            }
            if(!length)//注意，因为之前采用的是close方法，也就是说一旦传输数据完毕，则服务器端会断开链接，则read函数会返回0，所以这里 会退出循环。如果采用的是Keep-Alive则服务器不关闭TCP链接，也就说程序将会被阻塞在read函数中，因此要注意的是自己判断是否读到了响应 的结尾，然后在再次调用read之前退出循环。
                break;
        } while(1);
        close(file);
    } else {
        int length;
        do {
            length = read(sockfd, response, 1024);
            cout << response << endl;
            if(!length)
                break;
        } while(1);
    }
    free(response);
}

bool MyHttpRequest::pressure_testing(string url,int sleep_time,int n_thread,int n_visit){
    bool flag = instance_req(url,sleep_time,n_thread,n_visit);
    if(flag == false){
        perror("Some Error, In pressure testing !");
        return false;
    }
    vector<std::thread> pool_conn;
    for(int i=0;i<n_thread;i++){
        vector<double> id_time;
        resp_time.push_back(id_time);
        pool_conn.push_back(thread(&MyHttpRequest::single_connection,this,i+1));
    }
    for(int i=0;i<n_thread;i++){
        pool_conn[i].join();
    }
    for(size_t i=0;i<resp_time.size();i++){
        string info = "Thread " + to_string(i+1) + " : ";
        for(size_t j=0;j<resp_time[i].size();j++){
            info += (doubleToString(resp_time[i][j]) + " ");
        }
        write_log(LOG_INFO,info);
    }
    //计算相应的各类指标
    for(int i=0;i<resp_time.size();i++){
        cout << "Thread " << i+1 << endl;
        sort(resp_time[i].begin(),resp_time[i].end());
        cout << "\tMax visit time: " << resp_time[i][resp_time[i].size()-1] << " ms" << endl;
        cout << "\tMin visit time: " << resp_time[i][0] << " ms" << endl;
        double avg_visit = accumulate((resp_time[i]).begin(),(resp_time[i]).end(),0) / (double)(resp_time[i]).size();
        cout << "\tAvg visit time: " << avg_visit << " ms" << endl;
    }
    return true;
}

/*
 * host : www.baidu.com
 * url : http://www.baidu.com
*/
bool MyHttpRequest::download_url(string host,string url,string save_file){
    int sockfd;
    struct sockaddr_in serv_socket;
    int port = 80;
    vector<string> vec_ip;
    get_url_ip(host,vec_ip);
    if(vec_ip.size() == 0){
        perror("can't find the url's ip !\n");
        return false;
    }
    string ip = vec_ip[0];

    bzero(&serv_socket, sizeof(struct sockaddr_in));
    serv_socket.sin_family = AF_INET;
    serv_socket.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &serv_socket.sin_addr);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    int flag = connect(sockfd, (struct sockaddr *)&serv_socket, sizeof(serv_socket)); //建立和HTTP服务器的TCP链接
    if(flag < 0) {
        printf("connect error!!! flag = %d\n", flag);
        return false;
    }
    instance_req_header(ip,port,url);
    send_and_recv(sockfd,save_file);
    close(sockfd);
    return true;
}

void MyHttpRequest::demo_download_baidu() {
    string host = "blog.csdn.net";
    string url = "http://blog.csdn.net/huangxy10/article/details/7662322";
    string save_file = "/home/fighter/Documents/temp.html";
    download_url(host,url,save_file);
}

void MyHttpRequest::demo_get_query(){
    string host = "bj.meituan.com";
    string prefix = "/s/?w=%E4%BD%A0%E5%A5%BD%E6%BC%82%E4%BA%AE&mtt=1.index%2Ffloornew.0.0.ijb2krcl";
    int port = 80;
    string save_file = "/home/fighter/Documents/temp.html";

    int sockfd;
    struct sockaddr_in serv_socket;
    vector<string> vec_ip;
    get_url_ip(host,vec_ip);
    if(vec_ip.size() == 0){
        perror("can't find the url's ip !\n");
        exit(1);
    }
    string ip = vec_ip[0];

    bzero(&serv_socket, sizeof(struct sockaddr_in));
    serv_socket.sin_family = AF_INET;
    serv_socket.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &serv_socket.sin_addr);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    int flag = connect(sockfd, (struct sockaddr *)&serv_socket, sizeof(serv_socket)); //建立和HTTP服务器的TCP链接
    if(flag < 0) {
        printf("connect error!!! flag = %d\n", flag);
        exit(1);
    }
    instance_req_header(host,-1,prefix);
    send_and_recv(sockfd,save_file);
    close(sockfd);
    exit(0);
}

void MyHttpRequest::send_data_diff_type(string send_type,string ip,int port,string prefix,string content){
    int sockfd;
    struct sockaddr_in serv_socket;
    string file_loc = "";
    bzero(&serv_socket, sizeof(struct sockaddr_in));
    serv_socket.sin_family = AF_INET;
    serv_socket.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &serv_socket.sin_addr);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    int flag = connect(sockfd, (struct sockaddr *)&serv_socket, sizeof(serv_socket)); //建立和HTTP服务器的TCP链接
    if(flag < 0) {
        perror("connect error!!!");
        exit(1);
    }
    string post_data;
    if(send_type == "POST"){
        post_data = content;
    }else{
        prefix = prefix + "?" +UrlEncode(content);
        post_data = "";
    }
    instance_req_header(ip,port,prefix,send_type,post_data);
    send_and_recv(sockfd,file_loc);
    close(sockfd);
    exit(0);
}

void MyHttpRequest::single_connection(int id){
    int sockfd;
    struct sockaddr_in serv_socket;
    bzero(&serv_socket, sizeof(struct sockaddr_in));
    serv_socket.sin_family = AF_INET;
    serv_socket.sin_port = htons(req.port);
    inet_pton(AF_INET, req.ip.c_str(), &serv_socket.sin_addr);

    clock_t start, finish;
    double duration;
    for(int i=0;i<req.num_visit;i++){
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        start = clock();
        int flag = connect(sockfd, (struct sockaddr *)&serv_socket, sizeof(serv_socket)); //建立和HTTP服务器的TCP链接
        finish = clock();
        duration = (double)(finish - start);
        resp_time[id-1].push_back(duration);
        if(flag < 0) {
            cout << "connect " << req.url << " failed !" << endl;
        }
        usleep(req.sleep_time * 1000);
        close(sockfd);
    }
    double avg_visit = accumulate((resp_time[id-1]).begin(),(resp_time[id-1]).end(),0) / (double)(resp_time[id-1]).size();
    write_log(LOG_INFO,"Thread "+ to_string(id) + "avg visit time " + doubleToString(avg_visit) + " ms");
}
