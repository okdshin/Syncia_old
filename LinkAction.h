#pragma once
//LinkAction:20120827
#include <iostream>
#include <algorithm>
#include <cassert>
#include <boost/algorithm/string.hpp>
#include "neuria/Neuria.h"
#include "command/Command.h"

namespace sy
{

class LinkAction : 
		public boost::enable_shared_from_this<LinkAction> {
public:
	using Pointer = boost::shared_ptr<LinkAction>;

	static auto Create(const cmd::CommandId& command_id, 
			nr::ntw::SessionPool::Pointer linked_session_pool, 
			const nr::NodeId& node_id,
			std::ostream& os) -> Pointer {
		return Pointer(new LinkAction(command_id, linked_session_pool, node_id, os));
	}
	
	auto Bind(nr::ntw::Client::Pointer client) -> void {
		this->client = client;
	}

	auto CreateLink(const nr::NodeId& target_node_id, 
			const nr::ByteArray& wrapped_byte_array) -> void {
		this->client->Connect(target_node_id,
			[this, wrapped_byte_array]
					(nr::ntw::Session::Pointer session){
				this->pool->Add(session);	
				session->Send(cmd::DispatchCommand(this->command_id,
						cmd::LinkCommand(
							this->node_id, wrapped_byte_array
						).Serialize()
					).Serialize(), 
					[](nr::ntw::Session::Pointer){});	
			},
			[this](nr::ntw::Session::Pointer session, 
					const nr::ByteArray& byte_array){
				this->os << "on receive from connected node!" << std::endl;//to do
			},
			[this](nr::ntw::Session::Pointer session){
				this->pool->Erase(session);	
			});
	}

private:
	LinkAction(const cmd::CommandId& command_id, nr::ntw::SessionPool::Pointer pool, 
		const nr::NodeId& node_id, std::ostream& os) 
		: command_id(command_id), pool(pool), node_id(node_id), os(os){}

	nr::ntw::Client::Pointer client;
	cmd::CommandId command_id;
	nr::ntw::SessionPool::Pointer pool;
	nr::NodeId node_id;
	std::ostream& os;
	
};

}

