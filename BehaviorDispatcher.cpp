#ifdef BEHAVIORDISPATCHER_UNIT_TEST
#include "BehaviorDispatcher.h"
#include <iostream>

using namespace sy;

int main(int argc, char* argv[])
{
	boost::asio::io_service service;
	boost::asio::io_service::work w(service);
	boost::thread t(boost::bind(&boost::asio::io_service::run, &service));
	auto server = nr::ntw::SocketServer::Create(service, 54321, 128, std::cout);
	auto dispatcher = BehaviorDispatcher::Create(service, std::cout);
	dispatcher->Bind(server);
	std::cout << dispatcher << std::endl;
	server->StartAccept();
	t.join();

    return 0;
}

#endif