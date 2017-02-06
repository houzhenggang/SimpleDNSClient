all: main

main: main.o dns_encode.o utils.o
	gcc main.o dns_encode.o utils.o -o main

dns_encode.o: dns_encode.c
	gcc -c dns_encode.c

utils.o: utils.c
	gcc -c utils.c

