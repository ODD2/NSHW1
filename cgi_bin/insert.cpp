/*
 * test_cgi.cpp
 *
 *  Created on: Apr 6, 2020
 *      Author: xd
 */




#include <stdio.h>
#include <unistd.h>
#include <error.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fstream>
#include <iostream>
#include "../Helper.h"
#include <unistd.h>
#include <sys/select.h>
using namespace std;
/*
Please make sure you understand host.c
*/

int main(void){
    int unread = 0;
    char *buf;
    //select() requirements.
    fd_set rset;
    //	{secs, usecs}
    timeval tv = {10,0};

    //setup
    FD_ZERO(&rset);
    FD_SET(STDIN_FILENO,&rset);

    //watch stdin
    int readyN = select(STDIN_FILENO+1,&rset,NULL,NULL,&tv);

    //if stdin has action
    if(readyN >0 && FD_SET(STDIN_FILENO,&rset) == 1){
    	//peek for input size;
     	 if(ioctl(STDIN_FILENO,FIONREAD,&unread)){
     	 	            perror("ioctl");
     	 	            exit(EXIT_FAILURE);
     	 }
    	 if(unread > 0){
        	//allocate buffer
        	 buf = (char*)malloc(sizeof(char)*(unread+1));
        	 buf[unread] = '\0';

        	 //read
        	 read(STDIN_FILENO,buf,unread);

        	 //parse input query.
        	 map<string,string> inputParams  = ParseQuery(buf);

        	 //check query
        	 if(inputParams.count("value")){
  	      	    	fstream file("./tmp.txt",ios::in|ios::app);
  	      	    	file << inputParams["value"] <<endl;
  	      	    	file.close();
  	      	    	printf("Insert  \"%s\" Success\n",inputParams["value"].c_str());
  	      	    	return 0;
  	      	  }
  	      	  else{
  	      		  	  printf("No Value Inserted\n");
  	      		  	  return 0;
  	      	  }
    	 }
    }
    printf("No Parameter Given\n");
    return 0;
}
