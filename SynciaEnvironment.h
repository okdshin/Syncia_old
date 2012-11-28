#pragma once
//SynciaEnvironment:20121003
#define BOOST_ASIO_ENABLE_HANDLER_TRACKING
#include <iostream>
#include "SynciaCore.h"

namespace syncia
{
class SynciaEnvironment{
public:
	using Pointer = boost::shared_ptr<SynciaEnvironment>;

	static auto Create(
			int max_key_hash_count, int spread_key_hash_max_count, 
			int max_hop_count, int local_port, int buffer_size, int thread_num, 
			const neuria::network::NodeId& node_id,
			neuria::network::SessionPool::Pointer upper_pool,
			neuria::network::SessionPool::Pointer lower_pool,
			syncia::database::FileKeyHashDb::Pointer file_db,
			syncia::database::FileKeyHashDb::Pointer searched_file_db
				) -> Pointer {
		return Pointer(new SynciaEnvironment(
			max_key_hash_count, spread_key_hash_max_count, 
			max_hop_count, local_port, buffer_size, thread_num, 
			node_id, upper_pool, lower_pool, file_db, searched_file_db));

	}

    ~SynciaEnvironment(){
        this->service_ptr->stop();
        std::cout << "env deleted." << std::endl;
	}
	
	auto Initialize() -> void {
		this->syncia->Bind(this->dispatcher);	
		this->syncia->Bind(this->client);	
		this->syncia->Bind(this->multiple_timer);
	}

	auto GetSocketSyncia() -> syncia::SynciaCore::Pointer {
		return this->syncia;	
	}

	auto StartAccept() -> void {
		this->server->StartAccept(
			neuria::network::Server::OnAcceptedFunc([this](
					neuria::network::Session::Pointer session){
				session->StartReceive(this->dispatcher->GetOnReceivedFunc());
			}),
			neuria::network::Server::OnFailedAcceptFunc([](
					const neuria::network::ErrorCode){
				//nothing
			}),
			neuria::network::Session::OnClosedFunc([](
					neuria::network::Session::Pointer){
				//nothing
			})
		);
	}

	auto Join() -> void {
        this->thread_group.join_all();
	}

private:
	SynciaEnvironment(
			int max_key_hash_count, int spread_key_hash_max_count, 
			int max_hop_count, int local_port, int buffer_size, int thread_num, 
			const neuria::network::NodeId& node_id,
			neuria::network::SessionPool::Pointer upper_pool,
			neuria::network::SessionPool::Pointer lower_pool,
			syncia::database::FileKeyHashDb::Pointer file_db,
			syncia::database::FileKeyHashDb::Pointer searched_file_db)
		:service_ptr(new boost::asio::io_service), 
		work_ptr(new boost::asio::io_service::work(*(this->service_ptr))), 
		thread_group(),
		client(neuria::network::SocketClient::Create(*service_ptr, buffer_size, std::cout)),
		server(neuria::network::SocketServer::Create(
			*service_ptr, local_port, buffer_size, std::cout)),
		dispatcher(syncia::BehaviorDispatcher::Create(*service_ptr, std::cout)),
		multiple_timer(neuria::timer::MultipleTimer::Create(*service_ptr)),
		syncia(syncia::SynciaCore::Create(
			max_key_hash_count, spread_key_hash_max_count, 
			max_hop_count, buffer_size, upper_pool, lower_pool, 
			file_db, searched_file_db, node_id, std::cout)){
		for(int i = 0; i < thread_num; ++i){
			this->thread_group.create_thread(
				boost::bind(&boost::asio::io_service::run, service_ptr));
		}
	}


private:
	boost::shared_ptr<boost::asio::io_service> service_ptr;
	boost::shared_ptr<boost::asio::io_service::work> work_ptr;
	boost::thread_group thread_group;
	neuria::network::SessionPool::Pointer upper_pool, lower_pool;
	neuria::network::Client::Pointer client;
	neuria::network::Server::Pointer server;
	syncia::BehaviorDispatcher::Pointer dispatcher;
	neuria::timer::MultipleTimer::Pointer multiple_timer;
	syncia::SynciaCore::Pointer syncia;

};
}
