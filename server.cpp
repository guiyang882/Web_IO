/*************************************************************************
	> File Name: client.cpp
	> Author: 
	> Mail: 
	> Created Time: 2015年12月30日 星期三 15时29分51秒
 ************************************************************************/

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <ctype.h>

#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h>

using namespace std;


#define MAXLINE 80
#define SERV_PORT 8000

int main()
{
	struct sockaddr_in servaddr, cliaddr;
	socklen_t cliaddr_len;
	int listenfd, connfd;
	char buf[MAXLINE];
	char str[INET_ADDRSTRLEN];
	int i, n;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);
    
	bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

	listen(listenfd, 20);

	cout << "Accepting connections..." << endl;
	while (1) {
		cliaddr_len = sizeof(cliaddr);
		connfd = accept(listenfd, 
				(struct sockaddr *)&cliaddr, &cliaddr_len);
	  
		n = read(connfd, buf, MAXLINE);
		cout << "received from " \
             << inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)) \
             << ntohs(cliaddr.sin_port) << endl;
    
		for (i = 0; i < n; i++)
			buf[i] = toupper(buf[i]);
		write(connfd, buf, n);
		close(connfd);
	}
    return 0;
}
