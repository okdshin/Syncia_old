#ifdef FETCHBEHAVIOR_UNIT_TEST
#include "FetchBehavior.h"
#include <iostream>
#include "LinkAction.h"
#include "LinkBehavior.h"
#include "CommandToRandomNodeAction.h"
#include "neuria/utility/Shell.h"

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
	const unsigned int max_hop_count = 3;

	std::stringstream no_output;
	
	auto server = nr::ntw::SocketServer::Create(
		service, local_port, buffer_size, std::cout);
	auto dispatcher = BehaviorDispatcher::Create(service, std::cout);
	SetOnReceiveFuncOnly(server, dispatcher->GetOnReceiveFunc());

	auto accepted_pool = nr::ntw::SessionPool::Create();
	auto connected_pool = nr::ntw::SessionPool::Create();
	auto node_id = nr::utl::CreateSocketNodeId("127.0.0.1", local_port);
	auto client = nr::ntw::SocketClient::Create(service, buffer_size, std::cout);
	
	auto fetch_behavior1 = FetchBehavior::Create(
		cmd::CommandId("fetch1_command"), node_id, accepted_pool, std::cout);

	fetch_behavior1->SetIsTurningPointDecider(
		[max_hop_count](const cmd::FetchCommand::Route& route, 
				const nr::ByteArray& byte_array) -> cmd::IsAnswer {
			return cmd::IsAnswer(route.size() >= max_hop_count);
		});

	fetch_behavior1->SetFetchQueryRedirector(
		[](const nr::ByteArray& byte_array) -> nr::ByteArray {
			std::cout << nr::utl::ByteArray2String(byte_array) << std::endl;
			return byte_array;
		});

	fetch_behavior1->SetFetchAnswerRedirector(
		[](const nr::ByteArray& byte_array) -> nr::ByteArray {
			std::cout << nr::utl::ByteArray2String(byte_array) << std::endl;
			return byte_array;
		});
	
	fetch_behavior1->SetOnReceiveFetchAnswerFunc(
		[](nr::ntw::Session::Pointer session, const nr::ByteArray& byte_array){
			std::cout << "got answer!!" << std::endl;
		});
	
	fetch_behavior1->Bind(dispatcher);	
	fetch_behavior1->Bind(client);
	
	auto fetch_behavior2 = FetchBehavior::Create(
		cmd::CommandId("fetch2_command"), node_id, accepted_pool, std::cout);

	fetch_behavior2->SetIsTurningPointDecider(
		[max_hop_count](const cmd::FetchCommand::Route& route, 
				const nr::ByteArray& byte_array) -> cmd::IsAnswer {
			return cmd::IsAnswer(route.size() >= max_hop_count);
		});

	fetch_behavior2->SetFetchQueryRedirector(
		[](const nr::ByteArray& byte_array) -> nr::ByteArray {
			std::cout << "2222" << nr::utl::ByteArray2String(byte_array) << std::endl;
			return byte_array;
		});

	fetch_behavior2->SetFetchAnswerRedirector(
		[](const nr::ByteArray& byte_array) -> nr::ByteArray {
			std::cout << nr::utl::ByteArray2String(byte_array) << std::endl;
			return byte_array;
		});
	
	fetch_behavior2->SetOnReceiveFetchAnswerFunc(
		[](nr::ntw::Session::Pointer session, const nr::ByteArray& byte_array){
			std::cout << "got answer!!" << std::endl;
		});
	
	fetch_behavior2->Bind(dispatcher);	
	fetch_behavior2->Bind(client);
	
	auto link_behavior = LinkBehavior::Create(accepted_pool, 
		cmd::CommandId("test link query command"), std::cout);
	link_behavior->SetOnReceiveLinkQueryFunc(
		[](nr::ntw::Session::Pointer session, const nr::ByteArray& byte_array){
			std::cout << nr::utl::ByteArray2String(byte_array) << std::endl;	
		});

	link_behavior->Bind(dispatcher);
	
	server->StartAccept();

	auto link_action = LinkAction::Create(connected_pool, std::cout);
	
	link_action->Bind(client);

	auto shell = nr::utl::Shell(std::cout);
	nr::utl::RegisterExitFunc(shell);
	shell.Register("link", ": create new link.", 
		[link_action](const nr::utl::Shell::ArgumentList& argument_list){
			link_action->CreateLink(
				nr::utl::CreateSocketNodeId(
					argument_list.at(1), 
					boost::lexical_cast<int>(argument_list.at(2))),
				cmd::CommandId("test link query command"),
				nr::utl::String2ByteArray("link please!! 12345"));
		});

	auto command_to_random_node_action = 
		CommandToRandomNodeAction::Create(connected_pool, std::cout);
	shell.Register("fetch1", ": fetch",
		[command_to_random_node_action, node_id]
		(const nr::utl::Shell::ArgumentList& argument_list){
			auto initial_route = cmd::FetchCommand::Route();
			initial_route.push_back(node_id);
			command_to_random_node_action->CommandToRandomNode(
				cmd::CommandId("fetch1_command"),
				cmd::FetchCommand(cmd::IsAnswer(false), initial_route,
					nr::utl::String2ByteArray("fetch11111111!!!")).Serialize());
		});
	shell.Register("fetch2", ": fetch",
		[command_to_random_node_action, node_id]
		(const nr::utl::Shell::ArgumentList& argument_list){
			auto initial_route = cmd::FetchCommand::Route();
			initial_route.push_back(node_id);
			command_to_random_node_action->CommandToRandomNode(
				cmd::CommandId("fetch2_command"),
				cmd::FetchCommand(cmd::IsAnswer(false), initial_route,
					nr::utl::String2ByteArray("fetch222222222!!!")).Serialize());
		});
	shell.Start();

	t.join();
  	return 0;
}

#endif
