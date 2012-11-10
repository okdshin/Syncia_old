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

namespace syncia
{
class SearchKeyHashBehavior : 
		public boost::enable_shared_from_this<SearchKeyHashBehavior> {
public:
	using Pointer = boost::shared_ptr<SearchKeyHashBehavior>;
	using OnReceivedAnswerFunc = boost::function<void (const database::FileKeyHashList&)>;

	static auto Create(const neuria::network::NodeId& node_id, 
			unsigned int max_key_hash_count, unsigned int max_hop_count, 
			neuria::network::SessionPool::Pointer to_session_pool,
			database::FileKeyHashDb::Pointer file_db, 
			database::FileKeyHashDb::Pointer searched_file_db, 
			std::ostream& os) -> Pointer {
		
		auto fetch_behavior = FetchBehavior::Create(
			command::GetCommandId<command::SearchKeyHashCommand>(), 
			node_id, to_session_pool, os);
		auto search_key_hash_behavior = Pointer(
			new SearchKeyHashBehavior(
				fetch_behavior, max_key_hash_count, max_hop_count, file_db, searched_file_db, os));
		
		search_key_hash_behavior->BindToFetchBehavior(fetch_behavior);
		return search_key_hash_behavior;
	}

	auto SetOnReceivedAnswerFunc(OnReceivedAnswerFunc on_receive_answer_func) -> void {
		this->on_receive_answer_func = on_receive_answer_func;	
	}

	auto Bind(BehaviorDispatcher::Pointer dispatcher) -> void {
		this->fetch_behavior->Bind(dispatcher);
	}

	auto Bind(neuria::network::Client::Pointer client) -> void {
		this->fetch_behavior->Bind(client);	
	}

private:
    SearchKeyHashBehavior(FetchBehavior::Pointer fetch_behavior, 
		unsigned int max_key_hash_count, unsigned int max_hop_count, 
		database::FileKeyHashDb::Pointer file_db, 
		database::FileKeyHashDb::Pointer searched_file_db, 
		std::ostream& os) 
			: fetch_behavior(fetch_behavior), max_key_hash_count(max_key_hash_count), 
			max_hop_count(max_hop_count), file_db(file_db), 
			searched_file_db(searched_file_db), os(os){
				
			this->SetOnReceivedAnswerFunc(
				[this](const database::FileKeyHashList&){ 
					this->os << "on received answer!!" << std::endl; });
		}

	auto BindToFetchBehavior(FetchBehavior::Pointer fetch_behavior) -> void {
		fetch_behavior->SetIsTurningPointDecider(boost::bind(
			&SearchKeyHashBehavior::DecideIsTurningPoint, this->shared_from_this(), 
			_1, _2));	

		fetch_behavior->SetFetchQueryRedirector(boost::bind(
			&SearchKeyHashBehavior::RedirectFetchQuery, this->shared_from_this(), _1));
		
		fetch_behavior->SetFetchAnswerRedirector(boost::bind(
			&SearchKeyHashBehavior::RedirectFetchAnswer, this->shared_from_this(), _1));
		
		fetch_behavior->SetOnReceivedFetchAnswerFunc(boost::bind(
			&SearchKeyHashBehavior::OnReceivedFetchAnswer, this->shared_from_this(), 
			_1, _2));	
	}

	auto DecideIsTurningPoint(const command::FetchCommand::Route& route, 
			const neuria::ByteArray& byte_array) -> command::IsAnswer {
		auto command = command::SearchKeyHashCommand::Parse(byte_array);
		return command::IsAnswer(
			command.GetFoundKeyHashList().size() > this->max_key_hash_count
				|| route.size() > this->max_hop_count);
	};
	
	auto RedirectFetchQuery(const neuria::ByteArray& byte_array) -> neuria::ByteArray {
		auto command = command::SearchKeyHashCommand::Parse(byte_array);
		auto file_key_hash_list = this->file_db->Search(command.GetSearchKeywordList());
		std::cout << "redirect fetch query. file key hash list is " << file_key_hash_list << std::endl;
		command.AddFoundKeyHashList(file_key_hash_list);
		return command.Serialize();
	}

	auto RedirectFetchAnswer(const neuria::ByteArray& byte_array) -> neuria::ByteArray {
		auto command = command::SearchKeyHashCommand::Parse(byte_array);
		this->file_db->Add(command.GetFoundKeyHashList());
		std::cout << "redirect fetch answer : " << file_db << std::endl;
		return command.Serialize();
	}

	auto OnReceivedFetchAnswer(neuria::network::Session::Pointer session, 
			const neuria::ByteArray& byte_array) -> void {
		auto command = command::SearchKeyHashCommand::Parse(byte_array);
		this->file_db->Add(command.GetFoundKeyHashList());
		this->searched_file_db->Add(command.GetFoundKeyHashList());
		std::cout << "on receive fetch answer : " << file_db << std::endl;
		std::cout << "got answer" << std::endl;
		
		this->on_receive_answer_func(command.GetFoundKeyHashList());
	}

	FetchBehavior::Pointer fetch_behavior;
	unsigned int max_key_hash_count;
	unsigned int max_hop_count;
	database::FileKeyHashDb::Pointer file_db;
	database::FileKeyHashDb::Pointer searched_file_db;
	OnReceivedAnswerFunc on_receive_answer_func;
	std::ostream& os;
};

}

