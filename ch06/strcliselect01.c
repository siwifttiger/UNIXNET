#include	"unp.h"

void str_cli(FILE *fp, int sockfd)
{
	int	maxfdp1;
	fd_set	rset;
	char sendline[MAXLINE], recvline[MAXLINE];
	
	FD_ZERO(&rset);
	for(; ; ) {
		FD_SET(fileno(fp),&rset);
		FD_SET(sockfd,&rset);
		maxfdp1 = max(fileno(fp),sockfd) + 1;
		Select(maxfdp1,&rset,NULL,NULL,NULL);
		
		if (FD_ISSET(sockfd,&rset)) {    //套接字描述符读就绪
			if (Readline(sockfd,recvline,MAXLINE) == 0)
				err_quit("str_cli: server terminated prematureluy");
			Fputs(recvline,stdout);
		}	

		if (FD_ISSET(fileno(fp),&rset))  {  //输入描述符准备好读
			if (Fgets(sendline,MAXLINE,fp) == NULL)
				return;
			Writen(sockfd,sendline,strlen(sendline));	
		}
	}
}
