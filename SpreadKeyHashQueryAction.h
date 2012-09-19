#pragma once
//SpreadKeyHashQueryAction:20120827
#include <iostream>
#include <algorithm>
#include <cassert>
#include <boost/algorithm/string.hpp>
#include "neuria/Neuria.h"
#include "command/Command.h"

namespace sy
{

class SpreadKeyHashQueryAction : 
		public boost::enable_shared_from_this<SpreadKeyHashQueryAction> {
public:
	using Pointer = boost::shared_ptr<SpreadKeyHashQueryAction>;

	static auto Create(boost::asio::io_service& service, const nr::NodeId& node_id, 
			int buffer_size, nr::ntw::SessionPool::Pointer connected_pool, 
			std::ostream& os) -> Pointer {
		return Pointer(
			new SpreadKeyHashQueryAction(service, node_id, connected_pool, os));
	}
	
	auto Bind(nr::ntw::Client::Pointer client) -> void {
		this->client = client;
	}

	auto ConnectSpreadLink(const nr::NodeId& node_id) -> void {
		nr::ntw::Connect(this->client, node_id, this->connected_pool,
			[this](nr::ntw::Session::Pointer, const nr::ByteArray&){
				this->os << "on receive from connected node." << std::endl;
			});
	}

	auto QuerySpreadKeyHash(
			const nr::db::FileKeyHashDb::KeywardList& keyward_list) -> void {	
		auto command = cmd::SpreadKeyHashQueryCommand(keyward_list);
		//command.AddFindKeyHashList(nr::db::CreateTestKeyHash());
		command.AddRouteNodeId(this->node_id);

		this->at_random_selector(*connected_pool)->Send(nr::ntw::DispatchCommand(
			cmd::GetCommandId<cmd::SpreadKeyHashQueryCommand>(), 
			command.Serialize()).Serialize());
	}

private:
	SpreadKeyHashQueryAction(boost::asio::io_service& service, 
		const nr::NodeId& node_id, nr::ntw::SessionPool::Pointer connected_pool, 
		std::ostream& os) 
		: service(service), node_id(node_id), connected_pool(connected_pool), os(os){}
	
	boost::asio::io_service& service;
	nr::NodeId node_id;
	nr::ntw::SessionPool::Pointer connected_pool;
	nr::ntw::Client::Pointer client;
	std::ostream& os;
	
	nr::utl::RandomElementSelector at_random_selector;
};

}

