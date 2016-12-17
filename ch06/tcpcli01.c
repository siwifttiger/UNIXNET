#include	"unp.h"

void str_cli(FILE *fp, int sockfd);

int main(int argc, char **argv)
{
	int sockfd;
	struct sockaddr_in servaddr;
	if(argc != 2)
		err_quit("usage: tcpcli <IPaddress>");
	
	bzero(&servaddr, sizeof(servaddr));
	sockfd = Socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);	

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	
	Inet_pton(AF_INET,argv[1], &servaddr.sin_addr);
	Connect(sockfd,(SA*)&servaddr,sizeof(servaddr));
	
	str_cli(stdin, sockfd);
}
