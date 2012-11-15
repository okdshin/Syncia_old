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

inline auto ParseFile(const FileSystemPath& download_directory_path, 
		const FileSystemPath& file_path, 
		const neuria::ByteArray& file_byte_array) -> void {
	std::ofstream ofs(
		(download_directory_path.string()+file_path.leaf().string()).c_str(), 
		std::ios::binary);
	ofs.write(static_cast<char*>(static_cast<void*>(const_cast<uint8_t*>(
		&file_byte_array.front()))), file_byte_array.size());
	ofs.close();
}

class RequestFileAction :
		public boost::enable_shared_from_this<RequestFileAction> {
public:
	using Pointer = boost::shared_ptr<RequestFileAction>;
	using OnRepliedFileFunc = boost::function<void (const FileSystemPath&)>;

	static auto Create(
			const FileSystemPath& download_directory_path, 
			std::ostream& os) -> Pointer {
		return Pointer(new RequestFileAction(download_directory_path, os));
	}

	auto SetOnRepliedFileFunc(OnRepliedFileFunc on_replied_file_func) -> void {
		this->on_replied_file_func = on_replied_file_func;
	}

	auto SetDownloadDirectoryPath(const FileSystemPath& path) -> void {
		this->download_directory_path = path;
	}

	auto Bind(neuria::network::Client::Pointer client) -> void {
		this->client = client;	
	}

	auto RequestFile(const database::HashId& hash_id, 
			const neuria::network::NodeId& node_id) -> void {
		neuria::network::Communicate(this->client, node_id,
			[this](const neuria::network::ErrorCode& error_code){
				this->os << "failed connect to request file: " 
					<< error_code << std::endl;
			},
			command::DispatchCommand(
				command::GetCommandId<command::RequestFileQueryCommand>(),
				command::RequestFileQueryCommand(hash_id).Serialize()
			).Serialize(),
			[this](neuria::network::Session::Pointer session, 
					const neuria::ByteArray& byte_array){
				auto command = command::RequestFileAnswerCommand::Parse(byte_array);
				ParseFile(this->download_directory_path, command.GetFilePath(), 
					command.GetFileByteArray());
				this->os << "replied file!: " 
					<< command.GetFilePath().filename() << std::endl;
				this->on_replied_file_func(command.GetFilePath());
				session->Close();
			},
			[](neuria::network::Session::Pointer){}
		);
	}


private:
    RequestFileAction(
		const FileSystemPath& download_directory_path, std::ostream& os) 
				: download_directory_path(download_directory_path), os(os){
		this->SetOnRepliedFileFunc(
			[this](const FileSystemPath& file_path){
				this->os << "file \"" << file_path 
					<< "\" was replied! (this is default " 
					<< "\"syncia::RequestFileAction::OnRepliedFileFunc\")" 
					<< std::endl;
			}
		);	
	}

	OnRepliedFileFunc on_replied_file_func;
	neuria::network::Client::Pointer client;
	FileSystemPath download_directory_path;
	std::ostream& os;
};

}

