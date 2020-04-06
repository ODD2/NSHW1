/*
 * HttpHeaderParser.cpp
 *
 *  Created on: Apr 5, 2020
 *      Author: xd
 */


#include "HttpHeaderParser.h"

HttpHeaderParser::HttpHeaderParser(char* headerBuffer,size_t size){
	char * char_ptr_method,* char_ptr_location;

	char_ptr_method = strtok(headerBuffer," ");
	char_ptr_location = strtok (NULL," ");
	if(char_ptr_method==NULL || char_ptr_location ==NULL){
		//parse abort
		return;
	}

	if(strcmp("GET",char_ptr_method)==0){
		reqMethod = eMethod::Get;
	}
	else if(strcmp("POST",char_ptr_method)==0){
		reqMethod = eMethod::Post;
	}
	else{
		throw "Invalid Method";
	}

	parseLocation(char_ptr_location);
};

string HttpHeaderParser::getPath(){
	string ret;
	for(int i = 0 ,j = pathHierarchy.size();i<j;++i){
			ret += "/" + pathHierarchy[i];
	}
	return ret;
}


string HttpHeaderParser::getFile(){
	return pathHierarchy.back();
}

string HttpHeaderParser::getDirectory(){
	string ret;
	for(int i = 0 ,j = pathHierarchy.size()-1;i<j;++i){
		ret += "/" + pathHierarchy[i];
	}
	return ret;
}

eMethod HttpHeaderParser::getMethod(){
	return reqMethod;
}

void HttpHeaderParser::parseLocation(char * location){
		char * pch = strtok(location,"/");
		while( pch!=NULL )
		{
			if(strlen(pch) > 0){
				if(pch[0]=='.'&&pch[1]=='.')throw "Relative Path Invalid";
				pathHierarchy.push_back(string(pch));
			}
			pch = strtok(NULL,"/");
		}
		if(pathHierarchy.size() == 0){
			pathHierarchy.push_back("index.html");
		}
	}

