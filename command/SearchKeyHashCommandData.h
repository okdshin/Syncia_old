#pragma once
//SearchKeyHashCommandData:20120912
#include <iostream>
#include <array>
#include <algorithm>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include "../neuria/Neuria.h"
#include "../neuria/database/DataBase.h"
#include "Common.h"

namespace sy{
namespace cmd{

using StrList = std::vector<std::string>;

class SearchKeyHashCommandData {
public:
	SearchKeyHashCommandData(){}
	SearchKeyHashCommandData(
			const nr::db::FileKeyHashDb::KeywardList& search_keyward_list)
		:search_keyward_list(search_keyward_list){}

	static auto Parse(const nr::ByteArray& byte_array) -> SearchKeyHashCommandData {
		std::stringstream ss(nr::utl::ByteArray2String(byte_array));
		boost::archive::text_iarchive ia(ss);
		auto command = SearchKeyHashCommandData();
		ia >> command;
		return command;
	}

	auto GetSearchKeywardListRef() -> nr::db::FileKeyHashDb::KeywardList& {
		return search_keyward_list;	
	}

	auto GetFindKeyHashListRef() -> std::vector<nr::db::FileKeyHash>& {
		return find_key_hash_list;	
	}

	auto GetRouteNodeIdListRef() -> std::vector<nr::NodeId>& {
		return route_node_id_list;
	}

	auto GetSearchKeywardListRef() const 
			-> const nr::db::FileKeyHashDb::KeywardList& {
		return search_keyward_list;	
	}

	auto GetFindKeyHashListRef() const -> const std::vector<nr::db::FileKeyHash>& {
		return find_key_hash_list;	
	}

	auto GetRouteNodeIdListRef() const -> const std::vector<nr::NodeId>& {
		return route_node_id_list;
	}

	auto Serialize() const -> nr::ByteArray {
		std::stringstream ss;
		boost::archive::text_oarchive oa(ss);
		oa << static_cast<const SearchKeyHashCommandData&>(*this);
		return nr::utl::String2ByteArray(ss.str());
	}

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, unsigned int ver){
		ar & search_keyward_list & find_key_hash_list & route_node_id_list;
	}
	
	nr::db::FileKeyHashDb::KeywardList search_keyward_list;
	std::vector<nr::db::FileKeyHash> find_key_hash_list;
	std::vector<nr::NodeId> route_node_id_list;
};

auto operator<<(std::ostream& os, 
		const SearchKeyHashCommandData& command) -> std::ostream& {
	auto keyward_list = command.GetSearchKeywardListRef();
	std::copy(keyward_list.begin(), keyward_list.end(), 
		std::ostream_iterator<nr::db::FileKeyHash::Keyward>(os, ", "));
	os << "\n";
	auto find_key_hash_list = command.GetFindKeyHashListRef();
	std::copy(find_key_hash_list.begin(), find_key_hash_list.end(), 
		std::ostream_iterator<nr::db::FileKeyHash>(os, ", "));
	os << "\n";
	auto route_node_id_list = command.GetRouteNodeIdListRef();
	std::copy(route_node_id_list.begin(), route_node_id_list.end(), 
		std::ostream_iterator<nr::NodeId>(os, ", "));
	return os;
}

}
}

