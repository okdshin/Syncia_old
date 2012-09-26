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
#include "Common.h"

namespace sy{
namespace cmd{

class SearchKeyHashCommand {
public:
	SearchKeyHashCommand(){}
	SearchKeyHashCommand(const nr::db::KeywardList& search_keyward_list)
		: search_keyward_list(search_keyward_list()){}

	static auto Parse(const nr::ByteArray& byte_array) -> SearchKeyHashCommand {
		std::stringstream ss(nr::utl::ByteArray2String(byte_array));
		boost::archive::text_iarchive ia(ss);
		auto command = SearchKeyHashCommand();
		ia >> command;
		return command;
	}
	
	auto Serialize() const -> nr::ByteArray {
		std::stringstream ss;
		boost::archive::text_oarchive oa(ss);
		oa << static_cast<const SearchKeyHashCommand&>(*this);
		return nr::utl::String2ByteArray(ss.str());
	}

	auto GetSearchKeywardList() const -> nr::db::KeywardList { 
		return nr::db::KeywardList(search_keyward_list); }

	auto GetFoundKeyHashList() const -> nr::db::FileKeyHashList {
		return found_key_hash_list; }

	auto AddFoundKeyHashList(
			const nr::db::FileKeyHashList& found_key_hash_list) -> void {
		std::copy(found_key_hash_list.begin(), found_key_hash_list.end(), 
			std::back_inserter(this->found_key_hash_list));
	}

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, unsigned int ver){
		ar & search_keyward_list & found_key_hash_list;
	}

	nr::db::KeywardList::WrappedType search_keyward_list;
	nr::db::FileKeyHashList found_key_hash_list;
	
};

auto operator<<(std::ostream& os, 
		const SearchKeyHashCommand& command) -> std::ostream& {
	auto keyward_list = command.GetSearchKeywardList()();
	os << "KeywardList:";
	std::copy(keyward_list.begin(), keyward_list.end(), 
		std::ostream_iterator<nr::db::Keyward::WrappedType>(os, " "));
	os << " ";
	auto key_hash_list = command.GetFoundKeyHashList();
	os << "KeyHashList:";
	std::copy(key_hash_list.begin(), key_hash_list.end(), 
		std::ostream_iterator<nr::db::FileKeyHash>(os, " "));
	return os;
}

template<>
auto GetCommandId<SearchKeyHashCommand>() -> CommandId {
	return CommandId("search_key_hash_command");
}

}
}

