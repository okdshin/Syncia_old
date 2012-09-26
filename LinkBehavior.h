#pragma once
//LinkBehavior:20120922
#include <iostream>
#include "neuria/Neuria.h"
#include "command/Command.h"
#include "BehaviorDispatcher.h"

namespace sy
{

class LinkBehavior :
		public boost::enable_shared_from_this<LinkBehavior> {
public:
	using Pointer = boost::shared_ptr<LinkBehavior>;
	
	static auto Create(const nr::ntw::SessionPool::Pointer& pool, 
			const cmd::CommandId& link_command_id,
			std::ostream& os) -> Pointer {
		return Pointer(new LinkBehavior(pool, link_command_id, os));
	}

	auto SetOnReceiveLinkQueryFunc(nr::ntw::Session::OnReceiveFunc func) -> void {
		this->on_receive_link_query_func = func;
	}

	auto Bind(BehaviorDispatcher::Pointer dispatcher) -> void {
		dispatcher->RegisterFunc(link_command_id,
			boost::bind(&LinkBehavior::OnReceiveLinkQuery, 
				this->shared_from_this(), _1, _2));
	}

	auto Bind(nr::ntw::Client::Pointer client) -> void {
		this->client = client;	
	}

private:
    LinkBehavior(nr::ntw::SessionPool::Pointer pool, 
			const cmd::CommandId& link_command_id, 
			std::ostream& os) 
		: pool(pool), link_command_id(link_command_id), os(os){}
	
	auto OnReceiveLinkQuery(nr::ntw::Session::Pointer session, 
			const nr::ByteArray& byte_array) -> void {
		this->os << "on receive link query." << std::endl;
		auto command = cmd::LinkCommand::Parse(byte_array);
		std::cout << "LowerNodeId:" << command.GetNodeId() << std::endl;
		nr::ntw::Connect(this->client, command.GetNodeId(), this->pool, 
			[](nr::ntw::Session::Pointer, const nr::ByteArray&){});
		this->on_receive_link_query_func(session, command.GetWrappedByteArray());
	}

	nr::ntw::Session::OnReceiveFunc on_receive_link_query_func;
	nr::ntw::SessionPool::Pointer pool;
	nr::ntw::Client::Pointer client;
	cmd::CommandId link_command_id;
	std::ostream& os;
};

}

