server:main.o server.o get_msg.o admin.o store_msg.o wrap.o
	gcc -o server main.o server.o get_msg.o admin.o store_msg.o wrap.o -lpthread
main.o:main.c server.h
	gcc -c main.c
admin.o:admin.c server.h wrap.h
	gcc -c admin.c
wrap.o:wrap.c wrap.h
	gcc -c wrap.c
server.o:server.c server.h
	gcc -c server.c
get_msg.o:get_msg.c server.h 
	gcc -c get_msg.c
store_msg.o:store_msg.c server.h
	gcc -c store_msg.c
clean:
	rm -rf *.o
