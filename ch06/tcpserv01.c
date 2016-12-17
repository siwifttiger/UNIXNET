#include 	"unp.h"

int main(int argv, char **argc)
{
	int listenfd,connfd;
	struct sockaddr_in servaddr, cliaddr;
	pid_t childpid;	
	socklen_t clilen;
	void sig_child(int signo);
	
	bzero(&servaddr, sizeof(servaddr));
	listenfd = Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port	= htons(SERV_PORT);
	
	Bind(listenfd, (SA*) &servaddr, sizeof(servaddr));
	Listen(listenfd, LISTENQ);
	Signal(SIGCHLD,sig_child);

	for(; ;)	
	{
		clilen = sizeof(cliaddr);
		if( (connfd = accept(listenfd, (SA*)&cliaddr, &clilen )) < 0)
		{
			if(errno == EINTR)
				continue;
			else
				err_sys("accept error");
		}
		if((childpid = Fork()) == 0) {
			Close(listenfd);
			printf("child process %d started\n", childpid);
			str_echo(connfd);
			exit(0);
		}
		Close(connfd);
	}
}
