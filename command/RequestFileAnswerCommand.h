#pragma once
//RequestFileAnswerCommand:20120914
#include <iostream>
#include <string>
#include <boost/filesystem/path.hpp>
#include "../neuria/Neuria.h"
#include "Common.h"

namespace sy{
namespace cmd{

class RequestFileAnswerCommand{
public:
    RequestFileAnswerCommand(){}
    RequestFileAnswerCommand(const boost::filesystem::path& file_path, 
			const nr::ByteArray& file_byte_array) 
		: file_path_str(file_path.string()), file_byte_array(file_byte_array){}

	static auto Parse(const nr::ByteArray& byte_array) -> RequestFileAnswerCommand {
		std::stringstream ss(nr::utl::ByteArray2String(byte_array));
		boost::archive::text_iarchive ia(ss);
		auto command = RequestFileAnswerCommand();
		ia >> command;
		return command;
	}
	
	auto Serialize() const -> nr::ByteArray {
		std::stringstream ss;
		boost::archive::text_oarchive oa(ss);
		oa << static_cast<const RequestFileAnswerCommand&>(*this);
		return nr::utl::String2ByteArray(ss.str());
	}

	auto GetFilePath() const -> boost::filesystem::path { 
		return boost::filesystem::path(file_path_str); 
	}

	auto GetFileByteArray() const -> nr::ByteArray { return file_byte_array; }

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, unsigned int ver){
		ar & file_path_str & file_byte_array;
	}
	
	std::string file_path_str;
	nr::ByteArray file_byte_array;
};

auto operator<<(std::ostream& os,
		const RequestFileAnswerCommand& command) -> std::ostream& {
	os << "FilePath: " << command.GetFilePath() 
		<< "FileByteArray: ";
	auto file_byte_array = command.GetFileByteArray();
	std::copy(file_byte_array.begin(), file_byte_array.end(), 
		std::ostream_iterator<char>(os, ""));
	return os;			
}

template<>
auto GetCommandId<RequestFileAnswerCommand>() -> CommandId {
	return CommandId("request_file_answer");
}

}
}

