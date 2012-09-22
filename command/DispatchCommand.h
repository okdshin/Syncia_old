#pragma once
//DispatchCommand:20120913
#include <iostream>
#include <sstream>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include "../neuria/Neuria.h"

namespace sy{
namespace cmd{

class DispatchCommand{
public:
	using CommandId = std::string;
    
	DispatchCommand(){}
	DispatchCommand(const CommandId& command_id, const nr::ByteArray& byte_array)
		: command_id(command_id), byte_array(byte_array){}

	static auto Parse(const nr::ByteArray& byte_array) -> DispatchCommand {
		std::stringstream ss(nr::utl::ByteArray2String(byte_array));
		boost::archive::text_iarchive ia(ss);
		auto command = DispatchCommand();
		ia >> command;
		return command;
	}
	
	auto Serialize()const -> nr::ByteArray {
		std::stringstream ss;
		boost::archive::text_oarchive oa(ss);
		oa << static_cast<const DispatchCommand&>(*this);
		return nr::utl::String2ByteArray(ss.str());	
	}

	auto GetCommandId()const -> CommandId { return command_id; }
	auto GetWrappedByteArray()const -> nr::ByteArray { return byte_array; }

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, unsigned int ver){
		ar & command_id & byte_array;
	}

	CommandId command_id;
	nr::ByteArray byte_array;
};

auto operator<<(std::ostream& os, const DispatchCommand& command) -> std::ostream& {
	os << "command_id:" << command.GetCommandId() << " byte_array:" 
		<< nr::utl::ByteArray2String(command.GetWrappedByteArray()) << std::endl;
	return os;
}


}
}