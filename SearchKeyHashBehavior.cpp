#ifdef SEARCHKEYHASHBEHAVIOR_UNIT_TEST
#include "SearchKeyHashBehavior.h"
#include "LinkBehavior.h"
#include "LinkAction.h"
#include "SearchKeyHashAction.h"
#include "neuria/utility/Shell.h"
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
	const unsigned int max_key_hash_count = 5;
	const unsigned int max_hop_count = 3;

	std::stringstream no_output;
	
	auto server = nr::ntw::SocketServer::Create(
		service, local_port, buffer_size, std::cout);
	auto dispatcher = BehaviorDispatcher::Create(service, std::cout);
	SetOnReceiveFuncOnly(server, dispatcher->GetOnReceiveFunc());

	auto client = nr::ntw::SocketClient::Create(service, buffer_size, std::cout);
	
	auto upper_pool = nr::ntw::SessionPool::Create();
	auto lower_pool = nr::ntw::SessionPool::Create();
	auto node_id = nr::utl::CreateSocketNodeId("127.0.0.1", local_port);
	
	auto file_db = nr::db::FileKeyHashDb::Create(0.3, buffer_size, std::cout);
	
	auto search_key_hash_behavior = 
		SearchKeyHashBehavior::Create(node_id, max_key_hash_count, max_hop_count, 
			upper_pool, file_db, std::cout);	
	search_key_hash_behavior->Bind(dispatcher);
	
	auto link_behavior = LinkBehavior::Create(lower_pool, 
		cmd::CommandId("test link query command"), std::cout);
	link_behavior->SetOnReceiveLinkQueryFunc(
		[](nr::ntw::Session::Pointer session, const nr::ByteArray& byte_array){
			std::cout << nr::utl::ByteArray2String(byte_array) << std::endl;	
		});
	link_behavior->Bind(dispatcher);
	
	auto link_action = LinkAction::Create(upper_pool, std::cout);
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

	auto search_key_hash_action = 
		SearchKeyHashAction::Create(upper_pool, node_id, std::cout);
	shell.Register("fetch", ": fetch",
		[search_key_hash_action]
		(const nr::utl::Shell::ArgumentList& argument_list){
			search_key_hash_action->QuerySearchKeyHash(
				nr::db::KeywardList(argument_list));
		});
	
	shell.Register("db", ": show db",
		[file_db](const nr::utl::Shell::ArgumentList& argument_list){
			std::cout << file_db << std::endl;
		});
	
	server->StartAccept();
	shell.Start();
	
	t.join();

    return 0;
}

#endif
