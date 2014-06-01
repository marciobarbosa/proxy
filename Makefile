all:
	gcc -g ./src/proxy.c ./src/connection.c ./src/util.c -o proxy