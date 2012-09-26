#pragma once
//SearchKeyHashBehavior:20120923
#include <iostream>
#include <string>
#include <algorithm>
#include <cassert>
#include <boost/bind.hpp>
#include "neuria/Neuria.h"
#include "command/Command.h"
#include "FetchBehavior.h"

namespace sy
{
class SearchKeyHashBehavior : 
		public boost::enable_shared_from_this<SearchKeyHashBehavior> {
public:
	using Pointer = boost::shared_ptr<SearchKeyHashBehavior>;

	static auto Create(const nr::NodeId& node_id, 
			unsigned int max_key_hash_count, unsigned int max_hop_count, 
			nr::ntw::SessionPool::Pointer to_session_pool,
			nr::db::FileKeyHashDb::Pointer file_db, std::ostream& os) -> Pointer {
		
		auto fetch_behavior = FetchBehavior::Create(
			cmd::GetCommandId<cmd::SearchKeyHashCommand>(), 
			node_id, to_session_pool, os);
		auto search_key_hash_behavior = Pointer(
			new SearchKeyHashBehavior(fetch_behavior, max_key_hash_count, max_hop_count, file_db, os));
		
		search_key_hash_behavior->BindToFetchBehavior(fetch_behavior);
		return search_key_hash_behavior;
	}

	auto Bind(BehaviorDispatcher::Pointer dispatcher) -> void {
		this->fetch_behavior->Bind(dispatcher);
	}

private:
    SearchKeyHashBehavior(FetchBehavior::Pointer fetch_behavior, 
		unsigned int max_key_hash_count, unsigned int max_hop_count, 
		nr::db::FileKeyHashDb::Pointer file_db, std::ostream& os) 
			: fetch_behavior(fetch_behavior), max_key_hash_count(max_key_hash_count), 
			max_hop_count(max_hop_count), file_db(file_db), os(os){}

	auto BindToFetchBehavior(FetchBehavior::Pointer fetch_behavior) -> void {
		fetch_behavior->SetIsTurningPointDecider(boost::bind(
			&SearchKeyHashBehavior::DecideIsTurningPoint, this->shared_from_this(), 
			_1, _2));	

		fetch_behavior->SetFetchQueryRedirector(boost::bind(
			&SearchKeyHashBehavior::RedirectFetchQuery, this->shared_from_this(), _1));
		
		fetch_behavior->SetFetchAnswerRedirector(boost::bind(
			&SearchKeyHashBehavior::RedirectFetchAnswer, this->shared_from_this(), _1));
		
		fetch_behavior->SetOnReceiveFetchAnswerFunc(boost::bind(
			&SearchKeyHashBehavior::OnReceiveFetchAnswer, this->shared_from_this(), 
			_1, _2));	
	}

	auto DecideIsTurningPoint(const cmd::FetchCommand::Route& route, 
			const nr::ByteArray& byte_array) -> cmd::IsAnswer {
		auto command = cmd::SearchKeyHashCommand::Parse(byte_array);
		return cmd::IsAnswer(
			command.GetFoundKeyHashList().size() > this->max_key_hash_count
				|| route.size() > this->max_hop_count);
	};
	
	auto RedirectFetchQuery(const nr::ByteArray& byte_array) -> nr::ByteArray {
		auto command = cmd::SearchKeyHashCommand::Parse(byte_array);
		command.AddFoundKeyHashList(
			this->file_db->Search(command.GetSearchKeywardList()));
		return command.Serialize();
	}

	auto RedirectFetchAnswer(const nr::ByteArray& byte_array) -> nr::ByteArray {
		auto command = cmd::SearchKeyHashCommand::Parse(byte_array);
		this->file_db->Add(command.GetFoundKeyHashList());
		return command.Serialize();
	}

	auto OnReceiveFetchAnswer(nr::ntw::Session::Pointer session, 
			const nr::ByteArray& byte_array) -> void {
		auto command = cmd::SearchKeyHashCommand::Parse(byte_array);
		this->file_db->Add(command.GetFoundKeyHashList());
		std::cout << "got answer" << std::endl;
	}

	FetchBehavior::Pointer fetch_behavior;
	unsigned int max_key_hash_count;
	unsigned int max_hop_count;
	nr::db::FileKeyHashDb::Pointer file_db;
	std::ostream& os;
};

}

