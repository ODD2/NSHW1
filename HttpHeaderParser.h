#ifndef HttpHeaderParserOD
#define HttpHeaderParserOD
#include <vector>
#include <string>
#include <exception>
#include <string.h>
#include <iostream>
using namespace std;
enum eMethod{
		Unknown = -1,
		Get,
		Post,
		Total,
	};

class HttpHeaderParser{
public:
	HttpHeaderParser(char * headerBuffer, size_t size);

	string getPath();

	string getFile();

	string getDirectory();

	eMethod getMethod();
private:
	eMethod reqMethod = eMethod::Unknown;
	vector<string> pathHierarchy;
	void parseLocation(char * location);
};

#endif
