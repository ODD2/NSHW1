CXXFLAGS =	-O2 -g -Wall -fmessage-length=0

OBJS =		NSHW1.o HttpHeaderParser.o

LIBS = -lpthread

TARGET =	NSHW1

HttpHeaderParser.o : HttpHeaderParser.h HttpHeaderParser.cpp
	$(CXX) $(CXXFLAGS) -c HttpHeaderParser.cpp
	
$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
