#pragma once
//Command:20120912
#include <iostream>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include "neuria/Neuria.h"

namespace sy{
namespace cmd{
using StrList = std::vector<std::string>;

class QuerySearchKeyHashCommand {
public:
	QuerySearchKeyHashCommand(){}

	static auto Parse(const nr::ByteArray& byte_array) -> QuerySearchKeyHashCommand {
		std::stringstream ss(nr::utl::ByteArray2String(byte_array));
		boost::archive::text_iarchive ia(ss);
		auto command = QuerySearchKeyHashCommand();
		ia >> command;
		return command;
	}
	
	auto GetSearchKeywardList() -> StrList { return this->search_keyward_list; }
	auto AddToRoute(const nr::NodeId& node_id, const StrList& keyward_list) -> void { 
		this->route.push_back(std::make_tuple(node_id, keyward_list)); }
	auto GetHopCount() -> int { return this->route.size();	}

	auto Serialize() -> nr::ByteArray {
		std::stringstream ss;
		boost::archive::text_oarchive oa(ss);
		oa << static_cast<const QuerySearchKeyHashCommand&>(*this);
		return nr::utl::String2ByteArray(ss.str());
	}

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize( Archive& ar, unsigned int ver ){
		ar & search_keyward_list & route;
	}

	StrList search_keyward_list;
	std::vector<std::tuple<nr::NodeId, StrList>> route;
};

}
}

