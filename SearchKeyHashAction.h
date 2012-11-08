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

namespace syncia
{

class SearchKeyHashAction{
public:
	using Pointer = boost::shared_ptr<SearchKeyHashAction>;

	static auto Create(neuria::network::SessionPool::Pointer to_session_pool, 
			const neuria::network::NodeId& node_id, std::ostream& os) -> Pointer { 
		auto fetch_action = 
			FetchAction::Create(command::GetCommandId<command::SearchKeyHashCommand>(), 
				to_session_pool, node_id, os);
		return Pointer(new SearchKeyHashAction(fetch_action, os));
	}

	auto SearchKeyHash(database::KeywordList search_keyword_list) -> void {
		if(search_keyword_list().empty()){
			auto wrapped_keyword_list = search_keyword_list();
			wrapped_keyword_list.push_back("");
			search_keyword_list = database::KeywordList(wrapped_keyword_list);	
		}
		this->os << "query search key hash" << std::endl;
		this->fetch_action->Fetch(
			command::SearchKeyHashCommand(search_keyword_list).Serialize());	
	}

private:
    SearchKeyHashAction(FetchAction::Pointer fetch_action, std::ostream& os) 
		: fetch_action(fetch_action), os(os){}

	FetchAction::Pointer fetch_action;
	std::ostream& os;
};

}

