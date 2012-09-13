CXX = g++ -std=gnu++0x
CXXFLAGS = -Wall -g -D SEARCHKEYHASHANSWERCOMMAND_UNIT_TEST
INCLUDES = 
LIBS = -lboost_iostreams -lboost_serialization -lcrypto -lboost_thread -ldl -lpthread -lboost_system
OBJS = SearchKeyHashAnswerCommand.o
PROGRAM = SearchKeyHashAnswerCommand.out

all:$(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(INCLUDES) $(LIBS) -o $(PROGRAM)

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIBS) -c $<

.PHONY: clean
clean:
	rm -f *o $(PROGRAM)
