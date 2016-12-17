CC = gcc
CFLAGS = -g -O2 -Wall 
LIBS = -lunp -lpthread;
OBJS = strcliselect01.o strcliselect02.o  str_echo.o  sig_child.o
PROGS = tcpcli01 tcpserv01 tcpservselect01  tcpservpoll01 tcpservepoll01
CLEANFILES = *.o

all: ${OBJS} ${PROGS}

strcliselect01.o:strcliselect01.c
	${CC} ${CFLAGS} -c $<   ${LIBS}

strcliselect02.o:strcliselect02.c
	${CC} ${CFLAGS} -c $<   ${LIBS}

str_echo.o:str_echo.c
	${CC} ${CFLAGS} -c $< ${LIBS}


sig_child.o:sig_child.c
	${CC} ${CFLAGS} -c $< ${LIBS}

tcpcli01:tcpcli01.o
	${CC} ${CFLAGS} -o $@ tcpcli01.o strcliselect02.o ${LIBS}

tcpserv01:tcpserv01.o
	${CC} ${CFLAGS} -o $@ tcpserv01.o str_echo.o sig_child.o ${LIBS}

tcpservselect01:tcpservselect01.o
	${CC} ${CFLAGS} -o $@ tcpservselect01.o ${LIBS}

tcpservpoll01:tcpservpoll01.o
	${CC} ${CFLAGS} -o $@ tcpservpoll01.o ${LIBS}

tcpservepoll01:tcpservepoll01.o
	${CC} ${CFLAGS} -o $@ tcpservepoll01.o ${LIBS}

clean:
	rm -rf ${CLEANFILES}


