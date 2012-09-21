CXX = g++ -std=gnu++0x
CXXFLAGS = -Wall -g -D LINKACTION_UNIT_TEST
INCLUDES = 
LIBS = -lboost_date_time -lboost_serialization -lboost_thread -ldl -lpthread -lboost_system -ljsoncpp -lcrypto
OBJS = LinkAction.o
PROGRAM = LinkAction.out

all:$(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(INCLUDES) $(LIBS) -o $(PROGRAM)

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIBS) -c $<

.PHONY: clean
clean:
	rm -f *o $(PROGRAM)
