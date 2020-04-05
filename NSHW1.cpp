//============================================================================
// Name        : NSHW1.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C, Ansi-style
//============================================================================
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <thread>
#include <csignal>
#define PORT 5000
#define BUF_LEN 1024
using namespace std;

#define DEBUG
#ifdef DEBUG
#define DEBUG_ONLY(x) x
#else
#define DEBUG_ONLY(x)
#endif



void http_handler(int);
int header_parser(char (*)[BUF_LEN]);

int forkm(){
    /* 讓父行程不必等待子行程結束 */
    signal(SIGCHLD, SIG_IGN);
    int ret = fork();
    switch(ret){
    case -1:
    	exit(0);
    case 0:
    	return 0;
    default:
    	return ret;

    }
}

int main(int argc, char const *argv[])
{
    int server_fd, new_socket,read_count;
    struct sockaddr_in addr;
    int opt = 1;
    int addr_len =sizeof(addr);
    char buffer[BUF_LEN] = {0};
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons( PORT );

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&addr,sizeof(addr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while(1)
    {
        new_socket = accept(server_fd, (struct sockaddr *)&addr,(socklen_t *)&addr_len );
    	if(new_socket< 0){
    		perror("accept");
    	}
    	else{
    		DEBUG_ONLY(cout << "New Socket..." <<endl;)
////    		fork method
//			if(forkm()){
////				if parent, keep on listening ignore client socket.
//				close(new_socket);
//			}
//			else{
////				if child, handle client socket stop listening.
//				close(server_fd);
//				http_handler(new_socket);
//				break;
//			}

////			thread method
			std::thread Handler(http_handler,new_socket);
    		Handler.detach();
    	}
    }
    return 0;
}

void http_handler(int client_socket){
	char buffer[BUF_LEN] = {0};
	int read_count = 0;
	char respond[] =
			    "HTTP/1.1 200 OK\n"
			    "Date: Mon, 27 Jul 2009 12:28:53 GMT\n"
			    "Server: Apache/2.2.14 (Win32)\n"
			    "Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT\n"
			    "Content-Length: 88\n"
			    "Content-Type: text/html\n"
				"Conenction: Closed\n"
//			    "Connection: Keep-Alive\n"
//				"Keep-Alive: timeout=5, max=1000\n"
			    "\n"
			    "<html><body>\n"
			    "<h1>Hello, World!</h1>\n"
			    "</body></html>\n";

	read_count = recv( client_socket , buffer, BUF_LEN,0);

	if(read_count > 0){
				int send_size = send(client_socket, respond , strlen(respond),0);
				DEBUG_ONLY(cout << "Msg Sent:" << send_size <<endl;)
	}
	close(client_socket);
	DEBUG_ONLY(cout << "Socket Closed" <<endl;)
}

int header_parser(char (&buffer)[BUF_LEN]){
	return 0;
}
