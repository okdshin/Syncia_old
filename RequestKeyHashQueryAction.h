#pragma once
//RequestKeyHashQueryAction:20120827
#include <iostream>
#include <algorithm>
#include <cassert>
#include <boost/algorithm/string.hpp>
#include "neuria/Neuria.h"
#include "command/Command.h"

namespace sy
{

class RequestKeyHashQueryAction : 
		public boost::enable_shared_from_this<RequestKeyHashQueryAction> {
public:
	using Pointer = boost::shared_ptr<RequestKeyHashQueryAction>;

	static auto Create(boost::asio::io_service& service, const nr::NodeId& node_id, 
			int buffer_size, nr::ntw::SessionPool::Pointer connected_pool, 
			std::ostream& os) -> Pointer {
		return Pointer(
			new RequestKeyHashQueryAction(service, node_id, connected_pool, os));
	}
	
	auto Bind(nr::ntw::Client::Pointer client) -> void {
		this->client = client;
	}

	auto ConnectRequestLink(const nr::NodeId& node_id) -> void {
		nr::ntw::Connect(this->client, node_id, this->connected_pool,
			[this](nr::ntw::Session::Pointer, const nr::ByteArray&){
				this->os << "on receive from connected node." << std::endl;
			});
	}

	auto QueryRequestAtRandom(const nr::ntw::DispatchCommad::CommandId& command_id, 
			const nr::ByteArray& serialized_command_byte_array) -> void {	
		this->at_random_selector(*connected_pool)->Send(nr::ntw::DispatchCommand(
			command_id, serialized_command_byte_array).Serialize());
	}

private:
	RequestKeyHashQueryAction(boost::asio::io_service& service, 
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

