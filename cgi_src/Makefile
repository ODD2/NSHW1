OBJECTS := view.cgi insert.cgi

BIN_LOC := ../public/cgi_bin/

all: $(OBJECTS)

%.cgi : %.cpp ../Helper.cpp ../Global.h 
	$(CXX) -o $(BIN_LOC)$@ $^ 
