#pragma once
//JsonParser:20120827
#include <iostream>
#include <string>
#include <jsoncpp/json.h>
#include "neuria/Utility.h"

namespace sy
{

class JsonParser{
public:
	using Key = std::string;
	using ParsedCommand = Json::Value;

	JsonParser(const std::string& key_name) : key_name(key_name){}

	auto PickupKey(const utl::ByteArray& byte_array) -> Key {
		return this->Parse(byte_array)[key_name].asString();
	}

	auto Parse(const utl::ByteArray& byte_array) -> ParsedCommand {
		std::string command_str{byte_array.begin(), byte_array.end()};
		Json::Value root{};
		Json::Reader reader{};
		reader.parse(command_str, root);
		return root;
	}

private:
	std::string key_name;
	
};


}

