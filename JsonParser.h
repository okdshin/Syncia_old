#pragma once
//JsonParser:20120827
#include <iostream>
#include <string>
#include <jsoncpp/json.h>
#include "neuria/Neuria.h"

namespace sy
{

class JsonParser{
public:
	using Key = std::string;
	using Dict = Json::Value;

	JsonParser(const std::string& key_name) : key_name(key_name){}

	auto PickupKey(const nr::ByteArray& byte_array) -> Key {
		return Parse(byte_array)[key_name].asString();
	}

	auto Parse(const nr::ByteArray& byte_array) -> Dict {
		std::string command_str{byte_array.begin(), byte_array.end()};
		Json::Value root{};
		Json::Reader reader{};
		reader.parse(command_str, root);
		return root;
	}

	auto Combinate(const Dict& command) -> nr::ByteArray {
		Json::StyledWriter writer{};
		return nr::utl::String2ByteArray(writer.write(command));
	}

private:
	std::string key_name;
	
};


}

