#pragma once
//LinkCommand:20120912
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


class LinkCommand {
public:
	LinkCommand(){}
	LinkCommand(const neuria::network::NodeId& node_id, const neuria::ByteArray& wrapped_byte_array) 
		: node_id(node_id), byte_array(wrapped_byte_array){}

	static auto Parse(const neuria::ByteArray& byte_array) -> LinkCommand {
		std::stringstream ss(neuria::utility::ByteArray2String(byte_array));
		boost::archive::text_iarchive ia(ss);
		auto command = LinkCommand();
		ia >> command;
		return command;
	}
	
	auto Serialize() const -> neuria::ByteArray {
		std::stringstream ss;
		boost::archive::text_oarchive oa(ss);
		oa << static_cast<const LinkCommand&>(*this);
		return neuria::utility::String2ByteArray(ss.str());
	}
	
	auto GetNodeId()const -> neuria::network::NodeId { return node_id; }
	auto GetWrappedByteArray()const -> neuria::ByteArray { return byte_array; }

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, unsigned int ver){
		ar & node_id & byte_array;
	}

	friend
	auto operator<<(std::ostream& os, const LinkCommand& command) -> std::ostream&;

	neuria::network::NodeId node_id;
	neuria::ByteArray byte_array;
};

auto operator<<(std::ostream& os, 
		const LinkCommand& command) -> std::ostream& {
	os << "ServerNodeId:" << command.GetNodeId();
	os << " WrappedByteArray:";
	std::copy(command.byte_array.begin(), 
		command.byte_array.end(), std::ostream_iterator<char>(os, ""));
	return os;
}

}
}

