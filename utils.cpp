#include "utils.h"

void get_url_ip(string url,vector<string>& res){
    res.clear();
    char **pptr;
    struct hostent *hptr;
    char str[32];
    /* 取得命令后第一个参数，即要解析的域名或主机名 */
    /* 调用gethostbyname()。调用结果都存在hptr中 */
    if((hptr = gethostbyname(url.c_str()) ) == NULL){
        cout << "gethostbyname error for host:" <<  url << endl;
        return ; /* 如果调用gethostbyname发生错误，返回1 */
    }
    /* 将主机的规范名打出来 */
    cout << "official hostname: " << hptr->h_name << endl;
    /* 主机可能有多个别名，将所有别名分别打出来 */
    for(pptr = hptr->h_aliases; *pptr != NULL; pptr++)
        cout << "alias: " << *pptr << endl;
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

void demo_download_baidu() {
    int sockfd;
    struct sockaddr_in serv_socket;
    int port = 80;
    string url = "www.baidu.com";
    vector<string> vec_ip;
    get_url_ip(url,vec_ip);
    if(vec_ip.size() == 0)
    {
        perror("can't find the url's ip !\n");
        exit(1);
    }
    string ip = vec_ip[0];
    string file_loc = "/home/fighter/temp.html"; //下载的存放位置

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
    RequestHeader req;
    instance_req_header(req,ip,"80","http://www.baidu.com/");
    send_and_recv(sockfd,req,file_loc);
    close(sockfd);
    exit(0);
}

void demo_login_csdn(){
    int sockfd;
    struct sockaddr_in serv_socket;
    int port = 443;
    string url = "passport.csdn.net";
    vector<string> vec_ip;
    get_url_ip(url,vec_ip);
    if(vec_ip.size() == 0)
    {
        perror("can't find the url's ip !\n");
        exit(1);
    }
    string ip = vec_ip[0];
    string file_loc = "/home/fighter/Documents/temp.html";

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
    //string post_data = UrlEncode("username=liuguiyangnwpu@163.com&password=10152301150097");
    string post_data = "username=liuguiyangnwpu@163.com&password=10152301150097&lt=LT-119961-RPAHq7K7b5nd90p6hnRD3yrnzOZBWy&execution=e13s1&_eventId=submit";
    RequestHeader req;
    instance_req_header(req,ip,"443","/account/login?from=http://my.csdn.net/my/mycsdn","POST",post_data);
    send_and_recv(sockfd,req,file_loc);
    close(sockfd);
    exit(0);
}

void send_and_recv(int sockfd,RequestHeader& req,string save_file){
    write(sockfd, req.full_header.c_str(), req.full_header.size());
    cout << req.full_header << endl;
    cout << "Above is request !" << endl;

    char * response = (char *) malloc (1024 * sizeof(char));
    if(save_file.size() != 0) {
        int file = open(save_file.c_str(), O_RDWR | O_APPEND);
        cout << "Open Save File " << save_file << file << endl;
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
        cout << "Save Here !!" << endl;
    } else {
        int length;
        do {
            length = read(sockfd, response, 1024);
            printf("%s", response);
            if(!length)
                break;
        } while(1);
    }
    free(response);
}

/*
GET http://www.baidu.com/ HTTP/1.1
Accept: html/text
Host: 61.135.169.121:80
Connection: Close
*/
void instance_req_header(RequestHeader& reqHeader,string host,string port,string prefix,string send_type,string post_data){
    if(send_type == "POST"){
        reqHeader.send_type = "POST";
        if(post_data.size() == 0){
            return ;
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
    reqHeader.full_header = reqHeader.send_type + " " +
            reqHeader.prefix + " HTTP/1.1\r\nHost: " +
            reqHeader.host + ":"+port+"\r\nConnection: Close\r\n";
    reqHeader.full_header = reqHeader.full_header + "User-Agent: " + reqHeader.user_agent + "\r\n";
    if(send_type == "POST"){
        reqHeader.full_header = reqHeader.full_header + "Referer: https://passport.csdn.net/account/login?from=http://my.csdn.net/my/mycsdn\r\n";
        stringstream ss;
        int post_len = post_data.size() + 1;
        ss << post_len;
        string str_len = ss.str();
        reqHeader.full_header = reqHeader.full_header +
                "Content-Type: application/x-www-form-urlencoded\r\n\Content-Length: " + str_len + "\r\n\r\n" + post_data;
    }
    if(send_type == "GET"){
        reqHeader.full_header += "\r\n";
    }
}

bool instance_req(Request& req, string url,int sleep_time,int n_thread,int n_visit ){
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

void single_connection(Request &req){
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
        req.resp_time.push_back(duration);
        if(flag < 0) {
            cout << "connect " << req.url << " failed !" << endl;
        }
        usleep(req.sleep_time * 1000);
        close(sockfd);
    }
}

bool pressure_testing(string url,int sleep_time,int n_thread,int n_visit){
    Request req;
    bool flag = instance_req(req,url,sleep_time,n_thread,n_visit);
    if(flag == false){
        perror("Some Error, In pressure testing !");
        return false;
    }
    vector<std::thread> pool_conn;
    for(int i=0;i<n_thread;i++){
        pool_conn.push_back(thread(single_connection,std::ref(req)));
    }
    for(int i=0;i<n_thread;i++){
        pool_conn[i].join();
    }
    cout << "Here !!" << endl;
    return true;
}

unsigned char ToHex(unsigned char x){
    return  x > 9 ? x + 55 : x + 48;
}

unsigned char FromHex(unsigned char x){
    unsigned char y;
    if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
    else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
    else if (x >= '0' && x <= '9') y = x - '0';
    else assert(0);
    return y;
}

std::string UrlEncode(const std::string& str){
    std::string strTemp = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++)
    {
        if (isalnum((unsigned char)str[i]) ||
            (str[i] == '-') ||
            (str[i] == '_') ||
            (str[i] == '.') ||
            (str[i] == '~'))
            strTemp += str[i];
        else if (str[i] == ' ')
            strTemp += "+";
        else
        {
            strTemp += '%';
            strTemp += ToHex((unsigned char)str[i] >> 4);
            strTemp += ToHex((unsigned char)str[i] % 16);
        }
    }
    return strTemp;
}

std::string UrlDecode(const std::string& str){
    std::string strTemp = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++){
        if (str[i] == '+')
            strTemp += ' ';
        else if (str[i] == '%'){
            assert(i + 2 < length);
            unsigned char high = FromHex((unsigned char)str[++i]);
            unsigned char low = FromHex((unsigned char)str[++i]);
            strTemp += high*16 + low;
        }
        else
            strTemp += str[i];
    }
    return strTemp;
}
