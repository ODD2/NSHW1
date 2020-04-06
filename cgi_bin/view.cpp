/*
 * test_cgi.cpp
 *
 *  Created on: Apr 6, 2020
 *      Author: xd
 */



#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <error.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fstream>
#include <sys/stat.h>
#include "../Helper.h"
using namespace std;
/*
Please make sure you understand host.c
*/

int main(void){
//    int unread;
//    char *buf;
//
//    // wait for stdin
//    while(unread<1){
//        if(ioctl(STDIN_FILENO,FIONREAD,&unread)){
//            perror("ioctl");
//            exit(EXIT_FAILURE);
//        }
//    }
//
//
//    buf = (char*)malloc(sizeof(char)*(unread+1));
//    buf[unread] = '\0';
//    // read from stdin fd
//    read(STDIN_FILENO,buf,unread);
	string content;
    if( access( "./tmp.txt", F_OK ) != -1) {
    	fstream file("./tmp.txt",ios::in);
		char * buf = nullptr;
		if(file){
			file.seekg(0,file.end);
			int size = file.tellg();
			file.seekg(0,file.beg);
			char * buf = new char[size+1];
			file.read(buf,size);
			content = string(buf);
		}
	    delete[] buf;
    }
    else{
    	content = "File Not Found";
    }



    // output to stdout
    printf("<HTML><HEAD><meta http-equiv=\"content-type\" content=\"text/html;charset=utf-8\">\n");
    printf("<TITLE>View File</TITLE>\n");
    printf("<BODY>File Content: %s</BODY></HTML>\n",&content[0]);

}
