CXXFLAGS =	-O2 -g -Wall -fmessage-length=0

OBJS =		NSHW1.o HttpHeaderParser.o Helper.o

LIBS = -lpthread

TARGET =	NSHW1

TEST_CGI : test_cgi.cpp
	$(CXX) -o test_cgi.cgi test_cgi.cpp

HttpHeaderParser.o : HttpHeaderParser.h HttpHeaderParser.cpp
	$(CXX) $(CXXFLAGS) -c HttpHeaderParser.cpp
	
$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET) TEST_CGI 

clean:
	rm -f $(OBJS) $(TARGET)
