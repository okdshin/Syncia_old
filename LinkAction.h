#pragma once
//LinkAction:20120827
#include <iostream>
#include <algorithm>
#include <cassert>
#include <boost/algorithm/string.hpp>
#include "neuria/Neuria.h"
#include "command/Command.h"

namespace syncia
{

class LinkAction : 
		public boost::enable_shared_from_this<LinkAction> {
public:
	using Pointer = boost::shared_ptr<LinkAction>;
	using OnFailedCreateLinkFunc = neuria::network::Client::OnFailedConnectFunc;

	static auto Create(const command::CommandId& command_id, 
			neuria::network::SessionPool::Pointer linked_session_pool, 
			const neuria::network::NodeId& node_id,
			OnFailedCreateLinkFunc on_failed_create_link_func, 
			std::ostream& os) -> Pointer {
		return Pointer(new LinkAction(
			command_id, linked_session_pool, node_id, on_failed_create_link_func, os));
	}
	
	auto Bind(neuria::network::Client::Pointer client) -> void {
		this->client = client;
	}

	auto CreateLink(const neuria::network::NodeId& target_node_id, 
			const neuria::ByteArray& wrapped_byte_array) -> void {
		this->client->Connect(target_node_id,
			[this, wrapped_byte_array]
					(neuria::network::Session::Pointer session){
				this->pool->Add(session);	
				session->Send(command::DispatchCommand(this->command_id,
						command::LinkCommand(
							this->node_id, wrapped_byte_array
						).Serialize()
					).Serialize(), 
					[](neuria::network::Session::Pointer){});	
			},
			[this](const neuria::network::ErrorCode& error_code){
				std::cout << "link action error:" << error_code << std::endl;	
				this->on_failed_create_link_func(error_code);
			},
			[this](neuria::network::Session::Pointer session, 
					const neuria::ByteArray& byte_array){
				this->os << "on receive from connected node!" << std::endl;//to do
			},
			[this](neuria::network::Session::Pointer session){
				this->pool->Erase(session);	
			}
		);
	}

private:
	LinkAction(const command::CommandId& command_id, 
		neuria::network::SessionPool::Pointer pool, 
		const neuria::network::NodeId& node_id, 
		neuria::network::Client::OnFailedConnectFunc on_failed_create_link_func, 
		std::ostream& os) 
		: command_id(command_id), pool(pool), node_id(node_id), 
		on_failed_create_link_func(on_failed_create_link_func), os(os){}

	neuria::network::Client::Pointer client;
	command::CommandId command_id;
	neuria::network::SessionPool::Pointer pool;
	neuria::network::NodeId node_id;
	OnFailedCreateLinkFunc on_failed_create_link_func;
	std::ostream& os;
	
};

}

