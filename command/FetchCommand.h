#pragma once
//FetchCommand:20120912
#include <iostream>
#include <array>
#include <algorithm>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/string.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include "../neuria/Neuria.h"
#include "../database/DataBase.h"
#include "Common.h"

namespace syncia{
namespace command{

class IsAnswerType{};
using IsAnswer = neuria::utility::TypeWrapper<bool, IsAnswerType>;

const auto QUERY = IsAnswer(false);
const auto ANSWER = IsAnswer(true);

class FetchCommand {
public:
	using Route = std::vector<neuria::network::NodeId>;

	FetchCommand(){}
	FetchCommand(const IsAnswer& is_answer, const Route& route, const neuria::ByteArray& wrapped_byte_array) 
		: is_answer(is_answer()), route_node_id_list(route), 
		byte_array(wrapped_byte_array){}

	static auto Parse(const neuria::ByteArray& byte_array) -> FetchCommand {
		std::stringstream ss(neuria::utility::ByteArray2String(byte_array));
		boost::archive::text_iarchive ia(ss);
		auto command = FetchCommand();
		ia >> command;
		return command;
	}
	
	auto Serialize() const -> neuria::ByteArray {
		std::stringstream ss;
		boost::archive::text_oarchive oa(ss);
		oa << static_cast<const FetchCommand&>(*this);
		return neuria::utility::String2ByteArray(ss.str());
	}
	
	auto IsAnswer() -> bool { return is_answer; }
	auto AddRoute(const neuria::network::NodeId& node_id) -> void {
		this->route_node_id_list.push_back(node_id);
	}
	auto GetRoute()const -> Route { return route_node_id_list; }

	auto GetWrappedByteArray() -> neuria::ByteArray { return byte_array; }

	auto IsReturnBackToStart(const neuria::network::NodeId& node_id) -> bool {
		return this->is_answer && route_node_id_list.front() == node_id;	
	}

	auto GetOneStepCloserNodeId(const neuria::network::NodeId& node_id) -> neuria::network::NodeId {
		auto self_iter = std::find(this->route_node_id_list.begin(), 
			this->route_node_id_list.end(), node_id);
		assert("not found self node id in route." 
			&& self_iter != this->route_node_id_list.end());
		assert("call IsReturnBackToStart and check before." 
			&& self_iter != this->route_node_id_list.begin());
		return *(self_iter-1);
	}

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, unsigned int ver){
		ar & is_answer & route_node_id_list & byte_array;
	}

	friend
	auto operator<<(std::ostream& os, 
		const FetchCommand& command) -> std::ostream&;

	bool is_answer;
	Route route_node_id_list;
	neuria::ByteArray byte_array;
};

auto operator<<(std::ostream& os, 
		const FetchCommand& command) -> std::ostream& {
	os << "is answer:" << command.is_answer;
	std::copy(command.route_node_id_list.begin(), 
		command.route_node_id_list.end(), std::ostream_iterator<neuria::network::NodeId>(os, " "));
	os << "\n";
	std::copy(command.byte_array.begin(), 
		command.byte_array.end(), std::ostream_iterator<char>(os, ""));
	return os;
}

}
}

