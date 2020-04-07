CXXFLAGS = -O2 -g -Wall -fmessage-length=0

OBJS =	NSHW1.o HttpHeaderParser.o Helper.o

LIBS = -lpthread

TARGET = NSHW1

all: $(TARGET)

%.o : %.c
	$(CXX) $(CXXFLAGS) -c $@ $^

#HttpHeaderParser.o : HttpHeaderParser.h HttpHeaderParser.cpp
#	$(CXX) $(CXXFLAGS) -c HttpHeaderParser.cpp
	
$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)


clean:
	rm -f $(OBJS) $(TARGET)
