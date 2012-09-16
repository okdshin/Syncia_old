#pragma once
//RequestFileQueryCommand:20120914
#include <iostream>
#include <string>
#include "../neuria/Neuria.h"
#include "../neuria/database/DataBase.h"
#include "Common.h"

namespace sy{
namespace cmd{

class RequestFileQueryCommand{
public:
    RequestFileQueryCommand(){}
    RequestFileQueryCommand(const nr::db::FileKeyHash::HashId& hash_id) 
		: hash_id(hash_id){}

	static auto Parse(const nr::ByteArray& byte_array) -> RequestFileQueryCommand {
		std::stringstream ss(nr::utl::ByteArray2String(byte_array));
		boost::archive::text_iarchive ia(ss);
		auto command = RequestFileQueryCommand();
		ia >> command;
		return command;
	}
	
	auto Serialize() const -> nr::ByteArray {
		std::stringstream ss;
		boost::archive::text_oarchive oa(ss);
		oa << static_cast<const RequestFileQueryCommand&>(*this);
		return nr::utl::String2ByteArray(ss.str());
	}

	auto GetRequestHashId() const -> nr::db::FileKeyHash::HashId { return hash_id; }

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, unsigned int ver){
		ar & hash_id;
	}
	
	nr::db::FileKeyHash::HashId hash_id;;
};

auto operator<<(std::ostream& os,
		const RequestFileQueryCommand& command) -> std::ostream& {
	os << "hash id: " << command.GetRequestHashId(); 
	return os;			
}

template<>
auto GetCommandId<RequestFileQueryCommand>() 
		-> nr::ntw::DispatchCommand::CommandId {
	return nr::ntw::DispatchCommand::CommandId("request_file_query");
}

}
}

