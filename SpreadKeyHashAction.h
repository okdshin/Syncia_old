#pragma once
//SpreadKeyHashAction:20120924
#include <iostream>
#include <string>
#include <algorithm>
#include <cassert>
#include <boost/bind.hpp>
#include "neuria/Neuria.h"
#include "command/Command.h"
#include "FetchAction.h"

namespace syncia
{

class SpreadKeyHashAction{
public:
	using Pointer = boost::shared_ptr<SpreadKeyHashAction>;

	static auto Create(neuria::network::SessionPool::Pointer to_session_pool, 
			const neuria::network::NodeId& node_id, std::ostream& os) -> Pointer { 
		auto fetch_action = 
			FetchAction::Create(
				command::GetCommandId<command::SpreadKeyHashCommand>(), 
				to_session_pool, node_id, os);
		return Pointer(new SpreadKeyHashAction(fetch_action, os));
	}

	auto RequestSpreadKeyHash() -> void {
		this->os << "request spread key hash" << std::endl;
		this->fetch_action->Fetch(command::SpreadKeyHashCommand().Serialize());	
	}

private:
    SpreadKeyHashAction(FetchAction::Pointer fetch_action, std::ostream& os) 
		: fetch_action(fetch_action), os(os){}

	FetchAction::Pointer fetch_action;
	std::ostream& os;
};

}

