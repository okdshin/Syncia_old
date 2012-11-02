#pragma once
//RequestFileQueryCommand:20120914
#include <iostream>
#include <string>
#include "../neuria/Neuria.h"
#include "Common.h"
#include "../database/DataBase.h"

namespace syncia{
namespace command{

class RequestFileQueryCommand{
public:
    RequestFileQueryCommand(){}
    RequestFileQueryCommand(const database::HashId& hash_id) 
		: hash_id(hash_id()){}

	static auto Parse(const neuria::ByteArray& byte_array) -> RequestFileQueryCommand {
		std::stringstream ss(neuria::utility::ByteArray2String(byte_array));
		boost::archive::text_iarchive ia(ss);
		auto command = RequestFileQueryCommand();
		ia >> command;
		return command;
	}
	
	auto Serialize() const -> neuria::ByteArray {
		std::stringstream ss;
		boost::archive::text_oarchive oa(ss);
		oa << static_cast<const RequestFileQueryCommand&>(*this);
		return neuria::utility::String2ByteArray(ss.str());
	}

	auto GetRequestHashId() const -> database::HashId { return database::HashId(hash_id); }

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, unsigned int ver){
		ar & hash_id;
	}
	
	database::HashId::WrappedType hash_id;;
};

inline auto operator<<(std::ostream& os,
		const RequestFileQueryCommand& command) -> std::ostream& {
	os << "hash id: " << command.GetRequestHashId(); 
	return os;			
}

template<>
auto GetCommandId<RequestFileQueryCommand>() -> CommandId {
	return CommandId("request_file_query");
}

}
}

