#ifdef FETCHBEHAVIOR_UNIT_TEST
#include "FetchBehavior.h"
#include <iostream>
#include "LinkAction.h"
#include "LinkBehavior.h"
#include "CommandToRandomNodeAction.h"
#include "neuria/test/CuiShell.h"

using namespace syncia;

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
	const unsigned int max_hop_count = 6;

	std::stringstream no_output;
	
	auto server = neuria::network::SocketServer::Create(
		service, local_port, buffer_size, no_output);
	auto dispatcher = BehaviorDispatcher::Create(service, no_output);
	SetOnReceivedFuncOnly(server, dispatcher->GetOnReceivedFunc());

	auto accepted_pool = neuria::network::SessionPool::Create();
	auto connected_pool = neuria::network::SessionPool::Create();
	auto node_id = neuria::network::CreateSocketNodeId("127.0.0.1", local_port);
	auto client = neuria::network::SocketClient::Create(service, buffer_size, no_output);
	
	auto fetch_behavior1 = FetchBehavior::Create(
		command::CommandId("fetch1_command"), node_id, accepted_pool, std::cout);

	fetch_behavior1->SetIsTurningPointDecider(
		[max_hop_count](const command::FetchCommand::Route& route, 
				const neuria::ByteArray& byte_array) -> command::IsAnswer {
			return command::IsAnswer(route.size() >= max_hop_count);
		});

	fetch_behavior1->SetFetchQueryRedirector(
		[](const neuria::ByteArray& byte_array) -> neuria::ByteArray {
			std::cout << neuria::utility::ByteArray2String(byte_array) << std::endl;
			return byte_array;
		});

	fetch_behavior1->SetFetchAnswerRedirector(
		[](const neuria::ByteArray& byte_array) -> neuria::ByteArray {
			std::cout << neuria::utility::ByteArray2String(byte_array) << std::endl;
			return byte_array;
		});
	
	fetch_behavior1->SetOnReceivedFetchAnswerFunc(
		[](neuria::network::Session::Pointer session, const neuria::ByteArray& byte_array){
			std::cout << "got answer!!" << std::endl;
		});
	
	fetch_behavior1->Bind(dispatcher);	
	fetch_behavior1->Bind(client);
	
	auto fetch_behavior2 = FetchBehavior::Create(
		command::CommandId("fetch2_command"), node_id, accepted_pool, std::cout);

	fetch_behavior2->SetIsTurningPointDecider(
		[max_hop_count](const command::FetchCommand::Route& route, 
				const neuria::ByteArray& byte_array) -> command::IsAnswer {
			return command::IsAnswer(route.size() >= max_hop_count);
		});

	fetch_behavior2->SetFetchQueryRedirector(
		[](const neuria::ByteArray& byte_array) -> neuria::ByteArray {
			std::cout << "2222" << neuria::utility::ByteArray2String(byte_array) << std::endl;
			return byte_array;
		});

	fetch_behavior2->SetFetchAnswerRedirector(
		[](const neuria::ByteArray& byte_array) -> neuria::ByteArray {
			std::cout << neuria::utility::ByteArray2String(byte_array) << std::endl;
			return byte_array;
		});
	
	fetch_behavior2->SetOnReceivedFetchAnswerFunc(
		[](neuria::network::Session::Pointer session, const neuria::ByteArray& byte_array){
			std::cout << "got answer!!" << std::endl;
		});
	
	fetch_behavior2->Bind(dispatcher);	
	fetch_behavior2->Bind(client);
	
	auto link_behavior = LinkBehavior::Create(accepted_pool, 
		command::CommandId("link"), std::cout);
	link_behavior->SetOnReceivedLinkQueryFunc(
		[](neuria::network::Session::Pointer session, const neuria::ByteArray& byte_array){
			std::cout << neuria::utility::ByteArray2String(byte_array) << std::endl;	
		});

	link_behavior->Bind(dispatcher);
	link_behavior->Bind(client);
	
	server->StartAccept();

	auto link_action = LinkAction::Create(command::CommandId("link"), connected_pool, 
		node_id, std::cout);
	
	link_action->Bind(client);

	auto shell = neuria::test::CuiShell(std::cout);
	neuria::test::RegisterExitFunc(shell);
	shell.Register("link", ": create new link.", 
		[link_action](const neuria::test::CuiShell::ArgList& argument_list){
			link_action->CreateLink(
				neuria::network::CreateSocketNodeId(
					argument_list.at(1), 
					boost::lexical_cast<int>(argument_list.at(2))),
				//command::CommandId("test link query command"),
				neuria::utility::String2ByteArray("link please!! 12345"));
		});

	auto command_to_random_node_action = 
		CommandToRandomNodeAction::Create(connected_pool, std::cout);
	shell.Register("fetch1", ": fetch",
		[command_to_random_node_action, node_id]
		(const neuria::test::CuiShell::ArgList& argument_list){
			auto initial_route = command::FetchCommand::Route();
			initial_route.push_back(node_id);
			command_to_random_node_action->CommandToRandomNode(
				command::CommandId("fetch1_command"),
				command::FetchCommand(command::IsAnswer(false), initial_route,
					neuria::utility::String2ByteArray("fetch11111111!!!")).Serialize());
		});
	shell.Register("fetch2", ": fetch",
		[command_to_random_node_action, node_id]
		(const neuria::test::CuiShell::ArgList& argument_list){
			auto initial_route = command::FetchCommand::Route();
			initial_route.push_back(node_id);
			command_to_random_node_action->CommandToRandomNode(
				command::CommandId("fetch2_command"),
				command::FetchCommand(command::IsAnswer(false), initial_route,
					neuria::utility::String2ByteArray("fetch222222222!!!")).Serialize());
		});
	shell.Start();

	t.join();
  	return 0;
}

#endif
