#ifdef SEARCHKEYHASHQUERYBEHAVIOR_UNIT_TEST
#include "SearchKeyHashQueryBehavior.h"
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
	const int max_hop_count = 3;

	auto server = nr::ntw::SocketServer::Create(
		service, local_port, buffer_size, std::cout);
	auto dispatcher = nr::ntw::BehaviorDispatcher::Create(std::cout);
	dispatcher->Bind(server);

	auto connected_pool = nr::ntw::SessionPool::Create();

	auto file_db = nr::db::FileKeyHashDb::Create(0.3, buffer_size, std::cout);

	auto peer = SearchKeyHashQueryBehavior::Create(service, 
		nr::utl::CreateSocketNodeId("127.0.0.1", local_port), 
		max_hop_count, connected_pool, file_db, std::cout);
	peer->Bind(dispatcher);

	server->StartAccept();
	t.join();
 
  	return 0;
}

#endif
