CXX = ccache g++ -std=gnu++0x
CXXFLAGS = -Wall -g -D JSONPARSER_UNIT_TEST
INCLUDES =
LIBS = -ljsoncpp
OBJS = JsonParser.o
PROGRAM = JsonParser.out

all:$(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIBS) $^ -o $(PROGRAM)

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIBS) -c $<

.PHONY: clean
clean:
	rm -f *o $(PROGRAM)
