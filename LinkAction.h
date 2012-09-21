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

	static auto Create(nr::ntw::SessionPool::Pointer connected_pool, 
			std::ostream& os) -> Pointer {
		return Pointer(
			new LinkAction(connected_pool, os));
	}
	
	auto Bind(nr::ntw::Client::Pointer client) -> void {
		this->client = client;
	}

	auto ConnectFetchLink(const nr::NodeId& node_id, 
			const nr::ntw::DispatchCommand::CommandId& link_command_id,
			const nr::ByteArray& serialized_command_byte_array) -> void {
		this->client->Connect(node_id,
			[this, link_command_id, serialized_command_byte_array]
					(nr::ntw::Session::Pointer session){
				this->connected_pool->Add(session);	
				session->Send(nr::ntw::DispatchCommand(link_command_id,
					serialized_command_byte_array).Serialize());	
			},
			[this](nr::ntw::Session::Pointer session, 
					const nr::ByteArray& byte_array){
				this->os << "on receive from connected node!" << std::endl;//to do
			},
			[this](nr::ntw::Session::Pointer session){
				this->connected_pool->Erase(session);	
			});
	}

	auto QueryRequestAtRandom(const nr::ntw::DispatchCommand::CommandId& command_id, 
			const nr::ByteArray& serialized_command_byte_array) -> void {	
		this->at_random_selector(*connected_pool)->Send(nr::ntw::DispatchCommand(
			command_id, serialized_command_byte_array).Serialize());
	}

private:
	LinkAction(
		nr::ntw::SessionPool::Pointer connected_pool, std::ostream& os) 
		: connected_pool(connected_pool), os(os){}
	
	nr::ntw::Client::Pointer client;
	nr::utl::RandomElementSelector at_random_selector;
	nr::ntw::SessionPool::Pointer connected_pool;
	std::ostream& os;
	
};

}

