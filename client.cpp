/*************************************************************************
	> File Name: client.cpp
	> Author: 
	> Mail: 
	> Created Time: 2015年12月30日 星期三 15时29分51秒
 ************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <iostream>
#include <unistd.h>
#include <string>

using namespace std;


#define MAXLINE 80
#define SERV_PORT 8000
const string ip = "127.0.0.1";

int main(int argc, char *argv[])
{
	struct sockaddr_in servaddr;
	char buf[MAXLINE];
	int sockfd, n;
	string str;
    cout << "Say to Server:" << endl; 
    cin >>str;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET, ip.c_str(), &servaddr.sin_addr);
	servaddr.sin_port = htons(SERV_PORT);
	connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	
    write(sockfd, str.c_str(), str.size());

	n = read(sockfd, buf, MAXLINE);
    cout << "Response from server:" << string(buf,n) << endl;

	close(sockfd);
	return 0;
}
