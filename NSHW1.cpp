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
#include <string>
#include <fstream>
#include <map>
#include <chrono>
#include <wait.h>
#include "HttpHeaderParser.h"
#define PORT 5000
#define BUF_LEN 1024
#define RootPath "./"
using namespace std;

#define DEBUG
#ifdef DEBUG
#define DEBUG_ONLY(x) x
#else
#define DEBUG_ONLY(x)
#endif



void http_handler(int);
void html_handler(int,HttpHeaderParser&);
void cgi_handler(int,HttpHeaderParser&);
void html_404_handler(int);

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

string get_http_time(){
  char buf[30];
  time_t now = time(0);
  tm tm = *gmtime(&now);
  strftime(buf, sizeof buf, "%a, %d %b %Y %H:%M:%S %Z", &tm);
  return buf;
}

int main(int argc, char const *argv[])
 {
		const auto p1 = std::chrono::system_clock::now();

	    std::cout << "seconds since epoch: "
	              << std::chrono::duration_cast<std::chrono::seconds>(
	                   p1.time_since_epoch()).count() << '\n';

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
    		DEBUG_ONLY(cout << "<!New Socket!>" <<endl<<endl;)
		//			thread method
			std::thread Handler(http_handler,new_socket);
		    Handler.detach();
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
    	}
    }
    return 0;
}

void http_handler(int client_socket){
	char buffer[BUF_LEN] = {0};
	int read_count = 0;
	try{
		read_count = recv( client_socket , buffer, BUF_LEN,0);
			if(read_count > 0){
				DEBUG_ONLY(cout << buffer <<endl;);
				HttpHeaderParser parser(buffer,sizeof(buffer));
				if(parser.getFile().find("html")!=string::npos)html_handler(client_socket,parser);
				else if(parser.getFile().find("cgi")!=string::npos)cgi_handler(client_socket,parser);
				else throw "Test";
			}
	}
	catch(...){
		DEBUG_ONLY(cout << "Send 404." << endl;)
		html_404_handler(client_socket);
	}
	close(client_socket);
	DEBUG_ONLY(cout << "<!Socket Closed!>" <<endl<<endl;)
}

void http_sender(int dest_socket, std::map<string,string> header_options ){
	string ret =
			"HTTP/1.1 "+ (header_options.count("status")?header_options["status"]:"404 Not Found") +"\r\n"
			"date: "+get_http_time()+"\r\n"
			"expires: -1\r\n"
			"cache-control: private, max-age=0\r\n"
			"content-type: text/html; charset=UTF-8\r\n"
			"server: NSHW1_EZHTTPD\r\n"
			"content-length:"+ (header_options.count("content")==1?to_string(header_options["content"].size()):"0") +" \r\n"
			"\r\n"+
			header_options["content"]+"\r\n";

	for(int i = 0 , j = ret.size();i<j;i+=BUF_LEN){
		send(dest_socket,(void *)&ret[i],((j-i) < BUF_LEN?j-i:BUF_LEN),0 );
	}

	DEBUG_ONLY(cout << "Sent Package:\n" << ret <<endl;)
}

void html_handler(int client_socket,HttpHeaderParser& parser){
	int file_size = 0;
	DEBUG_ONLY(cout << "GetPath:" << parser.getPath() <<endl;)
	string fullPath = RootPath + parser.getPath();
	DEBUG_ONLY(cout << "FullPath:" << fullPath <<endl;)
	std::fstream infile = std::fstream(fullPath,ios::in|ios::binary);

	if(!infile){
		DEBUG_ONLY(cout << "HTML Handler: Cannot Open FIle." <<endl;)
		throw "Cannot Open File.";
	}

	infile.seekg(0,infile.end);
	file_size = infile.tellg();
	infile.seekg(0,infile.beg);
	char  * buffer = new char[file_size+1];
	buffer[file_size] = '\0';
	DEBUG_ONLY(cout << "File Size:" << file_size <<endl;)
	infile.read(buffer,file_size);
	infile.close();

	http_sender(client_socket,{
		{"status","200 OK"},
		{"content",buffer}
	});

	delete[] buffer;
}

void cgi_handler(int client_socket,HttpHeaderParser& parser){
	DEBUG_ONLY( cout << "CGI Handling." << endl;)
	int ParentOutput[2] = {0};
	int ChildOutput[2] = {0};
    int status;
    char* inputData={"Hello world"};
    pid_t cpid;
     char c;

      /* Use pipe to create a data channel betweeen two process
         'ParentOutput'  handle  data from 'host' to 'CGI'
         'ChildOutput' handle data from 'CGI' to 'host'*/
      if( pipe(ParentOutput)<0  || pipe(ChildOutput)<0){
            throw "Cannot Execute Cgi. Cannot CreatePipe.";
      }

      /* Creates a new process to execute cgi program */
      if((cpid = fork()) < 0){
            throw "Cannot Execute Cgi. Fork Failed.";
      }

      /*child process*/
      if(cpid == 0){
            //close unused fd
            close(ParentOutput[1]);
            close(ChildOutput[0]);

            //redirect the output from stdout to cgiOutput
            dup2(ChildOutput[1],STDOUT_FILENO);

            //redirect the input from stdin to cgiInput
            dup2(ParentOutput[0], STDIN_FILENO);

            //after redirect we don't need the old fd
            close(ParentOutput[0]);
            close(ChildOutput[1]);

            /* execute cgi program
               the stdout of CGI program is redirect to cgiOutput
               the stdin  of CGI program is redirect to cgiInput
            */
            string fullPath = RootPath + parser.getPath();
            status = execlp(fullPath.c_str(),fullPath.c_str(),NULL);
            exit(0);
      }

      /*parent process*/
      else{
    	  	 char buffer[BUF_LEN] = {0};

            //close unused fd
            close(ParentOutput[0]);
            close(ChildOutput[1]);

            // send the message to the CGI program
            write(ParentOutput[1], inputData, strlen(inputData));


            // receive the message from the  CGI program
            string result = "";
            while (read(ChildOutput[0], &c, 1) > 0){
            	 //buffer the message
                  result.append(1,c);
            }

            // connection finish
            close(ChildOutput[0]);
            close(ParentOutput[1]);
            waitpid(cpid, &status, 0);
            if(status == -1) throw "Execute Cgi Failed.";
            http_sender(client_socket,{
            		{"status","200 OK"},
            		{"content",result}
            });
      }
}

void cgi_execute(const char * target){

}

void html_404_handler(int client_socket){
	http_sender(client_socket,{
			{"status","404 Not Found"},
		});
}
