#pragma once
//CommandToRandomNodeAction:20120827
#include <iostream>
#include <algorithm>
#include <cassert>
#include <boost/algorithm/string.hpp>
#include "neuria/Neuria.h"
#include "command/Command.h"

namespace syncia
{

class CommandToRandomNodeAction : 
		public boost::enable_shared_from_this<CommandToRandomNodeAction> {
public:
	using Pointer = boost::shared_ptr<CommandToRandomNodeAction>;

	static auto Create(
			neuria::network::SessionPool::Pointer to_session_pool, std::ostream& os) -> Pointer {
		return Pointer(new CommandToRandomNodeAction(to_session_pool, os));
	}
	
	auto Bind(neuria::network::Client::Pointer client) -> void {
		this->client = client;
	}

	auto CommandToRandomNode(const command::CommandId& command_id, 
			const neuria::ByteArray& serialized_command_byte_array) -> void {
		//assert(to_session_pool->GetSize() > 0);
		if(to_session_pool->GetSize() == 0){
			os << "no link." << std::endl;
			return;	
		}
		this->at_random_selector(*(to_session_pool))->Send(
			command::DispatchCommand(
				command_id, serialized_command_byte_array).Serialize(),
			[](neuria::network::Session::Pointer){});
	}

private:
	CommandToRandomNodeAction(neuria::network::SessionPool::Pointer to_session_pool, 
		std::ostream& os) 
		: to_session_pool(to_session_pool), os(os){}
	
	neuria::network::Client::Pointer client;
	neuria::utility::RandomElementSelector at_random_selector;
	neuria::network::SessionPool::Pointer to_session_pool;
	std::ostream& os;
	
};

template<class Command>
auto CommandToRandomNode(
		CommandToRandomNodeAction::Pointer action, const Command& command) -> void{
	action->CommandToRandomNode(command::GetCommandId<Command>(), command.Serialize());
}

}

