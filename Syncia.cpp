#ifdef SYNCIA_UNIT_TEST
#include "Syncia.h"
#include <iostream>
#include "neuria/utility/Shell.h"

using namespace sy;

void TestCui(Syncia::Pointer peer){
	auto shell = nr::utl::Shell(std::cout);
	nr::utl::RegisterExitFunc(shell);
	shell.Register("link", "create new search link.", 
		[peer](const nr::utl::Shell::ArgumentList& argument_list){
			std::cout << "call searchlink." << std::endl;
			peer->ConnectSearchLink(
				nr::utl::CreateSocketNodeId(argument_list.at(1), 
					boost::lexical_cast<int>(argument_list.at(2))));
		});
	shell.Register("search", "query search key hash.", 
		[peer](const nr::utl::Shell::ArgumentList& argument_list){
			std::cout << "call querysearch" << std::endl;
			peer->QuerySearchKeyHash(argument_list);
		});
	shell.Start();
	
}

void TestCuiApp(int argc, char* argv[]){
	boost::asio::io_service service;
	boost::asio::io_service::work w(service);
	boost::thread t(boost::bind(&boost::asio::io_service::run, &service));

	const int buffer_size = 128;
	
	int local_port = 54321;
	if(argc == 2)
	{
		local_port = boost::lexical_cast<int>(argv[1]);
	}
	auto server = nr::ntw::SocketServer::Create(
		service, local_port, buffer_size, std::cout);
	auto dispatcher = nr::ntw::BehaviorDispatcher::Create(std::cout);
	dispatcher->Bind(server);
	
	int max_hop_count = 3;
	auto peer = Syncia::Create(service, 
		nr::utl::CreateSocketNodeId("127.0.0.1", local_port), 
		buffer_size, max_hop_count, std::cout);
	
	peer->Bind(dispatcher);
	std::cout << "dispatcher:" << dispatcher << std::endl;

	auto client = nr::ntw::SocketClient::Create(service, buffer_size, std::cout);
	peer->Bind(client);
	server->StartAccept();
	
	TestCui(peer);

	t.join();
}

int main(int argc, char* argv[])
{
	TestCuiApp(argc, argv);

    return 0;
}

#endif
