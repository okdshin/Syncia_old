#pragma once
//SpreadKeyHashCommand:20120912
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

class SpreadKeyHashCommand {
public:
	SpreadKeyHashCommand(){}

	static auto Parse(const neuria::ByteArray& byte_array) -> SpreadKeyHashCommand {
		std::stringstream ss(neuria::utility::ByteArray2String(byte_array));
		boost::archive::text_iarchive ia(ss);
		auto command = SpreadKeyHashCommand();
		ia >> command;
		return command;
	}
	
	auto Serialize() const -> neuria::ByteArray {
		std::stringstream ss;
		boost::archive::text_oarchive oa(ss);
		oa << static_cast<const SpreadKeyHashCommand&>(*this);
		return neuria::utility::String2ByteArray(ss.str());
	}

	auto GetSpreadKeyHashList() const -> database::FileKeyHashList {
		return spread_key_hash_list; }

	auto AddSpreadKeyHashList(
			const database::FileKeyHashList& spread_key_hash_list) -> void {
		std::copy(spread_key_hash_list.begin(), spread_key_hash_list.end(), 
			std::back_inserter(this->spread_key_hash_list));
	}

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, unsigned int ver){
		ar & spread_key_hash_list;
	}

	database::FileKeyHashList spread_key_hash_list;
	
};

inline auto operator<<(std::ostream& os, 
		const SpreadKeyHashCommand& command) -> std::ostream& {
	auto key_hash_list = command.GetSpreadKeyHashList();
	os << "KeyHashList:";
	std::copy(key_hash_list.begin(), key_hash_list.end(), 
		std::ostream_iterator<database::FileKeyHash>(os, " "));
	return os;
}

template<>
inline auto GetCommandId<SpreadKeyHashCommand>() -> CommandId {
	return CommandId("spread_key_hash_command");
}

}
}

