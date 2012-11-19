#ifdef LINKBEHAVIOR_UNIT_TEST
#include "LinkBehavior.h"
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
	auto server = neuria::network::SocketServer::Create(
		service, local_port, buffer_size, std::cout);
	auto dispatcher = BehaviorDispatcher::Create(service, std::cout);
	
	auto client = neuria::network::SocketClient::Create(service, buffer_size, std::cout);

	auto pool = neuria::network::SessionPool::Create();

	auto link_behavior = LinkBehavior::Create(pool, 
		command::CommandId("test link query command"), std::cout);
	link_behavior->SetOnReceivedLinkQueryFunc(
		[](neuria::network::Session::Pointer session, const neuria::ByteArray& byte_array){
			std::cout << neuria::utility::ByteArray2String(byte_array) << std::endl;	
		});

	link_behavior->Bind(dispatcher);
	link_behavior->Bind(client);

	server->StartAccept(
		neuria::network::Server::OnAcceptedFunc([dispatcher](
				neuria::network::Session::Pointer session){
			session->StartReceive(dispatcher->GetOnReceivedFunc());
		}),
		neuria::network::Server::OnFailedAcceptFunc([](
				const neuria::network::ErrorCode&){
			//nothing
		}),
		neuria::network::Session::OnClosedFunc([pool](
				neuria::network::Session::Pointer session){
			//nothing
		})
	);
	t.join();
    return 0;
}

#endif
