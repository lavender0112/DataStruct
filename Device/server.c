#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//./server ip port
int main(int argc, const char *argv[])
{
	int connect_fd;
	int listen_fd;
	struct sockaddr_in server_addr;
	struct sockaddr_in peer_addr;
	
	//创建流式套接字
	if((listen_fd = socket(AF_INET,SOCK_STREAM,0)) < 0)
	{
		perror("Fail to socket ");
		exit(EXIT_FAILURE);
	}
	
	bzero(&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port   = htons( atoi(argv[2]) );
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	//绑定IP和Port
	if(bind(listen_fd,(struct sockaddr *)&server_addr,sizeof(struct sockaddr)) < 0)
	{
		perror("Fail to bind");
		exit(EXIT_FAILURE);
	}
	
	//设置套接字设为监听模式
	listen(listen_fd,5);

	printf("Listen ...\n");
	
	int addr_len = sizeof(struct sockaddr);
	
	char buf[1024];
	int n;

		//提起请求
		connect_fd = accept(listen_fd,(struct sockaddr *)&peer_addr,&addr_len);
		if(connect_fd < 0)
		{
			perror("Fail to accept");
			exit(EXIT_FAILURE);
		}

		while(1)
		{
			printf("***********************\n");
			printf("Ip : %s.\n",inet_ntoa(peer_addr.sin_addr));
			printf("Port: %d.\n",ntohs(peer_addr.sin_port));
			printf("***********************\n");
			n = read(connect_fd,buf,sizeof(buf) - 1);
			buf[n] = '\0';

			printf("Read %d bytes : %s.\n",n,buf);
		}
		
	exit(EXIT_SUCCESS);
}
