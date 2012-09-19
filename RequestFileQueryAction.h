#pragma once
//RequestFileQueryAction:20120913
#include <iostream>
#include <string>
#include <fstream>
#include <boost/filesystem/fstream.hpp>
#include "command/Command.h"

namespace sy
{

auto ParseFile(const boost::filesystem::path& download_directory_path, 
		const boost::filesystem::path& file_path, 
		const nr::ByteArray& file_byte_array) -> void {
	std::ofstream ofs(
		(download_directory_path.string()+file_path.leaf().string()).c_str(), 
		std::ios::binary);
	ofs.write(&file_byte_array.front(), file_byte_array.size());
	ofs.close();
}

class RequestFileQueryAction :
		public boost::enable_shared_from_this<RequestFileQueryAction> {
public:
	using Pointer = boost::shared_ptr<RequestFileQueryAction>;

	static auto Create(const std::string& download_directory_path, 
			std::ostream& os) -> Pointer {
		return Pointer(new RequestFileQueryAction(download_directory_path, os));
	}

	auto Bind(nr::ntw::Client::Pointer client) -> void {
		this->client = client;	
	}

	auto RequestFile(const nr::db::FileKeyHash::HashId& hash_id, 
			const nr::NodeId& node_id) -> void {
		Communicate(this->client, node_id,
			nr::ntw::DispatchCommand(
				cmd::GetCommandId<cmd::RequestFileQueryCommand>(),
				cmd::RequestFileQueryCommand(hash_id).Serialize()
			).Serialize(),
			[this](nr::ntw::Session::Pointer session, 
					const nr::ByteArray& byte_array){
				auto command = cmd::RequestFileAnswerCommand::Parse(byte_array);
				ParseFile(this->download_directory_path, command.GetFilePath(), 
					command.GetFileByteArray());
				this->os << "replied file!: " 
					<< command.GetFilePath().filename() << std::endl;
				//session->Close();
			});
	}

	auto SetDownloadDirectoryPath(const std::string& download_directory_path) -> void {
		this->download_directory_path = download_directory_path;	
	}

private:
    RequestFileQueryAction(
			const std::string& download_directory_path, std::ostream& os)
		: download_directory_path(download_directory_path), os(os){}

	nr::ntw::Client::Pointer client;
	boost::filesystem::path download_directory_path;
	std::ostream& os;
};

}

