CC = gcc
CFLAGS = -g -O2 -Wall
LIBS = -lunp -lpthread
OBJS = str_cli.o str_echo.o signal.o sigchldwait.o  sigchldwaitpid.o
PROGS = tcpserv01 tcpcli01 tcpserv02 tcpcli02 tcpserv04 tcpcli04 
CLEANFILES = *.o 



all:	${OBJS} ${PROGS}

str_cli.o:str_cli.c
	${CC} ${CFLAGS} -c $< -o $@ ${LIBS}

str_echo.o:str_echo.c
	${CC} ${CFLAGS} -c $<  -o $@ ${LIBS}

signal.o:signal.c	
	${CC} ${CFLAGS} -c $< -o $@ ${LIBS}

sigchldwait.o:sigchldwait.c	
	${CC} ${CFLAGS} -c $< -o $@ ${LIBS}

sigchldwaitpid.o:sigchldwaitpid.c
	${CC} ${CFLAGS} -c $< -o $@ ${LIBS}


tcpserv01:tcpserv01.o
	${CC} ${CFLAGS} -o $@ tcpserv01.o  str_echo.o ${LIBS}

tcpcli01:tcpcli01.o
	${CC} ${CFLAGS} -o $@ tcpcli01.o ${LIBS}

tcpserv02:tcpserv02.o
	${CC} ${CFALGS} -o $@ tcpserv02.o str_echo.o signal.o sigchldwait.o ${LIBS}

tcpcli02:tcpcli02.o
	${CC} ${CFLAGS} -o $@ tcpcli02.o ${LIBS}

tcpserv04:sigchldwaitpid.o tcpserv04.o
	${CC] ${CLFAGS} -o $@ tcpserv04.o  sigchldwaitpid.o ${LIBS]

tcpcli04:tcpcli04.o
	${CC} ${CFALGS} -o $@ tcpcli04.o str_cli.o ${LIBS}

clean:
	rm -rf ${CLEANFILES}  
