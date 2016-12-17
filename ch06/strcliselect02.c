#include	"unp.h"

void str_cli(FILE *fp, int sockfd)
{
	int maxfdp1, stdineof;   //flag of end of std input
	fd_set rset;
	char buf[MAXLINE];
	int n;
	
	stdineof = 0;
	FD_ZERO(&rset);
	for( ; ; )
	{
		if(stdineof == 0)    //表示可读
			FD_SET(fileno(fp), &rset);
		FD_SET(sockfd, &rset);
		maxfdp1 = max(fileno(fp), sockfd) + 1;
		Select(maxfdp1,&rset, NULL,NULL,NULL);
		if(FD_ISSET(sockfd, &rset))  {  //socket is readable
			if( (n = Read(sockfd,buf,MAXLINE)) == 0 ) {
				if(stdineof == 1)     //正常结束 
					return;		
			
				else 
					err_quit("st_cli: server terminated prematurely");		
			}
			Writen(fileno(stdout),buf, n);
		}

		if(FD_ISSET(fileno(fp), &rset) ) {      //输入准备好读
			if( (n = Read(fileno(fp), buf, MAXLINE)) == 0 ) {
				stdineof = 1;      //标志设为1，记为读结束
				Shutdown(sockfd,SHUT_WR);   //send FIN, 结束可写能力
				FD_CLR(fileno(fp), &rset);  //清楚输入描述符
				continue;
			}
			Writen(sockfd, buf, n);
		}		

	}
	
}
