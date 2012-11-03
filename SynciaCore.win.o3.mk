CXX = g++ -std=gnu++0x -O3
CXXFLAGS = -D SYNCIACORE_UNIT_TEST
INCLUDES = -I include 
LIBS = -lwsock32 -lws2_32 -lboost_filesystem -lboost_iostreams -lcrypto -lboost_serialization -lboost_thread -lpthread -lboost_chrono -lboost_date_time -lboost_system
OBJS = SynciaCore.o
PROGRAM = SynciaCore.out

all:$(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(INCLUDES) $(LIBS) -o $(PROGRAM)

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIBS) -c $<

.PHONY: clean
clean:
	rm -f *o $(PROGRAM)
