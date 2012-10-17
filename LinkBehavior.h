#pragma once
//LinkBehavior:20120922
#include <iostream>
#include "neuria/Neuria.h"
#include "command/Command.h"
#include "BehaviorDispatcher.h"

namespace syncia
{

class LinkBehavior :
		public boost::enable_shared_from_this<LinkBehavior> {
public:
	using Pointer = boost::shared_ptr<LinkBehavior>;
	
	static auto Create(const neuria::network::SessionPool::Pointer& pool, 
			const command::CommandId& link_command_id,
			std::ostream& os) -> Pointer {
		return Pointer(new LinkBehavior(pool, link_command_id, os));
	}

	auto SetOnReceiveLinkQueryFunc(neuria::network::Session::OnReceiveFunc func) -> void {
		this->on_receive_link_query_func = func;
	}

	auto Bind(BehaviorDispatcher::Pointer dispatcher) -> void {
		dispatcher->RegisterFunc(link_command_id,
			boost::bind(&LinkBehavior::OnReceiveLinkQuery, 
				this->shared_from_this(), _1, _2));
	}

	auto Bind(neuria::network::Client::Pointer client) -> void {
		this->client = client;	
	}

private:
    LinkBehavior(neuria::network::SessionPool::Pointer pool, 
			const command::CommandId& link_command_id, 
			std::ostream& os) 
		: pool(pool), link_command_id(link_command_id), os(os){}
	
	auto OnReceiveLinkQuery(neuria::network::Session::Pointer session, 
			const neuria::ByteArray& byte_array) -> void {
		this->os << "on receive link query." << std::endl;
		auto command = command::LinkCommand::Parse(byte_array);
		std::cout << "LowerNodeId:" << command.GetNodeId() << std::endl;
		neuria::network::Connect(this->client, command.GetNodeId(), this->pool, 
			[](neuria::network::Session::Pointer, const neuria::ByteArray&){});
		this->on_receive_link_query_func(session, command.GetWrappedByteArray());
	}

	neuria::network::Session::OnReceiveFunc on_receive_link_query_func;
	neuria::network::SessionPool::Pointer pool;
	neuria::network::Client::Pointer client;
	command::CommandId link_command_id;
	std::ostream& os;
};

}

