CC = gcc
CFLAGS = -g -O2 -Wall
LIBS = -lunp -lpthread
PROGS = daytimetcpcli daytimetcpcliv6 daytimetcpsrv
CLEANFILES = *.o

all:	${PROGS}

daytimetcpcli: daytimetcpcli.o
		${CC} ${CFLAGS} -o $@ daytimetcpcli.o ${LIBS}
daytimetcpcliv6: daytimetcpcliv6.o
		${CC} ${CFLAGS} -o $@ daytimetcpcliv6.o ${LIBS}
daytimetcpsrv: daytimetcpsrv.o
		${CC} ${CFLAGS} -o $@ daytimetcpsrv.o ${LIBS}


clean:
		rm -rf ${PROGS} ${CLEANFILES}
