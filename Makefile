CC=gcc

all: libcgif

libcgif: hex.o cgi.o
	$(CC) -shared -W1,-soname,libcgif.so -o libcgif.so hex.o cgi.o -lc -lfcgi -L/usr/lib
	rm *.o

hex.o: hex.c
	$(CC) -c -fPIC hex.c
cgi.o: cgi.c
	$(CC) -c -fPIC  -lfcgi -L/usr/lib cgi.c

install:
	cp cgi.h /usr/include/libcgif.h
	cp libcgif.so /usr/lib 
