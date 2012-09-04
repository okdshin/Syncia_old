#ifdef SERVANT_UNIT_TEST
#include "Servant.h"
#include <iostream>
#include "JsonParser.h"

using namespace sy;

int main(int argc, char* argv[])
{
/*	
	boost::asio::io_service service;
	int local_port = 54321;
	if(argc == 2)
	{
		local_port = boost::lexical_cast<int>(argv[1]);
	}

	const int buffer_size = 128;
	
	auto servant_ptr = Servant<JsonParser>::Create(service, "127.0.0.1", local_port, 
		buffer_size, JsonParser("command"), std::cout);
	TestCuiApp(service, servant_ptr);	
 */
	//std::cout << "\033[1;35mbold red text\033[0m\n" << std::endl;
	ManyServant(100);
  	return 0;
}

#endif
