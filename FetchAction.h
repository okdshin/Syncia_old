#pragma once
//FetchAction:20120924
#include <iostream>
#include <string>
#include <algorithm>
#include <cassert>
#include "neuria/Neuria.h"
#include "command/Command.h"
#include "CommandToRandomNodeAction.h"

namespace syncia
{

class FetchAction{
public:
	using Pointer = boost::shared_ptr<FetchAction>;

	static auto Create(const command::CommandId& command_id, 
			neuria::network::SessionPool::Pointer to_session_pool, const neuria::network::NodeId& node_id, 
			std::ostream& os) -> Pointer { 
		auto command_to_random_node_action = 
			CommandToRandomNodeAction::Create(to_session_pool, os);
		return Pointer(new FetchAction(command_to_random_node_action, command_id, 
			node_id, os));	
	}

	auto Fetch(const neuria::ByteArray& wrapped_byte_array) -> void {
		auto initial_route = command::FetchCommand::Route();
		initial_route.push_back(this->node_id);
		this->command_to_random_node_action->CommandToRandomNode(
			this->command_id,
			command::FetchCommand(command::QUERY, initial_route, wrapped_byte_array
			).Serialize()
		);
	}


private:
    FetchAction(CommandToRandomNodeAction::Pointer command_to_random_node_action,
		const command::CommandId& command_id, const neuria::network::NodeId& node_id,
		std::ostream& os) 
			: command_to_random_node_action(command_to_random_node_action),
			command_id(command_id), node_id(node_id), os(os){}
	
	CommandToRandomNodeAction::Pointer command_to_random_node_action;
	command::CommandId command_id;
	neuria::network::NodeId node_id;
	std::ostream& os;

};

}

