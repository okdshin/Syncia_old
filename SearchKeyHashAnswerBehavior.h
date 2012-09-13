#pragma once
//SearchKeyHashAnswerBehavior:20120827
#include <iostream>
#include <algorithm>
#include <cassert>
#include <boost/algorithm/string.hpp>
#include "neuria/Neuria.h"
#include "command/Command.h"

namespace sy
{

class SearchKeyHashAnswerBehavior : 
		public boost::enable_shared_from_this<SearchKeyHashAnswerBehavior> {
public:
	using Pointer = boost::shared_ptr<SearchKeyHashAnswerBehavior>;

	static auto Create(boost::asio::io_service& service, const nr::NodeId& node_id, 
			int buffer_size, std::ostream& os) -> Pointer {
		return Pointer(new SearchKeyHashAnswerBehavior(service, node_id, os));
	}

	auto Bind(nr::ntw::BehaviorDispatcher::Pointer dispatcher) -> void {
		dispatcher->RegisterFunc(
			nr::ntw::DispatchCommand::CommandId("search_key_hash_answer"),
			boost::bind(&SearchKeyHashAnswerBehavior::OnReceiveSearchKeyHashAnswer, 
				this->shared_from_this(), _1, _2));
	}

	auto Bind(nr::ntw::Client::Pointer client) -> void {
		this->client = client;	
	}

private:
	SearchKeyHashAnswerBehavior(boost::asio::io_service& service, 
		const nr::NodeId& node_id, std::ostream& os) 
		: service(service), node_id(node_id), os(os){}
	
	auto OnReceiveSearchKeyHashAnswer(nr::ntw::Session::Pointer session, 
			const nr::ByteArray& byte_array) -> void {
		this->os << "on receive search key hash answer." << std::endl;
		auto command = cmd::SearchKeyHashAnswerCommand::Parse(byte_array);	
		std::cout << "DEBUG:" << command << std::endl;
		AnswerSearchKeyHash(command);
	}

	auto AnswerSearchKeyHash(nr::ntw::Session::Pointer session, 
			const nr::ByteArray& byte_array) -> void {
		AnswerSearchKeyHash(cmd::SearchKeyHashAnswerCommand::Parse(byte_array));
	}

public:
	auto AnswerSearchKeyHash(const cmd::SearchKeyHashAnswerCommand& command) -> void {
		this->os << "DEBUG(parsed byte_array):" << command << std::endl;
		const auto route_node_id_list = command.GetRouteNodeIdList();
		auto self_iter = std::find(
			route_node_id_list.begin(), route_node_id_list.end(), this->node_id);
		
		if(self_iter == route_node_id_list.begin()){
			this->os << "query was answered!" << std::endl;		
			return;
		}
		else{
			this->os << "resend answer" << std::endl;
			nr::ntw::Send(this->client, *(self_iter-1), 
				nr::ntw::DispatchCommand(
					cmd::GetCommandId<cmd::SearchKeyHashAnswerCommand>(), 
					command.Serialize()).Serialize());
		}
	}

private:
	boost::asio::io_service& service;
	nr::NodeId node_id;
	nr::ntw::Client::Pointer client;
	std::ostream& os;
};

}

