#pragma once
//SearchKeyHashQueryBehavior:20120827
#include <iostream>
#include <algorithm>
#include <cassert>
#include <boost/algorithm/string.hpp>
#include "neuria/Neuria.h"
#include "command/Command.h"

namespace sy
{

class SearchKeyHashQueryBehavior : 
		public boost::enable_shared_from_this<SearchKeyHashQueryBehavior> {
public:
	using Pointer = boost::shared_ptr<SearchKeyHashQueryBehavior>;
	using Answerer = boost::function<void (const cmd::SearchKeyHashQueryCommand&)>; 

	static auto Create(boost::asio::io_service& service, const nr::NodeId& node_id, 
			int max_hop_count, nr::ntw::SessionPool::Pointer connected_pool, 
			std::ostream& os) -> Pointer {
		return Pointer(
			new SearchKeyHashQueryBehavior(
				service, node_id, max_hop_count, connected_pool, os));
	}
	
	auto Bind(nr::ntw::BehaviorDispatcher::Pointer dispatcher) -> void {
		dispatcher->RegisterFunc(cmd::GetCommandId<cmd::SearchKeyHashQueryCommand>(),
			boost::bind(&SearchKeyHashQueryBehavior::OnReceiveSearchKeyHashQuery, 
				this->shared_from_this(), _1, _2));
	}

	auto SetSearchKeyHashAnswerer(Answerer answerer) -> void {
		this->answerer = answerer;
	}
	
private:
	SearchKeyHashQueryBehavior(boost::asio::io_service& service, 
		const nr::NodeId& node_id, int max_hop_count, 
		nr::ntw::SessionPool::Pointer connected_pool, std::ostream& os) 
		: service(service), node_id(node_id), max_hop_count(max_hop_count),
		connected_pool(connected_pool), os(os){}
	
	auto OnReceiveSearchKeyHashQuery(nr::ntw::Session::Pointer session, 
			const nr::ByteArray& byte_array) -> void {
		this->os << "on receive search key hash query." << std::endl;
		auto command = cmd::SearchKeyHashQueryCommand::Parse(byte_array);
		command.AddFindKeyHash(nr::db::CreateTestKeyHash());//to do
		command.AddRouteNodeId(this->node_id);
		if(command.GetHopCount() <= this->max_hop_count){
			this->os << "resend query." << std::endl;
			assert(this->connected_pool->GetSize() > 0);
			this->at_random_selector(*this->connected_pool)->Send(
				nr::ntw::DispatchCommand(
					cmd::GetCommandId<cmd::SearchKeyHashQueryCommand>(), 
					command.Serialize()).Serialize());
		}
		else{
			this->os << "return back query as answer." << std::endl;//to do
			this->answerer(command);
		}
	}

	boost::asio::io_service& service;
	nr::NodeId node_id;
	int max_hop_count;
	nr::ntw::SessionPool::Pointer connected_pool;
	std::ostream& os;
	nr::utl::RandomElementSelector at_random_selector;
	Answerer answerer;
};

}

