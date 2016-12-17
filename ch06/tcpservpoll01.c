#include	"unp.h"
#define OPEN_MAX 1024

int main(int argv, char **argc)
{
	int i, maxi, listenfd, connfd, sockfd;
	int nready;
	struct sockaddr_in servaddr, cliaddr;
	socklen_t clilen;
	char buf[MAXLINE];
	ssize_t n;
	struct pollfd client[OPEN_MAX];
	
	bzero(&servaddr, sizeof(servaddr));	
	listenfd = Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);
	
	Bind(listenfd, (SA*)&servaddr, sizeof(servaddr));
	Listen(listenfd, LISTENQ);

	client[0].fd = listenfd;
	client[0].events = POLLRDNORM;
	for(i = 1; i < OPEN_MAX; ++i)
		client[i].fd = -1;
	maxi = 0;

	for( ; ; )
	{
		nready = Poll(client, maxi + 1, INFTIM);
		if(client[0].revents & POLLRDNORM)
		{
			clilen = sizeof(cliaddr);
			connfd = Accept(listenfd, (SA*)&cliaddr, &clilen);
			
			for(i = 1; i < OPEN_MAX; ++i){
				if(client[i].fd < 0) {
					client[i].fd = connfd;
					break;
				}	
				
			}
			
			if( i == OPEN_MAX)
				err_quit("too many clients");
			if(maxi < i)
				maxi = i;
			
			client[i].events = POLLRDNORM;
			
			if( --nready <= 0) 
				continue;
		}
		
		for(i = 1; i <= maxi; ++ i)
		{
			if( (sockfd = client[i].fd) < 0 )
				continue;
				
			if(client[i].revents & (POLLRDNORM | POLLERR)){
				if( (n = read(sockfd, buf, MAXLINE)) < 0 ){
					if ( errno == ECONNRESET){
						//connection reset by client
						Close(sockfd);
						client[i].fd = -1;
					}else
						err_sys("read error");
				}else if( n == 0) {   //connection closed by client
					Close(sockfd);
					client[i].fd = -1;
				}else 
					Writen(sockfd, buf, n);
				if(--nready <= 0)
					break;
			}
		}	
	}
}
