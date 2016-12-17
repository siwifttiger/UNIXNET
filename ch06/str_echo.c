#include	"unp.h"

void str_echo(int fd)
{
	char recv[MAXLINE];
	ssize_t	n;
again:
	while((n = read(fd, recv, MAXLINE)) > 0) {
		Writen(fd, recv, n);
		Fputs(recv, stdout);
	}
	if( n < 0 && errno == EINTR)
		goto again;
	else if(n < 0)
		err_sys("str_echo: read error");
	
}
