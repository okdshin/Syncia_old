CXX = ccache g++ -std=gnu++0x
CXXFLAGS = -Wall -g -D SEARCHKEYHASHABILITY_UNIT_TEST
INCLUDES = 
LIBS = -lboost_serialization -lboost_thread -ldl -lpthread -lboost_system -ljsoncpp -lcrypto
OBJS = SearchKeyHashAbility.o
PROGRAM = SearchKeyHashAbility.out

all:$(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(INCLUDES) $(LIBS) -o $(PROGRAM)

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIBS) -c $<

.PHONY: clean
clean:
	rm -f *o $(PROGRAM)
