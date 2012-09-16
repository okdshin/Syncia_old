#ifdef SYNCIA_UNIT_TEST
#include "Syncia.h"
#include <iostream>
#include "neuria/utility/Shell.h"
#include "neuria/utility/LabeledSink.h"

using namespace sy;

auto TestCui(Syncia::Pointer peer) -> void {
	auto shell = nr::utl::Shell(std::cout);
	nr::utl::RegisterExitFunc(shell);
	shell.Register("link", "create new search link.", 
		[peer](const nr::utl::Shell::ArgumentList& argument_list){
			std::cout << "call: create search link." << std::endl;
			peer->ConnectSearchLink(
				nr::utl::CreateSocketNodeId(argument_list.at(1), 
					boost::lexical_cast<int>(argument_list.at(2))));
		});
	shell.Register("search", "query search key hash.", 
		[peer](const nr::utl::Shell::ArgumentList& argument_list){
			std::cout << "call: search key hash" << std::endl;
			peer->QuerySearchKeyHash(nr::utl::Shell::ArgumentList(
				argument_list.begin()+1, argument_list.end()));
		});
	shell.Register("request", "request file.", 
		[peer](const nr::utl::Shell::ArgumentList& argument_list){
			std::cout << "call: request file" << std::endl;
			peer->RequestFile(
				nr::db::FileKeyHash::HashId(argument_list.at(1)),
				nr::utl::CreateSocketNodeId(argument_list.at(2), 
					boost::lexical_cast<int>(argument_list.at(3)))
			);
		});
	shell.Register("upls", "list up uploaded files.", 
		[peer](const nr::utl::Shell::ArgumentList& argument_list){
			std::cout << "call: upls" << std::endl;
			std::cout << peer->GetFileKeyHashDb() << std::endl;
		});
	shell.Register("upload", "upload file.", 
		[peer](const nr::utl::Shell::ArgumentList& argument_list){
			std::cout << "call: upload" << std::endl;
			auto file_path = boost::filesystem::path(argument_list.at(1));
			nr::db::FileKeyHash::Keyward keyward;
			if(argument_list.size() == 2){
				keyward = nr::db::FileKeyHash::Keyward(file_path.filename().string());
			}
			else{
				keyward = nr::db::FileKeyHash::Keyward(argument_list.at(2));
			}
			peer->UploadFile(keyward, file_path);
			std::cout << "call querysearch" << std::endl;
		});
	shell.Start();
	
}

auto CreatePeer(boost::asio::io_service& service, 
		const std::string& hostname, int local_port) -> Syncia::Pointer {
	const int buffer_size = 128;
	boost::asio::io_service::work w(service);
	boost::thread t(boost::bind(&boost::asio::io_service::run, &service));

	auto server = nr::ntw::SocketServer::Create(
		service, local_port, buffer_size, std::cout);
	auto dispatcher = nr::ntw::BehaviorDispatcher::Create(std::cout);
	dispatcher->Bind(server);
	
	int max_hop_count = 6;
	auto node_id = nr::utl::CreateSocketNodeId(hostname, local_port); 
	std::cout << node_id << std::endl;
	auto os = nr::utl::LabeledSink(node_id, std::cout);
	auto peer = Syncia::Create(service, node_id,
		buffer_size, max_hop_count, "./", 0.3, std::cout);
	
	peer->Bind(dispatcher);

	auto client = nr::ntw::SocketClient::Create(service, buffer_size, std::cout);
	peer->Bind(client);
	server->StartAccept();

	return peer;
}	

void TestManyPeer(int peer_num){
	boost::asio::io_service service;
	boost::asio::io_service::work w(service);
	boost::thread t(boost::bind(&boost::asio::io_service::run, &service));

	auto hostname = std::string("localhost");
	int base_port_num = 6000;
	auto peer_list = std::vector<Syncia::Pointer>();
	for(int i = 0; i < peer_num; i++){
		peer_list.push_back(CreatePeer(service, hostname, base_port_num+i));		
	}

	for(int k = 0 ; k < 2; k++){
		for(int i = 0; i < peer_num; i++){
			peer_list.at(i)->ConnectSearchLink(
				nr::utl::CreateSocketNodeId(
					hostname, base_port_num+nr::utl::Random(0, peer_num-1)));
		}
	}

	TestCui(peer_list.front());

	t.join();
}

int main(int argc, char* argv[])
{
	TestManyPeer(100);

    return 0;
}

#endif
