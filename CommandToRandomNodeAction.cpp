#ifdef COMMANDTORANDOMNODEACTION_UNIT_TEST
#include "CommandToRandomNodeAction.h"
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
	auto command_to_random_node_action = 
		CommandToRandomNodeAction::Create(connected_pool, std::cout);
	
	command_to_random_node_action->Bind(client);

	auto shell = neuria::test::CuiShell(std::cout);
	neuria::test::RegisterExitFunc(shell);
	shell.Register("link", "create new link.", 
		[command_to_random_node_action](const neuria::test::CuiShell::ArgumentList& argument_list){
			command_to_random_node_action->CreateLink(
				nr::utl::CreateSocketNodeId(
					argument_list.at(1), 
					boost::lexical_cast<int>(argument_list.at(2))),
				neuria::network::DispatchCommand::CommandId("test link query command"),
				nr::utl::String2ByteArray("link please!! 12345"));
		});
	shell.Register("query", "query to linked node at random.",
		[command_to_random_node_action](const nr::utl::Shell::ArgumentList& argument_list){
			command_to_random_node_action->QueryRequestAtRandom(
				neuria::network::DispatchCommand::CommandId("test fetch query"),
				nr::utl::String2ByteArray("fetch! fetch!! fetch!!!"));
		});
	shell.Start();
	t.join();
}

int main(int argc, char* argv[])
{
	TestCuiApp(argc, argv);
	//TestManyCommandToRandomNodeAction();
 
  	return 0;
}

#endif
