#include	"unp.h"
#include	<sys/epoll.h>
#define EPOLLSIZE 1024


int main(int argc, char **argv)
{
	int efd,listenfd,connfd,sockfd;
	int nfds;		//就绪的文件描述符个数
	char buf[MAXLINE];
	struct sockaddr_in servaddr, cliaddr;
	socklen_t clilen;
	ssize_t n;
	struct epoll_event ectl;                     //用于注册
	struct epoll_event events[256];                 //epoll_wait 返回的事件集合
	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port	= htons(SERV_PORT);
	
	listenfd = Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	Bind(listenfd, (SA*)&servaddr, sizeof(servaddr));
	
	Listen(listenfd, LISTENQ);
	//设置相关事件和文件描述符
	ectl.data.fd = listenfd;
	ectl.events  =  EPOLLIN;               //读取模式
	
	efd = epoll_create(EPOLLSIZE);         //创建epoll句柄
	if(efd == -1){
		perror("epoll_create");
		return 0;
	}
	//epoll注册，将socket加入到efd中，注册要监听的事件类型
	int s = epoll_ctl(efd, EPOLL_CTL_ADD,listenfd, &ectl);
	if(s == -1)
	{
		perror("epoll_ctl");
		return 0;
	}
	

	for(;;)
	{
		nfds = epoll_wait(efd,events,20,-1);
		for(int i = 0; i < nfds; ++i){
			if(events[i].data.fd == listenfd) {   //有新的连接
				clilen = sizeof(cliaddr);
				connfd = Accept(listenfd, (SA*)&clilen, &clilen);
				//打印新连接的ip 和 端口
				printf("connection from %s, port %d\n", Inet_ntop(AF_INET,(void*)&cliaddr.sin_addr,buf,sizeof(buf)),
					ntohs(cliaddr.sin_port));
				
				//设置用于读数据的文件描述符和事件
				ectl.data.fd = connfd;
				ectl.events = EPOLLIN;
				
				//重新注册
				s = epoll_ctl(efd,EPOLL_CTL_ADD,connfd,&ectl);
				if(s == -1){
					perror("epoll_ctl");
					return 0;
				}
			} else if(events[i].events & EPOLLIN) {   //已连接用户，文件描述符准备好读
				sockfd = events[i].data.fd;
				if(sockfd < 0)
					continue;
				if( (n = read(sockfd,buf,MAXLINE)) < 0 ){
					if(errno == ECONNRESET) {
						//connection reset by client
						Close(sockfd);
						events[i].data.fd = -1;
					}else 
						err_sys("read error");
				} else if (n == 0) {
					// connection closed by client
					Close(sockfd);
					events[i].data.fd = -1;
				} else {
					//等待下一个循环时写数据
					ectl.data.fd = sockfd;
					ectl.events  = EPOLLOUT;
					epoll_ctl(efd,EPOLL_CTL_MOD,sockfd,&ectl);
				}	
			} else if(events[i].events & EPOLLOUT) {  //文件描述符准备好写
				sockfd = events[i].data.fd;
				Writen(sockfd,buf,n);
				ectl.data.fd = sockfd;
				ectl.events  = EPOLLIN;
				epoll_ctl(efd,EPOLL_CTL_MOD,sockfd,&ectl);
			}
		}
	}
		
	return 0;
}
