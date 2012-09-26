#pragma once
//SearchKeyHashAction:20120924
#include <iostream>
#include <string>
#include <algorithm>
#include <cassert>
#include <boost/bind.hpp>
#include "neuria/Neuria.h"
#include "command/Command.h"
#include "FetchAction.h"

namespace sy
{

class SearchKeyHashAction{
public:
	using Pointer = boost::shared_ptr<SearchKeyHashAction>;

	static auto Create(nr::ntw::SessionPool::Pointer to_session_pool, 
			const nr::NodeId& node_id, std::ostream& os) -> Pointer { 
		auto fetch_action = 
			FetchAction::Create(cmd::GetCommandId<cmd::SearchKeyHashCommand>(), 
				to_session_pool, node_id, os);
		return Pointer(new SearchKeyHashAction(fetch_action, os));
	}

	auto SearchKeyHash(
			const nr::db::KeywardList& search_keyward_list) -> void {
		assert(!search_keyward_list().empty());
		this->os << "query search key hash" << std::endl;
		this->fetch_action->Fetch(
			cmd::SearchKeyHashCommand(search_keyward_list).Serialize());	
	}

private:
    SearchKeyHashAction(FetchAction::Pointer fetch_action, std::ostream& os) 
		: fetch_action(fetch_action), os(os){}

	FetchAction::Pointer fetch_action;
	std::ostream& os;
};

}

