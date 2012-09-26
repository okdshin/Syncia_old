#pragma once
//CommandToRandomNodeAction:20120827
#include <iostream>
#include <algorithm>
#include <cassert>
#include <boost/algorithm/string.hpp>
#include "neuria/Neuria.h"
#include "command/Command.h"

namespace sy
{

class CommandToRandomNodeAction : 
		public boost::enable_shared_from_this<CommandToRandomNodeAction> {
public:
	using Pointer = boost::shared_ptr<CommandToRandomNodeAction>;

	static auto Create(
			nr::ntw::SessionPool::Pointer to_session_pool, std::ostream& os) -> Pointer {
		return Pointer(new CommandToRandomNodeAction(to_session_pool, os));
	}
	
	auto Bind(nr::ntw::Client::Pointer client) -> void {
		this->client = client;
	}

	auto CommandToRandomNode(const cmd::CommandId& command_id, 
			const nr::ByteArray& serialized_command_byte_array) -> void {
		assert(to_session_pool->GetSize() > 0);
		this->at_random_selector(*(to_session_pool))->Send(
			cmd::DispatchCommand(
				command_id, serialized_command_byte_array).Serialize(),
			[](nr::ntw::Session::Pointer){});
	}

private:
	CommandToRandomNodeAction(nr::ntw::SessionPool::Pointer to_session_pool, 
		std::ostream& os) 
		: to_session_pool(to_session_pool), os(os){}
	
	nr::ntw::Client::Pointer client;
	nr::utl::RandomElementSelector at_random_selector;
	nr::ntw::SessionPool::Pointer to_session_pool;
	std::ostream& os;
	
};

template<class Command>
auto CommandToRandomNode(
		CommandToRandomNodeAction::Pointer action, const Command& command) -> void{
	action->CommandToRandomNode(cmd::GetCommandId<Command>(), command.Serialize());
}

}

