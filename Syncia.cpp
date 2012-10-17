#ifdef SYNCIA_UNIT_TEST
#include "Syncia.h"
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
	const unsigned int spread_key_hash_max_count = 2;
	const unsigned int max_hop_count = 3;

	std::stringstream no_output;
	
	auto server = neuria::network::SocketServer::Create(
		service, local_port, buffer_size, std::cout);
	auto dispatcher = BehaviorDispatcher::Create(service, std::cout);
	SetOnReceiveFuncOnly(server, dispatcher->GetOnReceiveFunc());

	auto client = neuria::network::SocketClient::Create(service, buffer_size, std::cout);
	
	auto node_id = neuria::network::CreateSocketNodeId("127.0.0.1", local_port);
	
	auto upper_session_pool = neuria::network::SessionPool::Create();
	auto lower_session_pool = neuria::network::SessionPool::Create();
	auto file_db = database::FileKeyHashDb::Create(0.3, buffer_size, std::cout);
	auto syncia = Syncia::Create(
		max_key_hash_count, spread_key_hash_max_count, max_hop_count, buffer_size, 
		upper_session_pool, lower_session_pool, file_db, node_id, std::cout);
	syncia->Bind(client);
	syncia->Bind(dispatcher);

	auto shell = neuria::test::CuiShell(std::cout);
	neuria::test::RegisterExitFunc(shell);
	
	shell.Register("upload", ": upload directory.", 
		[syncia](const neuria::test::CuiShell::ArgList& argument_list){
			//syncia->UploadDirectory(FileSystemPath(argument_list.at(1)));
		});
	shell.Register("db", ": show uploaded files.", 
		[file_db](const neuria::test::CuiShell::ArgList& argument_list){
			std::cout << file_db << std::endl;
		});
	shell.Register("link", ": create new search link.", 
		[syncia](const neuria::test::CuiShell::ArgList& argument_list){
			syncia->CreateSearchLink(
				neuria::network::CreateSocketNodeId(
					argument_list.at(1), 
					boost::lexical_cast<int>(argument_list.at(2))
				)
			);
		});
	shell.Register("search", ": search key hash.", 
		[syncia](const neuria::test::CuiShell::ArgList& argument_list){
			auto keyward_only = neuria::test::CuiShell::ArgList();
			std::copy(argument_list.begin()+1, argument_list.end(), 
				std::back_inserter(keyward_only));
			syncia->SearchKeyHash(database::KeywardList(keyward_only));
		});
	
	shell.Register("spread", ": request spread key hash.", 
		[syncia](const neuria::test::CuiShell::ArgList& argument_list){
			syncia->RequestSpreadKeyHash();
		});
	
	shell.Register("request", ": request file.", 
		[syncia](const neuria::test::CuiShell::ArgList& argument_list){
			syncia->RequestFile(database::HashId(argument_list.at(3)),
				neuria::network::CreateSocketNodeId(
					argument_list.at(1), 
					boost::lexical_cast<int>(argument_list.at(2))),
				FileSystemPath(argument_list.at(4))
			);
		});
	
	shell.Register("upper", ": show upper linked sessions.", 
		[upper_session_pool](const neuria::test::CuiShell::ArgList& argument_list){
			std::cout << "upper session: " << upper_session_pool << std::endl;
		});
	shell.Register("lower", ": show lower linked sessions.", 
		[lower_session_pool](const neuria::test::CuiShell::ArgList& argument_list){
			std::cout << "lower session: " << lower_session_pool << std::endl;
		});
	
	server->StartAccept();
	shell.Start();
	
	t.join();

    return 0;
}

#endif
