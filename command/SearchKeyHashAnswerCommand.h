#pragma once
//SearchKeyHashAnswerCommand:20120912
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
#include "SearchKeyHashCommandData.h"
#include "SearchKeyHashQueryCommand.h"

namespace sy{
namespace cmd{

using StrList = std::vector<std::string>;

class SearchKeyHashAnswerCommand {
public:
	SearchKeyHashAnswerCommand(){}
	SearchKeyHashAnswerCommand(const SearchKeyHashCommandData& data)
		: data(data){}

	static auto Parse(const nr::ByteArray& byte_array) -> SearchKeyHashAnswerCommand {
		std::stringstream ss(nr::utl::ByteArray2String(byte_array));
		boost::archive::text_iarchive ia(ss);
		auto command = SearchKeyHashAnswerCommand();
		ia >> command;
		return command;
	}

	auto GetFindKeyHashList() const -> nr::db::FileKeyHashDb::FileKeyHashList {
		return data.GetFindKeyHashListRef();	
	}

	auto GetRouteNodeIdList() const -> std::vector<nr::NodeId> {
		return data.GetRouteNodeIdListRef();
	}

	auto Serialize() const -> nr::ByteArray {
		std::stringstream ss;
		boost::archive::text_oarchive oa(ss);
		oa << static_cast<const SearchKeyHashAnswerCommand&>(*this);
		return nr::utl::String2ByteArray(ss.str());
	}

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, unsigned int ver){
		ar & data;
	}

	friend auto operator<<(std::ostream& os, 
		const SearchKeyHashAnswerCommand& command) -> std::ostream&;

	SearchKeyHashCommandData data;
};

auto operator<<(std::ostream& os, 
		const SearchKeyHashAnswerCommand& command) -> std::ostream& {
	os << command.data;
	return os;
}

template<>
auto GetCommandId<SearchKeyHashAnswerCommand>() -> CommandId {
	return CommandId("search_key_hash_answer");
}

auto SearchKeyHashQueryCommand2SearchKeyHashAnswerCommand(
		const SearchKeyHashQueryCommand& query_command) 
		-> SearchKeyHashAnswerCommand {
	return SearchKeyHashAnswerCommand(query_command.GetData());	
}

}
}

