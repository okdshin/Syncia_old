#ifdef SEARCHKEYHASHABILITY_UNIT_TEST
#include "SearchKeyHashAbility.h"
#include <iostream>
#include "JsonParser.h"

using namespace sy;

void TestCuiApp(int argc, char* argv[]){
	boost::asio::io_service service;
	int local_port = 54321;
	if(argc == 2)
	{
		local_port = boost::lexical_cast<int>(argv[1]);
	}

	const int buffer_size = 128;
	
	auto client = nr::ntw::SocketClient::Create(service, buffer_size, std::cout);
	auto server = nr::ntw::SocketServer::Create(
		service, local_port, buffer_size, std::cout);

	auto peer = SearchKeyHashAbility<JsonParser>::Create(service, 
		nr::utl::CreateSocketNodeId("127.0.0.1", local_port), 
		buffer_size, JsonParser("command"), std::cout);
	peer->Action::Bind(client);
	peer->Behavior::Bind(server);

	server->StartAccept();
	TestSearchKeyHashAbilityCuiApp<JsonParser>(service, peer);	
}

void TestManySearchKeyHashAbility(){
	ManySearchKeyHashAbility(100);	
}

int main(int argc, char* argv[])
{
	//TestCuiApp(argc, argv);
	TestManySearchKeyHashAbility();
 
  	return 0;
}

#endif
