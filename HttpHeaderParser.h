#ifndef HttpHeaderParserOD
#define HttpHeaderParserOD
#include <vector>
#include <string>
#include <exception>
#include <string.h>
#include <iostream>
#include <map>
#include <sys/socket.h>
using namespace std;
enum eMethod{
		Unknown = -1,
		Get,
		Post,
		Total,
	};

class HttpHeaderParser{
public:
	HttpHeaderParser(int client_socket,char * headerBuffer);

	string getPath();

	string getFile();

	string getDirectory();

	string getContent();

	string getQuery();

	string getParams();

	map<string,string>& getOptions();

	eMethod getMethod();

private:
	void parseFirstLine(string firstLine);
	void parseOptions(string optLines);

	eMethod reqMethod = eMethod::Unknown;
	map<string,string> optPair;
	string mUrl = "";
	string mQuery = "";
	string mContent = "";
};

#endif
