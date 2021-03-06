#pragma once
//SearchKeyHashCommand:20120912
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

class SearchKeyHashCommand {
public:
	SearchKeyHashCommand(){}
	SearchKeyHashCommand(const database::KeywordList& search_keyword_list)
		: search_keyword_list(search_keyword_list()){}

	static auto Parse(const neuria::ByteArray& byte_array) -> SearchKeyHashCommand {
		std::stringstream ss(neuria::utility::ByteArray2String(byte_array));
		boost::archive::text_iarchive ia(ss);
		auto command = SearchKeyHashCommand();
		ia >> command;
		return command;
	}
	
	auto Serialize() const -> neuria::ByteArray {
		std::stringstream ss;
		boost::archive::text_oarchive oa(ss);
		oa << static_cast<const SearchKeyHashCommand&>(*this);
		return neuria::utility::String2ByteArray(ss.str());
	}

	auto GetSearchKeywordList() const -> database::KeywordList { 
		return database::KeywordList(search_keyword_list); }

	auto GetFoundKeyHashList() const -> database::FileKeyHashList {
		return found_key_hash_list; }

	auto AddFoundKeyHashList(
			const database::FileKeyHashList& found_key_hash_list) -> void {
		std::copy(found_key_hash_list.begin(), found_key_hash_list.end(), 
			std::back_inserter(this->found_key_hash_list));
	}

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, unsigned int ver){
		ar & search_keyword_list & found_key_hash_list;
	}

	database::KeywordList::WrappedType search_keyword_list;
	database::FileKeyHashList found_key_hash_list;
	
};

inline auto operator<<(std::ostream& os, 
		const SearchKeyHashCommand& command) -> std::ostream& {
	auto keyword_list = command.GetSearchKeywordList()();
	os << "KeywordList:";
	std::copy(keyword_list.begin(), keyword_list.end(), 
		std::ostream_iterator<database::Keyword::WrappedType>(os, " "));
	os << " ";
	auto key_hash_list = command.GetFoundKeyHashList();
	os << "KeyHashList:";
	std::copy(key_hash_list.begin(), key_hash_list.end(), 
		std::ostream_iterator<database::FileKeyHash>(os, " "));
	return os;
}

template<>
inline auto GetCommandId<SearchKeyHashCommand>() -> CommandId {
	return CommandId("search_key_hash_command");
}

}
}

