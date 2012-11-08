#ifdef SEARCHKEYHASHBEHAVIOR_UNIT_TEST
#include "SearchKeyHashBehavior.h"
#include "LinkBehavior.h"
#include "LinkAction.h"
#include "SearchKeyHashAction.h"
#include "neuria/test/CuiShell.h"
#include <iostream>

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
	const unsigned int max_key_hash_count = 5;
	const unsigned int max_hop_count = 3;

	std::stringstream no_output;
	
	auto server = neuria::network::SocketServer::Create(
		service, local_port, buffer_size, std::cout);
	auto dispatcher = BehaviorDispatcher::Create(service, std::cout);
	SetOnReceivedFuncOnly(server, dispatcher->GetOnReceivedFunc());

	auto client = neuria::network::SocketClient::Create(service, buffer_size, std::cout);
	
	auto upper_pool = neuria::network::SessionPool::Create();
	auto lower_pool = neuria::network::SessionPool::Create();
	auto node_id = neuria::network::CreateSocketNodeId("127.0.0.1", local_port);
	
	auto file_db = database::FileKeyHashDb::Create(0.3, buffer_size, std::cout);
	auto searched_file_db = database::FileKeyHashDb::Create(0.3, buffer_size, std::cout);
	
	auto search_key_hash_behavior = 
		SearchKeyHashBehavior::Create(node_id, max_key_hash_count, max_hop_count, 
			upper_pool, file_db, searched_file_db, std::cout);	
	search_key_hash_behavior->Bind(dispatcher);
	
	auto link_behavior = LinkBehavior::Create(lower_pool, 
		command::CommandId("test link query command"), std::cout);
	link_behavior->SetOnReceivedLinkQueryFunc(
		[](neuria::network::Session::Pointer session, const neuria::ByteArray& byte_array){
			std::cout << neuria::utility::ByteArray2String(byte_array) << std::endl;	
		});
	link_behavior->Bind(dispatcher);
	
	auto link_action = LinkAction::Create(command::CommandId("test link query command"), upper_pool, node_id, std::cout);
	link_action->Bind(client);

	auto shell = neuria::test::CuiShell(std::cout);
	neuria::test::RegisterExitFunc(shell);
	shell.Register("link", ": create new link.", 
		[link_action](const neuria::test::CuiShell::ArgList& argument_list){
			link_action->CreateLink(
				neuria::network::CreateSocketNodeId(
					argument_list.at(1), 
					boost::lexical_cast<int>(argument_list.at(2))),
				neuria::utility::String2ByteArray("link please!! 12345"));
		});

	auto search_key_hash_action = 
		SearchKeyHashAction::Create(upper_pool, node_id, std::cout);
	shell.Register("fetch", ": fetch",
		[search_key_hash_action]
		(const neuria::test::CuiShell::ArgList& argument_list){
			search_key_hash_action->SearchKeyHash(
				database::KeywordList(argument_list));
		});
	
	shell.Register("db", ": show db",
		[file_db](const neuria::test::CuiShell::ArgList& argument_list){
			std::cout << file_db << std::endl;
		});
	
	server->StartAccept();
	shell.Start();
	
	t.join();

    return 0;
}

#endif
