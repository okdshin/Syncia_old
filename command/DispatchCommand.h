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

namespace syncia{
namespace command{

class CommandIdType{};
using CommandId = neuria::utility::TypeWrapper<std::string, CommandIdType>;

class DispatchCommand{
public: 
	DispatchCommand(){}
	DispatchCommand(const CommandId& command_id, const neuria::ByteArray& byte_array)
		: command_id_str(command_id()), byte_array(byte_array){}

	static auto Parse(const neuria::ByteArray& byte_array) -> DispatchCommand {
		std::stringstream ss(neuria::utility::ByteArray2String(byte_array));
		boost::archive::text_iarchive ia(ss);
		auto command = DispatchCommand();
		ia >> command;
		return command;

		/*
		std::stringstream ss;
		auto str = neuria::utility::ByteArray2String(byte_array);
		ss << str;
		std::string command_id_str;
		ss >> command_id_str;
		std::string wrapped_byte_array_str = std::string(
			str.begin()+command_id_str.size()+1, str.end());
		return DispatchCommand(CommandId(command_id_str), 
			neuria::utility::String2ByteArray(wrapped_byte_array_str));
		*/	
	}
	
	auto Serialize()const -> neuria::ByteArray {
		std::stringstream ss;
		boost::archive::text_oarchive oa(ss);
		oa << static_cast<const DispatchCommand&>(*this);
		return neuria::utility::String2ByteArray(ss.str());	
			
		/*
		std::stringstream ss;
		ss << this->command_id_str << " " 
			<< neuria::utility::ByteArray2String(this->byte_array) << std::flush;
		return neuria::utility::String2ByteArray(ss.str());
		*/
	}

	auto GetCommandId()const -> CommandId { return CommandId(command_id_str); }
	auto GetWrappedByteArray()const -> neuria::ByteArray { return byte_array; }

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, unsigned int ver){
		ar & command_id_str & byte_array;
	}

	std::string command_id_str;
	neuria::ByteArray byte_array;
};

inline auto operator<<(std::ostream& os, const DispatchCommand& command) -> std::ostream& {
	os << "command_id:\"" << command.GetCommandId()() << "\" byte_array:\"" 
		<< neuria::utility::ByteArray2String(command.GetWrappedByteArray()) << "\"" << std::endl;
	return os;
}


}
}
