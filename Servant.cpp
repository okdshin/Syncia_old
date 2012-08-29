#ifdef SERVANT_UNIT_TEST
#include "Servant.h"
#include <iostream>
#include <boost/algorithm/string.hpp>
#include "JsonParser.h"

using namespace sy;

int main(int argc, char* argv[])
{
	
	boost::asio::io_service service;
	int server_port = 54321;
	if(argc == 2)
	{
		server_port = boost::lexical_cast<int>(argv[1]);
	}

	const int buffer_size = 128;
	
	auto servant_ptr = Servant<JsonParser>::Create(service, "127.0.0.1", server_port, buffer_size, 
		JsonParser("command"), std::cout);

	auto core_ptr = servant_ptr->GetCorePtr();
	boost::thread t(boost::bind(&boost::asio::io_service::run, &service));
	std::cout << "\"help\" shows commands" << std::endl;
	while(true){ //main loop
		try{
			const auto command = utl::GetInput<std::string>("command?:");
			
			if(command == "search"){
				const auto keywards = utl::GetInput<std::string>("keywards?:");
				std::vector<std::string> keyward_list{};
				boost::split(keyward_list, keywards, boost::is_space());
				servant_ptr->SearchKeyHash(keyward_list);
			}
			else if(command == "connect"){
				const auto hostname = utl::GetInput<std::string>("hostname?:");
				const auto port = utl::GetInput<int>("port?:");	

				core_ptr->Connect(hostname, port);
			}	
		}
		catch(std::exception& e){
			std::cout << "error!!!:" << e.what() << std::endl;	
		}
	}
	t.join();	
	
    return 0;
}

#endif
