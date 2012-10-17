#pragma once
//LinkForFetchKeyHashCommand:20120912
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
#include "Common.h"

namespace syncia{
namespace command{

class LinkForFetchKeyHashCommand {
public:
	LinkForFetchKeyHashCommand(){}
	
	static auto Parse(const neuria::ByteArray& byte_array) -> LinkForFetchKeyHashCommand {
		std::stringstream ss(neuria::utility::ByteArray2String(byte_array));
		boost::archive::text_iarchive ia(ss);
		auto command = LinkForFetchKeyHashCommand();
		ia >> command;
		return command;
	}
	
	auto Serialize() const -> neuria::ByteArray {
		std::stringstream ss;
		boost::archive::text_oarchive oa(ss);
		oa << static_cast<const LinkForFetchKeyHashCommand&>(*this);
		return neuria::utility::String2ByteArray(ss.str());
	}

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, unsigned int ver){}

};

auto operator<<(std::ostream& os, const LinkForFetchKeyHashCommand&) -> std::ostream& {
	os << "LinkForFetchKeyHashCommand";
	return os;
}

template<>
auto GetCommandId<LinkForFetchKeyHashCommand>() -> CommandId {
	return CommandId("link_for_fetch_key_hash_command");
}

}
}

