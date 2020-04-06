/*
 * HttpHeaderParser.cpp
 *
 *  Created on: Apr 5, 2020
 *      Author: xd
 */


#include "HttpHeaderParser.h"
#include "Global.h"

HttpHeaderParser::HttpHeaderParser(int client_socket,char* headerBuffer){
	string headerRaw(headerBuffer);
	string firstLine;
	string optLines;
	string contents;
//	seperate the first line
	{
		size_t pos;
		if((pos = headerRaw.find("\r\n"))==string::npos) throw "Header Format Error";

		firstLine = headerRaw.substr(0,pos);
		headerRaw = headerRaw.substr(pos+2);
	}
// seperate the options and contents
	{
		size_t pos,prev_pos=0;
		while(1){
			pos = headerRaw.find("\r\n",prev_pos);
			if(pos ==string::npos)
				throw "Header Format Error";
			else if(prev_pos == pos){
				optLines = headerRaw.substr(0,prev_pos);
				contents = headerRaw.substr(pos+2);
				break;
			}
			else
				prev_pos = pos+2;
		}
	}

	parseFirstLine(firstLine);
	parseOptions(optLines);

	// fetch the contents
		{
			if(optPair.count("Content-Length")){
				char buffer[BUF_LEN] = {0};
				int read_count = 0;
				size_t length =  strtoll(optPair["Content-Length"].c_str(),NULL,10);
			    fd_set rset;
			    timeval tv = {KEEP_ALIVE_TIMEOUT,0};


				while( contents.size() < length) {
				    //setup
				    FD_ZERO(&rset);
				    //listening event
				    int readyN = select(client_socket+1,&rset,NULL,NULL,&tv);

				    if(readyN == 0)throw EC_CON_TIMEOUT;

					read_count = recv( client_socket , buffer, BUF_LEN, MSG_DONTWAIT);

					if(read_count == -1) continue;
					else if(read_count == 0) throw EC_CON_CLOSE;

					contents += buffer;
				}
				DEBUG_ONLY(cout << "Content Fetched. Content Tmp == Content-Length ? " << (contents.size()==length) <<endl;);
			}
		}
		mContent = move(contents);
};

map<string,string>& HttpHeaderParser::getOptions(){
	return optPair;
}

string HttpHeaderParser::getPath(){
	DEBUG_ONLY(cout << __FUNCTION__ << ":" <<mUrl <<endl;)
	return mUrl;
}

string HttpHeaderParser::getFile(){
	DEBUG_ONLY(cout << __FUNCTION__ << ":" <<mUrl.substr(mUrl.rfind("/")+1) <<endl;)
	return mUrl.substr(mUrl.rfind("/")+1);
}

string HttpHeaderParser::getDirectory(){
	DEBUG_ONLY(cout << __FUNCTION__ << ":" <<mUrl.substr(0,mUrl.rfind("/")) <<endl;)
	return mUrl.substr(0,mUrl.rfind("/"));
}

eMethod HttpHeaderParser::getMethod(){
	DEBUG_ONLY(cout << __FUNCTION__ << ":" << reqMethod <<endl;)
	return reqMethod;
}

string HttpHeaderParser::getContent(){
	DEBUG_ONLY(cout << __FUNCTION__ << ":" << mContent <<endl;)
	return mContent;
}

string HttpHeaderParser::getQuery(){
	DEBUG_ONLY(cout << __FUNCTION__ << ":" << mQuery <<endl;)
	return mContent;
}

string HttpHeaderParser::getParams(){
	string ret = "";
	switch(reqMethod){
	case eMethod::Get:
		ret = mQuery;
		break;
	case eMethod::Post:
		ret=mContent;
		break;
	default:
		break;
	}
	DEBUG_ONLY(cout << __FUNCTION__ << ":" << ret <<endl;)
	return ret;
}

void HttpHeaderParser::parseFirstLine(string firstLine){
	//	Get Method
	{
		size_t pos = firstLine.find(" ");
		if(pos == string::npos)
			throw "Header Format Error";
		string method = firstLine.substr(0,pos);
		if(method == "GET"){
			reqMethod = eMethod::Get;
		}
		else if(method == "POST"){
			reqMethod = eMethod::Post;
		}
		else
			throw "Unknown Method";

		firstLine = firstLine.substr(pos+1);
	}
	// Get URL
	{
		size_t pos = firstLine.find(" ");
		if(pos == string::npos) throw "Header Format Error";
		string url = firstLine.substr(0,pos);

		//	Get query string if method is GET
		if(reqMethod == eMethod::Get ){
			pos = url.find("?");
			if(pos != string::npos){
				mQuery= url.substr(pos+1);
				url = url.substr(0,pos);
			}
		}

		//	Check if Url  has relative path
		if(url.find("../")!=string::npos){
			throw "Invalid Url with relative path";
		}

		//Check if Url request default index.html
		if(url.back() == '/'){
			url.append("index.html");
		}

		mUrl = url;
	}
}

void HttpHeaderParser::parseOptions(string optLines){
		size_t pos = 0,prev_pos=0,comma=0;
		string option;
		while((pos = optLines.find("\r\n",prev_pos))!= string::npos){
			option = optLines.substr(prev_pos,pos-prev_pos);
			//Unknown Option Format Ignoring.
			if((comma = option.find(":"))== string::npos) continue;
			//Seperate key and value
			optPair[option.substr(0,comma)] = option.substr(comma+1);
			prev_pos = pos + 2;
		}
}

