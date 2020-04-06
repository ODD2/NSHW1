OBJECTS := view.cgi insert.cgi

all: $(OBJECTS)

%.cgi : %.cpp ../Helper.cpp 
	$(CXX) -o $@ $^ 
