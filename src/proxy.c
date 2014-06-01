/* Developed by Marcio Brito Barbosa */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../hdr/util.h"
#include "../hdr/connection.h"

int main(int argc, char *argv[]) 
{
	int port_number = -1, count = 0;
	filterList *begin = NULL;
	int sock = -1;

	if(argc < 2) {
		printf("ERROR: You have to specify the port number!\n");
		exit(EXIT_FAILURE);
	} else {
		port_number = atoi(argv[1]);

		for(count = 0; count < argc - 2; count++)
			begin = insertFilterList(argv[count + 2], begin);		
	}

	sock = setupServer(port_number);
	startServer(sock, begin);

	clearFilterList(begin);

	return EXIT_SUCCESS;
}