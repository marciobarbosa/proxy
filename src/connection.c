/* Developed by Marcio Brito Barbosa */

#include "../hdr/connection.h"

int success = 0, filtered = 0, error = 0;
int process_id = 0;

int setupServer(int port_number) {
	int server_sock = -1;
	struct sockaddr_in server;

	memset(&server, 0, sizeof(struct sockaddr_in));

	server_sock = socket(AF_INET, SOCK_STREAM, 0); 

	if(server_sock == -1) {
		printf("ERROR: Unable to create a socket!\n");
		exit(EXIT_FAILURE);
	}

	server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port_number);

    if(bind(server_sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("The following error occurred (bind)");
        exit(EXIT_FAILURE);
    }

    listen(server_sock, 5);

    return server_sock;
}

char* getFunction(char *request) {
	char *function = NULL;
	char *begin = request, *end = NULL;
	int offset = 0;

	end = strpbrk(request, " ");
	offset = (int)end - (int)begin;
	function = (char*)calloc(offset, sizeof(char));
	strncpy(function, begin, offset);
	function[offset] = '\0';

	return function;
}

char* getURL(char *request) {
	char *url = NULL;
	char *begin = NULL, *end = NULL;
	int offset = 0;

	begin = strpbrk(request, "://");
	begin += 3;
	end = strpbrk(begin, "/");

	offset = (int)end - (int)begin;
	url = (char*)calloc(offset, sizeof(char));
	strncpy(url, begin, offset);
	url[offset] = '\0';

	return url;
}

char* getDirectory(char *request) {
	char *directory = NULL;
	char *begin = NULL, *end = NULL;
	int offset = 0;

	begin = strpbrk(request, "://");
	begin += 3;
	begin = strpbrk(begin, "/");
	end = strpbrk(begin, " ");

	offset = (int)end - (int)begin;
	directory = (char*)calloc(offset, sizeof(char));
	strncpy(directory, begin, offset);
	directory[offset] = '\0';

	return directory;
}

char* getHttpVersion(char *request) {
	char *version = NULL;
	char *begin = NULL, *end = request;
	int offset = 0, i;

	begin = strpbrk(request, "://");
	begin += 3;
	begin = strpbrk(begin, "/");
	begin = strpbrk(begin, " ");
	begin = strpbrk(begin, "H");
	
	end = strpbrk(begin, "\n");

	offset = (int)end - (int)begin;
	version = (char*)calloc(offset, sizeof(char));
	strncpy(version, begin, offset);
	version[offset] = '\0';

	return version;
}

char* getToPrint(char *request) {
	char *str = NULL;
	char *begin = request, *end = NULL;
	int offset = 0;

	end = strpbrk(request, " ");
	end++;
	end = strpbrk(end, " ");

	offset = (int)end - (int)begin;
	str = (char*)calloc(offset, sizeof(char));
	strncpy(str, begin, offset);
	str[offset] = '\0';

	return str;
}

char* getLength(char *request) {
	char *begin = request, *end = NULL;
	char *len = NULL;
	int offset = 0;

	begin = strstr(request, "Length:");
	begin += 8;
	end = strpbrk(begin, "\n");

	offset = (int)end - (int)begin;
	len = (char*)calloc(offset, sizeof(char));
	strncpy(len, begin, offset);
	len[offset] = '\0';

	return len;
}

char* getData(char *request, int size) {
	char *begin = request;
	char *data = (char*)calloc(size, sizeof(char));
	int i;

	begin += strlen(request) - size;
		
	strncpy(data, begin, size);
	data[size] = '\0';

	return data;
}

void makeRequest(char *url, int client_sock) {
	int clntSock = -1;
    struct addrinfo addrCriteria;
    struct addrinfo *addrList, *addr;
    int returned_value;
    char recvline[MAX_LINE + 1];
    size_t n;
    char send_message[MAX_LINE];    
    char *bad_request = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\nContent-Length: 100\r\n\r\n<html>\n<head><title>400 Bad Request</title>\n</head>\n<body>\n<h1>400 Bad Request</h1>\n</body>\n</html>\r\n\r\n";

    if(strcmp("GET", getFunction(url)) == 0)
    	sprintf(send_message, "GET %s %s\r\nHost: %s\r\n\r\n", getDirectory(url), getHttpVersion(url), getURL(url));
    else if(strcmp("HEAD", getFunction(url)) == 0)
    	sprintf(send_message, "HEAD %s %s\r\nHost: %s\r\n\r\n", getDirectory(url), getHttpVersion(url), getURL(url));
    else if(strcmp("POST", getFunction(url)) == 0) 
    	sprintf(send_message, "POST %s %s\r\nHost: %s\r\nContent-Length: %s\r\n\r\n%s", getDirectory(url), getHttpVersion(url), getURL(url), getLength(url), getData(url, atoi(getLength(url))));
        
    memset(&addrCriteria, 0, sizeof(addrCriteria));
    addrCriteria.ai_family = AF_UNSPEC;
    addrCriteria.ai_socktype = SOCK_STREAM;
    addrCriteria.ai_protocol = IPPROTO_TCP;
    
    returned_value = getaddrinfo(getURL(url), "80", &addrCriteria, &addrList);

    if(returned_value != 0) {
    	write(client_sock, bad_request, strlen(bad_request));
    	return;
    }

	for(addr = addrList; addr != NULL; addr = addr->ai_next) {
        clntSock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);

        if(clntSock < 0)
            continue;

        if(connect(clntSock, addr->ai_addr, addr->ai_addrlen) == 0)
            break;

        close(clntSock);
        clntSock = -1;
    }

    freeaddrinfo(addrList);

    if (write(clntSock, send_message, strlen(send_message)) >= 0) {
    	while ((n = read(clntSock, recvline, MAX_LINE)) > 0) {
    		recvline[n] = '\0';
        	write(client_sock, recvline, strlen(recvline));
    	} 
    }

    close(clntSock);
}

