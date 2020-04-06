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
using namespace std;
/*
Please make sure you understand host.c
*/

int main(void){
    int unread;
    char *buf;

    {
    	// wait for stdin
    	bool waitSec = false;
    	while(unread<1){
 	        if(ioctl(STDIN_FILENO,FIONREAD,&unread)){
 	            perror("ioctl");
 	            exit(EXIT_FAILURE);
 	        }
    	}
    }



    buf = (char*)malloc(sizeof(char)*(unread+1));
    buf[unread] = '\0';
//    cout << unread <<endl;
    // read from stdin fd
    read(STDIN_FILENO,buf,unread);
//    cout << "asdfasdfasdf"<<endl;
    map<string,string> inputParams  = ParseQuery(buf);
    //for(auto i = inputParams.begin();i!=inputParams.end();i++){
    //	cout << i->first << i->second <<endl;
    //}
    if(inputParams.count("value")){
    	fstream file("./tmp.txt",ios::in|ios::app);
    	file << inputParams["value"] <<endl;
    	file.close();
    	printf("Insert  \"%s\" Success\n",inputParams["value"].c_str());
    }
    else{
    	printf("No Value Inserted\n");
    }
}
