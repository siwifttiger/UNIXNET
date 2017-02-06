CC = gcc
CFLAGS = -g -O2 -Wall
LIBS = -lunp -lpthread
PROGS = byteorder
CLEANFILES = byteorder.o

all:	${PROGS}
byteorder:byteorder.o
	${CC} ${CFLAGS} -o $@  byteorder.o ${LIBS} 		

clean:
	rm -rf ${CLEANFILES} ${PROGS}
