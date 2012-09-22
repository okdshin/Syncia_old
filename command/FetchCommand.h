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
#include "../neuria/database/DataBase.h"
#include "Common.h"
#include "SearchKeyHashCommandData.h"
#include "../TypeWrapper.h"

namespace sy{
namespace cmd{

class FetchCommand {
public:
	using Route = std::vector<nr::NodeId>;

	FetchCommand(){}
	FetchCommand(const IsAnswer& is_answer, const nr::ByteArray& wrapped_byte_array) 
		: is_answer(is_answer()), byte_array(wrapped_byte_array){}

	static auto Parse(const nr::ByteArray& byte_array) -> FetchCommand {
		std::stringstream ss(nr::utl::ByteArray2String(byte_array));
		boost::archive::text_iarchive ia(ss);
		auto command = FetchCommand();
		ia >> command;
		return command;
	}
	
	auto Serialize() const -> nr::ByteArray {
		std::stringstream ss;
		boost::archive::text_oarchive oa(ss);
		oa << static_cast<const FetchCommand&>(*this);
		return nr::utl::String2ByteArray(ss.str());
	}
	
	auto IsAnswer() -> bool { return is_answer; }
	auto AddRoute(const nr::NodeId& node_id) -> void {
		this->route_node_id_list.push_back(node_id);
	}
	auto GetRoute()const -> Route { return this->route_node_id_list; }

	auto GetWrappedByteArray() -> nr::ByteArray { return byte_array; }

	auto IsReturnBackToStart(const nr::NodeId& node_id) -> bool {
		return route_node_id_list.front() == node_id;	
	}

	auto GetOneStepCloserNodeId(const nr::NodeId& node_id) -> nr::NodeId {
		auto self_iter = std::find(this->route_node_id_list.begin(), 
			this->route_node_id_list.end(), node_id);
		assert("call IsReturnBackStart and check before." 
			&& self_iter != this->route_node_id_list.end());
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
	nr::ByteArray byte_array;
};

auto operator<<(std::ostream& os, 
		const FetchCommand& command) -> std::ostream& {
	os << command.is_answer;
	return os;
}

template<>
auto GetCommandId<FetchCommand>() 
		-> DispatchCommand::CommandId {
	return DispatchCommand::CommandId("fetch_command");
}

}
}

