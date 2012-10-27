#pragma once
//SpreadKeyHashBehavior:20120923
#include <iostream>
#include <string>
#include <algorithm>
#include <cassert>
#include <boost/bind.hpp>
#include "neuria/Neuria.h"
#include "command/Command.h"
#include "FetchBehavior.h"

namespace syncia
{
class SpreadKeyHashBehavior : 
		public boost::enable_shared_from_this<SpreadKeyHashBehavior> {
public:
	using Pointer = boost::shared_ptr<SpreadKeyHashBehavior>;

	static auto Create(const neuria::network::NodeId& node_id, 
			unsigned int spread_key_hash_max_count, unsigned int max_hop_count, 
			neuria::network::SessionPool::Pointer to_session_pool,
			database::FileKeyHashDb::Pointer file_db, std::ostream& os) -> Pointer {
		
		auto fetch_behavior = FetchBehavior::Create(
			command::GetCommandId<command::SpreadKeyHashCommand>(), 
			node_id, to_session_pool, os);
		auto search_key_hash_behavior = Pointer(
			new SpreadKeyHashBehavior(fetch_behavior, spread_key_hash_max_count, 
				max_hop_count, file_db, os));
		
		search_key_hash_behavior->BindToFetchBehavior(fetch_behavior);
		return search_key_hash_behavior;
	}

	auto Bind(BehaviorDispatcher::Pointer dispatcher) -> void {
		this->fetch_behavior->Bind(dispatcher);
	}
	
	auto Bind(neuria::network::Client::Pointer client) -> void {
		this->fetch_behavior->Bind(client);
	}

private:
    SpreadKeyHashBehavior(FetchBehavior::Pointer fetch_behavior, 
		unsigned int spread_key_hash_max_count, unsigned int max_hop_count, 
		database::FileKeyHashDb::Pointer file_db, std::ostream& os) 
			: fetch_behavior(fetch_behavior), 
			spread_key_hash_max_count(spread_key_hash_max_count), 
			max_hop_count(max_hop_count), file_db(file_db), os(os){}

	auto BindToFetchBehavior(FetchBehavior::Pointer fetch_behavior) -> void {
		fetch_behavior->SetIsTurningPointDecider(boost::bind(
			&SpreadKeyHashBehavior::DecideIsTurningPoint, this->shared_from_this(), 
			_1, _2));	

		fetch_behavior->SetFetchQueryRedirector(boost::bind(
			&SpreadKeyHashBehavior::RedirectFetchQuery, this->shared_from_this(), _1));
		
		fetch_behavior->SetFetchAnswerRedirector(boost::bind(
			&SpreadKeyHashBehavior::RedirectFetchAnswer, this->shared_from_this(), _1));
		
		fetch_behavior->SetOnReceiveFetchAnswerFunc(boost::bind(
			&SpreadKeyHashBehavior::OnReceiveFetchAnswer, this->shared_from_this(), 
			_1, _2));	
	}

	auto DecideIsTurningPoint(const command::FetchCommand::Route& route, 
			const neuria::ByteArray& byte_array) -> command::IsAnswer {
		this->os << "decide is turning point" << std::endl;
		auto command = command::SpreadKeyHashCommand::Parse(byte_array);
		return command::IsAnswer(route.size() > this->max_hop_count);
	};
	
	auto RedirectFetchQuery(const neuria::ByteArray& byte_array) -> neuria::ByteArray {
		auto command = command::SpreadKeyHashCommand::Parse(byte_array);
		command.AddSpreadKeyHashList(
			this->file_db->GetNewer(this->spread_key_hash_max_count));
		return command.Serialize();
	}

	auto RedirectFetchAnswer(const neuria::ByteArray& byte_array) -> neuria::ByteArray {
		auto command = command::SpreadKeyHashCommand::Parse(byte_array);
		this->file_db->Add(command.GetSpreadKeyHashList());
		return command.Serialize();
	}

	auto OnReceiveFetchAnswer(neuria::network::Session::Pointer session, 
			const neuria::ByteArray& byte_array) -> void {
		auto command = command::SpreadKeyHashCommand::Parse(byte_array);
		this->file_db->Add(command.GetSpreadKeyHashList());
		std::cout << "got answer" << std::endl;
	}

	FetchBehavior::Pointer fetch_behavior;
	unsigned int spread_key_hash_max_count;
	unsigned int max_hop_count;
	database::FileKeyHashDb::Pointer file_db;
	std::ostream& os;
};

}

