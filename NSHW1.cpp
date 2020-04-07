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
#include <sys/stat.h>
#include <sys/select.h>
#include "HttpHeaderParser.h"
#include "Global.h"
using namespace std;




void http_handler(int);
void file_handler(int,HttpHeaderParser&);
void cgi_handler(int,HttpHeaderParser&);
void html_404_handler(int,const char *);
void http_sender(int dest_socket, std::map<string,string> header_options );

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
		int server_fd, new_socket;
	    struct sockaddr_in addr;
	    int opt = 1;
	    int addr_len =sizeof(addr);
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
//// Fork Method
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
	//select() requirements.
    fd_set rset;
	//	{secs, usecs}
    timeval tv = {KEEP_ALIVE_TIMEOUT,0};

    //setup
    FD_ZERO(&rset);

	while(1){
		try{
			//watch client_socket
		    FD_SET(client_socket,&rset);

		    int readyN = select(client_socket+1,&rset,NULL,NULL,&tv);
		    //if timeout or error
		    if(readyN <=0)throw EC_CON_TIMEOUT;
		    //if client_socket has event
		    else if(FD_ISSET(client_socket,&rset)){
				read_count = recv( client_socket , buffer, BUF_LEN,MSG_DONTWAIT);
				//nothing to read
				if(read_count == -1) continue;
				//connection closed
				else if(read_count == 0 )break;

				DEBUG_ONLY(cout << buffer <<endl;);
				HttpHeaderParser parser(client_socket,buffer);
				if(parser.getFile().find("cgi")!=string::npos)cgi_handler(client_socket,parser);
				else file_handler(client_socket,parser);
		    }
		}
		catch(const char * msg){
			DEBUG_ONLY(cout << "Send 404." << endl;)
			html_404_handler(client_socket,msg);
		}
		catch(int errorCode){
			if(errorCode == EC_CON_TIMEOUT){
				DEBUG_ONLY(cout << "Connection Overtime." << endl;)
				break;
			}
			else if(errorCode == EC_CON_CLOSE){
				DEBUG_ONLY(cout << "Connection Closed By Client." << endl;)
				break;
			}
			else{
				http_sender(client_socket,{
						{"Status","500 Internal Server Error"},
						{"Content","Unknown Error"}
				});
			}
		}
		catch(...){
			DEBUG_ONLY(cout << "Send 500." << endl;)
			http_sender(client_socket,
					{
							{"Status","500 Internal Server Error"},
							{"Content","Server Error"}
					});
			break;
		}
		memset(buffer,'\0',BUF_LEN);
	}

	close(client_socket);
	DEBUG_ONLY(cout << "<!Socket Closed!>" <<endl<<endl;)
}

void http_sender(int dest_socket, std::map<string,string> header_options ){
	string ret =
			"HTTP/1.1 "+ (header_options.count("Status")?header_options["Status"]:"404 Not Found") +"\r\n"
			CONNECTION_OPTION
			"Date: "+get_http_time()+"\r\n"
			"Expires: -1\r\n"
			"Server: NSHW1_EZHTTPD\r\n"
			"Cache-Control: private, max-age=0\r\n"
			"Content-Type:" + (header_options.count("Content-Type")? header_options["Content-Type"]:" text/html; charset=UTF-8")+"\r\n"
			"Content-Length:"+ (header_options.count("Content")?to_string(header_options["Content"].size()):"0") +" \r\n"
			//Add Custom Header
			+header_options["Custom"]+
			//Add Content
			"\r\n"
			+ header_options["Content"]+"\r\n";

	//Send Packet
	for(int i = 0 , j = ret.size();i<j;i+=BUF_LEN){
		send(dest_socket,(void *)&ret[i],((j-i) < BUF_LEN?j-i:BUF_LEN),0 );
	}

	DEBUG_ONLY(cout << "Sent Package:\n" << ret <<endl;)
}

void file_handler(int client_socket,HttpHeaderParser& parser){
	int file_size = 0;
	string fullPath = ROOT_PATH + parser.getPath();
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
		{"Status","200 OK"},
		{"Content",buffer}
	});

	delete[] buffer;
}

void cgi_handler(int client_socket,HttpHeaderParser& parser){
	DEBUG_ONLY( cout << "CGI Handling." << endl;)
	string fullPath = ROOT_PATH + parser.getPath();
    if( access( fullPath.c_str(), F_OK ) == -1) throw "No Cgi File";

	int ParentOutput[2] = {0};
	int ChildOutput[2] = {0};
    int status;
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

            execlp(fullPath.c_str(),fullPath.c_str(),NULL);
            exit(0);
      }

      /*parent process*/
      else{
            //close unused fd
            close(ParentOutput[0]);
            close(ChildOutput[1]);

            // send the message to the CGI program
            string params = parser.getParams();
            int totalSize = params.size();
            for(int i = 0; i < totalSize;){
            	i += write(ParentOutput[1], &params[i], (i+BUF_LEN < totalSize?BUF_LEN:totalSize-i));
            }

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
            http_sender(client_socket,{
            		{"Status","200 OK"},
            		{"Content",result}
            });
      }
}

void html_404_handler(int client_socket,const char * msg){
	http_sender(client_socket,{
			{"Status","404 Not Found"},
			{"Content",msg}
		});
}
