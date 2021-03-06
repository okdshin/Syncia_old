#pragma once
//SearchKeyHashQueryCommand:20120912
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
#include "SearchKeyHashCommandData.h"

namespace syncia{
namespace command{

using StrList = std::vector<std::string>;

class SearchKeyHashQueryCommand {
public:
	SearchKeyHashQueryCommand(){}
	SearchKeyHashQueryCommand(
			const nr::db::FileKeyHashDb::KeywardList& search_keyward_list)
		: data(search_keyward_list){}

	static auto Parse(const nr::ByteArray& byte_array) -> SearchKeyHashQueryCommand {
		std::stringstream ss(nr::utl::ByteArray2String(byte_array));
		boost::archive::text_iarchive ia(ss);
		auto command = SearchKeyHashQueryCommand();
		ia >> command;
		return command;
	}
	
	auto GetSearchKeywardList() const -> nr::db::FileKeyHashDb::KeywardList { 
		return this->data.GetSearchKeywardListRef(); }

	auto GetFindKeyHashList() const -> std::vector<nr::db::FileKeyHash> {
		return data.GetFindKeyHashListRef();	
	}

	auto AddFindKeyHashList(
			const nr::db::FileKeyHashDb::FileKeyHashList& find_key_hash_list) -> void {
		std::copy(find_key_hash_list.begin(), find_key_hash_list.end(), 
			std::back_inserter(this->data.GetFindKeyHashListRef()));
	}

	auto AddRouteNodeId(const nr::NodeId& node_id) -> void {
		this->data.GetRouteNodeIdListRef().push_back(node_id);
	}

	auto GetHopCount() -> int { return this->data.GetRouteNodeIdListRef().size(); }

	auto GetData() const -> SearchKeyHashCommandData { return data; }


	auto Serialize() const -> nr::ByteArray {
		std::stringstream ss;
		boost::archive::text_oarchive oa(ss);
		oa << static_cast<const SearchKeyHashQueryCommand&>(*this);
		return nr::utl::String2ByteArray(ss.str());
	}
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, unsigned int ver){
		ar & data;
	}
	
	SearchKeyHashCommandData data;
};

auto operator<<(std::ostream& os, 
		const SearchKeyHashQueryCommand& command) -> std::ostream& {
	os << command.GetData();
	return os;
}

template<>
auto GetCommandId<SearchKeyHashQueryCommand>() -> CommandId {
	return CommandId("search_key_hash_query");
}

}
}

