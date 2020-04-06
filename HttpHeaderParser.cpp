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
		char * querystr ;
		char_ptr_location = strtok(char_ptr_location,"?");
	    querystr = strtok(NULL,"?");
		char * query = strtok(querystr,"&");
		while(query!=NULL){
	    	contentList.push_back(query);
	    	query = strtok(NULL,"&");
		}
	}
	else if(strcmp("POST",char_ptr_method)==0){
		reqMethod = eMethod::Post;
		int contentLength = 0;

		char * headOpt = strtok(NULL,"\r\n");
		while(headOpt!=NULL){
			string headStr(headOpt);
			if(headStr.substr(0,headStr.find(':')) == "Content-Length"){
			  contentLength = strtoll(headStr.substr(15).c_str(),NULL,10);
			  break;
			}
			headOpt = strtok(NULL,"\r\n");
		}

		if(contentLength > 0 && contentLength < size){
			char * contentStart = &headerBuffer[size - contentLength];
			char * query = strtok(contentStart,"&");
			while(query!=NULL){
			   	contentList.push_back(query);
			   	query = strtok(NULL,"&");
			}
		}
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

string HttpHeaderParser::getContent(){
	if(contentList.size()==0)return " ";
	else{
		string ret = contentList[0];
		for(int i = 1 , j = contentList.size(); i < j ; ++i){
			ret += "&" + contentList[i];
		}
		return ret;
	}
}

void HttpHeaderParser::parseLocation(char * location){
		if(location[strlen(location)-1]=='/') isDir = true;

		char * pch = strtok(location,"/");
		while( pch!=NULL )
		{
			if(strlen(pch) > 0){
				if(pch[0]=='.'&&pch[1]=='.')throw "Relative Path Invalid";
				pathHierarchy.push_back(string(pch));
			}
			pch = strtok(NULL,"/");
		}
		if(pathHierarchy.size() == 0 || isDir){
			pathHierarchy.push_back("index.html");
		}
	}

