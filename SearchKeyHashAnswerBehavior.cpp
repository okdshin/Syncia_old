#ifdef SEARCHKEYHASHANSWERBEHAVIOR_UNIT_TEST
#include "SearchKeyHashAnswerBehavior.h"
#include <iostream>

using namespace sy;

int main(int argc, char* argv[])
{
	boost::asio::io_service service;
	boost::asio::io_service::work w(service);
	boost::thread t(boost::bind(&boost::asio::io_service::run, &service));
	
	int local_port = 54321;
	if(argc == 2)
	{
		local_port = boost::lexical_cast<int>(argv[1]);
	}

	const int buffer_size = 128;
	
	auto server = nr::ntw::SocketServer::Create(
		service, local_port, buffer_size, std::cout);
	auto dispatcher = nr::ntw::BehaviorDispatcher::Create();
	dispatcher->Bind(server);

	auto client = nr::ntw::SocketClient::Create(service, buffer_size, std::cout);

	auto peer = SearchKeyHashAnswerBehavior::Create(service, 
		nr::utl::CreateSocketNodeId("127.0.0.1", local_port), 
		buffer_size, std::cout);
	peer->Bind(dispatcher);
	peer->Bind(client);

	server->StartAccept();
	t.join();
 
  	return 0;
}

#endif
