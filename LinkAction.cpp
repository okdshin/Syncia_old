#ifdef LINKACTION_UNIT_TEST
#include "LinkAction.h"
#include <iostream>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include "neuria/test/CuiShell.h"

using namespace syncia;

void TestCuiApp(int argc, char* argv[]){
	boost::asio::io_service service;
	boost::asio::io_service::work w(service);
	boost::thread t(boost::bind(&boost::asio::io_service::run, &service));

	const int buffer_size = 128;
	auto client = neuria::network::SocketClient::Create(service, buffer_size, std::cout);
	
	auto connected_pool = neuria::network::SessionPool::Create();
	auto link_action = LinkAction::Create(
		command::CommandId("test link query command"), connected_pool, node_id, std::cout);
	
	link_action->Bind(client);

	auto shell = nr::utl::Shell(std::cout);
	nr::utl::RegisterExitFunc(shell);
	shell.Register("link", "create new link.", 
		[link_action](const nr::utl::Shell::ArgumentList& argument_list){
			link_action->CreateLink(
				nr::utl::CreateSocketNodeId(
					argument_list.at(1), 
					boost::lexical_cast<int>(argument_list.at(2))),
				nr::utl::String2ByteArray("link please!! 12345"));
		});
	shell.Start();
	t.join();
}

int main(int argc, char* argv[])
{
	TestCuiApp(argc, argv);
	//TestManyLinkAction();
 
  	return 0;
}

#endif