void applicationProtocol(int client_sock, filterList *begin, char *host_name, int server_sock) {
	int rcv_msg_size = 0;
	char buffer[MAX_SIZE];
	char *bad_request = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\nContent-Length: 100\r\n\r\n<html>\n<head><title>400 Bad Request</title>\n</head>\n<body>\n<h1>400 Bad Request</h1>\n</body>\n</html>\r\n\r\n";
	char *method_not_allowed = "HTTP/1.1 405 Method not allowed\r\nContent-Type: text/html\r\nContent-Length: 100\r\n\r\n<html>\n<head><title>405 Method not allowed</title>\n</head>\n<body>\n<h1>405 Method not allowed</h1>\n</body>\n</html>\r\n\r\n";

	while(TRUE) {
		memset(buffer, 0, MAX_SIZE);

		rcv_msg_size = read(client_sock, buffer, MAX_SIZE);

		if(rcv_msg_size < 0) {
			perror("The following error occurred (read)");
			exit(EXIT_FAILURE);
		}

		if(buffer[rcv_msg_size - 2] == '\r' && buffer[rcv_msg_size - 1] == '\n')
			buffer[rcv_msg_size - 2] = '\0';
		else
			buffer[rcv_msg_size] = '\0';

		if(strcmp("SIGUSR1", buffer) == 0) {
			printf("Received SIGUSR1...reporting status:\n");
			printf("-- Processed %d requests successfully\n", success);
			printf("-- Filtering: ");
			printList(begin);
			printf("-- Filtered %d requests\n", filtered);
			printf("-- Encountered %d requests in error\n", error);
		}

		else if(strcmp("SIGUSR2", buffer) == 0) {
			close(server_sock);
			kill(process_id, SIGKILL);
		}

		else if(filterAddr(begin, getURL(buffer))) {
			filtered++;
			printf("%s", host_name);
    		printf(": ");
    		printf("%s", getToPrint(buffer));
    		printf(" [FILTERED]\n");
    		write(client_sock, bad_request, strlen(bad_request));    		
    	}

    	else if(strcmp(getFunction(buffer), "GET") != 0 && strcmp(getFunction(buffer), "HEAD") != 0 && strcmp(getFunction(buffer), "POST") != 0) {
    		error++;
    		printf("%s", host_name);
    		printf(": ");
    		printf("%s", getToPrint(buffer));
    		printf(" [INVALID REQUEST]\n");
    		write(client_sock, method_not_allowed, strlen(method_not_allowed));    		
    	}

    	else {
    		success++;
    		printf("%s", host_name);
    		printf(": ");
    		printf("%s", getToPrint(buffer));
    		printf("\n");
    		makeRequest(buffer, client_sock);    		
    	}		
	}
}

void startServer(int server_sock, filterList *begin) {
	struct sockaddr_in client;
	int client_sock = -1;
	int pid = -1;
	int client_len = -1;
	struct in_addr ipv4addr;
	struct hostent *hostName;
	char *host_name = NULL;

	memset(&client, 0, sizeof(struct sockaddr_in));

	while(TRUE) {
		client_len = sizeof(client);
		client_sock = accept(server_sock, (struct sockaddr*)&client, (socklen_t*)&client_len);

		if(client_sock < 0) {
            perror("The following error occurred (accept)");
            exit(EXIT_FAILURE);
        }
        
        pid = fork();

        if(pid < 0) {
        	perror("The following error occurred (fork)");
        	exit(EXIT_FAILURE);
        }

        if(pid == 0) {
        	inet_pton(AF_INET, inet_ntoa(client.sin_addr), &ipv4addr);
			hostName = gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);
			host_name = (char*)calloc(strlen(hostName->h_name), sizeof(char));
			strcpy(host_name, hostName->h_name);
        	close(server_sock);
        	applicationProtocol(client_sock, begin, host_name, server_sock);
        	close(client_sock);
        	free(host_name);
        	exit(EXIT_SUCCESS);
        } else {
        	process_id = getpid();
        	close(client_sock);
        }        
	}
}