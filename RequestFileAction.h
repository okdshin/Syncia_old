#pragma once
//RequestFileAction:20120913
#include <iostream>
#include <string>
#include <fstream>
#include <boost/filesystem/path.hpp>
#include "command/Command.h"
#include "FileSystemPath.h"

namespace syncia
{

auto ParseFile(const FileSystemPath& download_directory_path, 
		const FileSystemPath& file_path, 
		const neuria::ByteArray& file_byte_array) -> void {
	std::ofstream ofs(
		(download_directory_path.string()+file_path.leaf().string()).c_str(), 
		std::ios::binary);
	ofs.write(&file_byte_array.front(), file_byte_array.size());
	ofs.close();
}

class RequestFileAction :
		public boost::enable_shared_from_this<RequestFileAction> {
public:
	using Pointer = boost::shared_ptr<RequestFileAction>;

	static auto Create(std::ostream& os) -> Pointer {
		return Pointer(new RequestFileAction(os));
	}

	auto Bind(neuria::network::Client::Pointer client) -> void {
		this->client = client;	
	}

	auto RequestFile(const database::HashId& hash_id, const neuria::network::NodeId& node_id, 
			const FileSystemPath& download_directory_path) -> void {
		Communicate(this->client, node_id,
			command::DispatchCommand(
				command::GetCommandId<command::RequestFileQueryCommand>(),
				command::RequestFileQueryCommand(hash_id).Serialize()
			).Serialize(),
			[this, download_directory_path](neuria::network::Session::Pointer session, 
					const neuria::ByteArray& byte_array){
				auto command = command::RequestFileAnswerCommand::Parse(byte_array);
				ParseFile(download_directory_path, command.GetFilePath(), 
					command.GetFileByteArray());
				this->os << "replied file!: " 
					<< command.GetFilePath().filename() << std::endl;
			},
			[](neuria::network::Session::Pointer){});
	}


private:
    RequestFileAction(std::ostream& os) : os(os){}

	neuria::network::Client::Pointer client;
	std::ostream& os;
};

}

