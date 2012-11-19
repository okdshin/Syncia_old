#ifdef LINKACTION_UNIT_TEST
#include "LinkAction.h"
#include <iostream>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include "neuria/test/CuiShell.h"

using namespace syncia;

void TestCuiApp(int argc, char* argv[]){
	int local_port = 54321;
	auto hostname = std::string("localhost");
	if(argc > 1){
		hostname = std::string(argv[1]);
		if(argc == 3)
		{
			local_port = boost::lexical_cast<int>(std::string(argv[2]));
		}	
	}
	auto node_id = neuria::network::CreateSocketNodeId(hostname, local_port);
	std::cout << "NodeId is " << node_id << std::endl; 
	boost::asio::io_service service;
	boost::asio::io_service::work w(service);
	boost::thread t(boost::bind(&boost::asio::io_service::run, &service));

	const int buffer_size = 128;
	auto client = neuria::network::SocketClient::Create(service, buffer_size, std::cout);
	
	auto connected_pool = neuria::network::SessionPool::Create();
	auto link_action = LinkAction::Create(
		command::CommandId("test link query command"), connected_pool, node_id, std::cout);
	
	link_action->Bind(client);

	auto shell = neuria::test::CuiShell(std::cout);
	neuria::test::RegisterExitFunc(shell);
	shell.Register("link", "create new link.", 
		[link_action](const neuria::test::CuiShell::ArgList& argument_list){
			link_action->CreateLink(
				neuria::network::CreateSocketNodeId(
					argument_list.at(1), 
					boost::lexical_cast<int>(argument_list.at(2))),
				neuria::utility::String2ByteArray("link please!! 12345"));
		});
	shell.Register("upper", "show upper linked sessions.", 
		[connected_pool](const neuria::test::CuiShell::ArgList& args){
			std::cout << "upper session: " << connected_pool << std::endl;
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
