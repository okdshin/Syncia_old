#ifdef LINKBEHAVIOR_UNIT_TEST
#include "LinkBehavior.h"
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
	auto server = nr::ntw::SocketServer::Create(
		service, local_port, buffer_size, std::cout);
	auto dispatcher = BehaviorDispatcher::Create(service, std::cout);
	SetOnReceiveFuncOnly(server, dispatcher->GetOnReceiveFunc());
	
	auto client = nr::ntw::SocketClient::Create(service, buffer_size, std::cout);

	auto pool = nr::ntw::SessionPool::Create();

	auto link_behavior = LinkBehavior::Create(pool, 
		cmd::CommandId("test link query command"), std::cout);
	link_behavior->SetOnReceiveLinkQueryFunc(
		[](nr::ntw::Session::Pointer session, const nr::ByteArray& byte_array){
			std::cout << nr::utl::ByteArray2String(byte_array) << std::endl;	
		});

	link_behavior->Bind(dispatcher);
	link_behavior->Bind(client);

	server->StartAccept();
	t.join();
    return 0;
}

#endif
