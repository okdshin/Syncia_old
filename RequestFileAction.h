#pragma once
//RequestFileAction:20120913
#include <iostream>
#include <string>
#include <fstream>
#include <boost/filesystem/path.hpp>
#include "command/Command.h"

namespace sy
{

auto ParseFile(const nr::FileSystemPath& download_directory_path, 
		const nr::FileSystemPath& file_path, 
		const nr::ByteArray& file_byte_array) -> void {
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

	auto Bind(nr::ntw::Client::Pointer client) -> void {
		this->client = client;	
	}

	auto RequestFile(const nr::db::HashId& hash_id, const nr::NodeId& node_id, 
			const nr::FileSystemPath& download_directory_path) -> void {
		Communicate(this->client, node_id,
			cmd::DispatchCommand(
				cmd::GetCommandId<cmd::RequestFileQueryCommand>(),
				cmd::RequestFileQueryCommand(hash_id).Serialize()
			).Serialize(),
			[this, download_directory_path](nr::ntw::Session::Pointer session, 
					const nr::ByteArray& byte_array){
				auto command = cmd::RequestFileAnswerCommand::Parse(byte_array);
				ParseFile(download_directory_path, command.GetFilePath(), 
					command.GetFileByteArray());
				this->os << "replied file!: " 
					<< command.GetFilePath().filename() << std::endl;
			},
			[](nr::ntw::Session::Pointer){});
	}


private:
    RequestFileAction(std::ostream& os) : os(os){}

	nr::ntw::Client::Pointer client;
	std::ostream& os;
};

}

