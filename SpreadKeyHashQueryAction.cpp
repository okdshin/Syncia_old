#ifdef SPREADKEYHASHQUERYACTION_UNIT_TEST
#include "SpreadKeyHashQueryAction.h"
#include <iostream>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include "neuria/utility/Shell.h"

using namespace sy;

void TestCuiApp(int argc, char* argv[]){
	boost::asio::io_service service;
	boost::asio::io_service::work w(service);
	boost::thread t(boost::bind(&boost::asio::io_service::run, &service));

	const int buffer_size = 128;
	auto client = nr::ntw::SocketClient::Create(service, buffer_size, std::cout);
	
	int local_port = 98765;
	auto connected_pool = nr::ntw::SessionPool::Create();
	auto peer = SpreadKeyHashQueryAction::Create(service, 
		nr::utl::CreateSocketNodeId("127.0.0.1", local_port), 
		buffer_size, connected_pool, std::cout);
	
	peer->Bind(client);

	auto shell = nr::utl::Shell(std::cout);
	nr::utl::RegisterExitFunc(shell);
	shell.Register("searchlink", "create new search link.", 
		[peer](const nr::utl::Shell::ArgumentList& argument_list){
			std::cout << "call searchlink." << std::endl;
			peer->ConnectSearchLink(
				nr::utl::CreateSocketNodeId(argument_list.at(1), 
					boost::lexical_cast<int>(argument_list.at(2))));
		});
	shell.Register("querysearch", "query search key hash.", 
		[peer](const nr::utl::Shell::ArgumentList& argument_list){
			std::cout << "call querysearch" << std::endl;
			peer->QuerySearchKeyHash(argument_list);
		});
	shell.Start();
	t.join();
}

int main(int argc, char* argv[])
{
	TestCuiApp(argc, argv);
	//TestManySpreadKeyHashQueryAction();
 
  	return 0;
}

#endif
