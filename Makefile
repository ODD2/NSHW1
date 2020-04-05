CXXFLAGS =	-O2 -g -Wall -fmessage-length=0

OBJS =		NSHW1.o

LIBS = -lpthread

TARGET =	NSHW1

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
