/*
 * Helper.cpp
 *
 *  Created on: Apr 6, 2020
 *      Author: xd
 */

#include <iostream>
#include "Helper.h"

using namespace std;

map<string,string> ParseQuery( char * buffer){
	map<string,string> ret;
	char * line = strtok(buffer,"&");
	string id,value;
	size_t split;

	while(line!=NULL){
		id = line;
		split = id.find('=');
		//if input format incorrect, DROP.
		if(split != string::npos) {
			//save key/value pair
			value = id.substr(split+1);
			id = id.substr(0,split);
			ret[id] = value;
		}
		//get next line
		line = strtok(NULL,"&");
	}
	return ret;
}
