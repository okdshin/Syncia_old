#pragma once
//LinkBehavior:20120922
#include <iostream>
#include "neuria/Neuria.h"
#include "command/Command.h"
#include "TypeWrapper.h"

namespace sy
{
class LinkBehavior :
		public boost::enable_shared_from_this<LinkBehavior> {
public:
	using Pointer = boost::shared_ptr<LinkBehavior>;
	using OnReceiveLinkQueryFunc = boost::function<
		void (nr::ntw::Session::Pointer, const nr::ByteArray&)>;
	
	static auto Create(const AcceptedPool& accepted_pool, 
			const nr::ntw::DispatchCommand::CommandId& link_command_id,
			std::ostream& os) -> Pointer {
		return Pointer(new LinkBehavior(accepted_pool, link_command_id, os));
	}

	auto SetOnReceiveLinkQueryFunc(OnReceiveLinkQueryFunc func) -> void {
		this->on_receive_link_query_func = func;
	}

	auto Bind(nr::ntw::BehaviorDispatcher::Pointer dispatcher) -> void {
		dispatcher->RegisterFunc(link_command_id,
			boost::bind(&LinkBehavior::OnReceiveLinkQuery, 
				this->shared_from_this(), _1, _2));
	}

private:
    LinkBehavior(const AcceptedPool& accepted_pool, 
			const nr::ntw::DispatchCommand::CommandId& link_command_id, 
			std::ostream& os) 
		: accepted_pool(accepted_pool), link_command_id(link_command_id), os(os){}
	
	auto OnReceiveLinkQuery(nr::ntw::Session::Pointer session, 
			const nr::ByteArray& byte_array) -> void {
		this->os << "on receive link query." << std::endl;
		this->accepted_pool()->Add(session);
		this->on_receive_link_query_func(session, byte_array);
	}

	OnReceiveLinkQueryFunc on_receive_link_query_func;
	AcceptedPool accepted_pool;
	nr::ntw::DispatchCommand::CommandId link_command_id;
	std::ostream& os;
};
}

