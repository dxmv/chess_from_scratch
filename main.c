#include <stdio.h>      // standard i/o functions like printf() and perror()
#include <stdlib.h>     // standard library functions like exit()
#include <string.h>     // string manipulation functions like strlen() and memset()
#include <unistd.h>     // POSIX functions like read(), write(), and close()
#include <netinet/in.h> // definitions for internet domain addresses (struct sockaddr_in)
#include <sys/socket.h> // socket functions like socket(), bind(), listen(), and accept()

/**
 * Opens a file and serves it to the server
**/
void serve_file(int client_fd, const char *filepath) {
    	FILE *fp = fopen(filepath, "r"); // read the file
    	if (!fp) {
        	// file not found, send 404 response
        	char *notFound = "http/1.1 404 not found\r\ncontent-length: 9\r\n\r\nnot found";
        	write(client_fd, notFound, strlen(notFound));
        	return;
    	}

    	// get file size
    	fseek(fp, 0, SEEK_END);
    	long filesize = ftell(fp);
    	rewind(fp);

    	// allocate buffer and read file content
    	char *buffer = malloc(filesize);
    	if (!buffer) {
        	fclose(fp);
        	return;
    	}
    	fread(buffer, 1, filesize, fp);
    	fclose(fp);

	// create header with content length and content type
	char header[256];
	sprintf(header, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %ld\r\n\r\n", filesize);

    	// send header then file content
    	write(client_fd, header, strlen(header));
    	write(client_fd, buffer, filesize);

    	free(buffer);
}

int main(void) {
    	// file descriptor for the server's listening socket and for client connections
    	int server_fd, client_fd;
    
	// structure to hold the server's address information (ipv4)
    	struct sockaddr_in addr;
    
    	// integer option used to allow immediate reuse of the port (prevents "address already in use" errors)
	int opt = 1;
    
	// holds the size of the addr structure; needed for functions like accept()
	int addrlen = sizeof(addr);
    
	// buffer to store incoming data from the client; initialized to zero
	char buffer[1024] = {0};
    
	// define responses for different routes
    	char *root_path = "./public/home/index.html";
    	char *response_about = "http/1.1 200 ok\r\ncontent-length: 11\r\n\r\nhello about";
    	char *not_found_path = "./public/not_found/not_found.html";
   
    	//  AF_INET indicates ipv4
    	//  SOCK_STREAM indicates a TCP socket (stream-oriented)
    	//  0 lets the system choose the appropriate protocol
    	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        	perror("socket failed"); 
        	exit(EXIT_FAILURE);     
    	}

	// SO_REUSEADDR allows the socket to be quickly reused after the program terminates
    	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        	perror("setsockopt");
        	exit(EXIT_FAILURE); 
    	}

	addr.sin_family = AF_INET;            // set the address family to ipv4
        addr.sin_addr.s_addr = INADDR_ANY;      // accept connections from any network interface
	addr.sin_port = htons(8080);           // set the port number to 8080; htons converts to network byte order

    	//  casts addr to a generic sockaddr pointer
    	//  if binding fails (e.g., port already in use), print error and exit
    	if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        	perror("bind failed");
        	exit(EXIT_FAILURE);
    	}
	// set the socket to listen mode:
    	// backlog value of 3 means up to 3 pending connections can queue up
    	if (listen(server_fd, 3) < 0) {
        	perror("listen");
        	exit(EXIT_FAILURE);
    	}

	// main loop to accept and handle incoming client connections
    	while (1) {
		printf("Listening on port: 8080...\n");
        	// accept an incoming connection:
        	//  returns a new file descriptor (client_fd) for the accepted connection
        	//  addr and addrlen can be used to retrieve client address info (here, we reuse our addr)
        	if ((client_fd = accept(server_fd, (struct sockaddr *)&addr, (socklen_t*)&addrlen)) < 0) {
            		perror("accept");
            		exit(EXIT_FAILURE);
        	}
        
        	// clear the buffer to ensure it's empty before reading new data
        	memset(buffer, 0, sizeof(buffer));
        
        	// read data from the client into the buffer 
        	read(client_fd, buffer, 1024);

        	// print the received request to the console for debugging purposes
        	printf("received request:\n%s\n", buffer);

		// assume that the request is well formed
		// on the first line we have the method, route and protocol we're using
        	char method[16], route[256];
        	sscanf(buffer, "%15s %255s", method, route); // read method and route safely

        	// simple route handling:
        	// check the requested route and respond accordingly
        	if (strcmp(route, "/") == 0) {
            		serve_file(client_fd,root_path);
        	} else if (strcmp(route, "/about") == 0) {
            		write(client_fd, response_about, strlen(response_about));
        	} else {
            		// if no matching route, send a 404 response
            		serve_file(client_fd,not_found_path);
        	}
        	close(client_fd);
    }
    
    // this return is never reached because of the infinite loop,
    return 0;
}
