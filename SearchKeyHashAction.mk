CXX = g++ -std=gnu++0x
CXXFLAGS = -Wall -g -D SEARCHKEYHASHACTION_UNIT_TEST
INCLUDES = 
LIBS = -lboost_date_time -lboost_iostreams -lboost_serialization -lcrypto -lboost_thread -ldl -lpthread -lboost_system
OBJS = SearchKeyHashAction.o
PROGRAM = SearchKeyHashAction.out

all:$(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(INCLUDES) $(LIBS) -o $(PROGRAM)

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIBS) -c $<

.PHONY: clean
clean:
	rm -f *o $(PROGRAM)
