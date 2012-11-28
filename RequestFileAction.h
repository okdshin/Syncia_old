#pragma once
//RequestFileAction:20120913
#include <iostream>
#include <string>
#include <fstream>
#include <boost/filesystem/path.hpp>
#include "command/Command.h"
#include "FileSystemPath.h"
#include "database/DataBase.h"

namespace syncia
{

inline auto ParseFile(const FileSystemPath& download_directory_path, 
		const FileSystemPath& file_path, 
		const neuria::ByteArray& file_byte_array) -> void {
	std::ofstream ofs(
		(download_directory_path/file_path.leaf()).string().c_str(), 
		std::ios::binary);
	ofs.write(static_cast<char*>(static_cast<void*>(const_cast<uint8_t*>(
		&file_byte_array.front()))), file_byte_array.size());
	ofs.close();
}

class RequestFileAction :
		public boost::enable_shared_from_this<RequestFileAction> {
public:
	using Pointer = boost::shared_ptr<RequestFileAction>;
	using OnRepliedFileFunc = boost::function<void (const database::FileKeyHash&)>;

	static auto Create(
			const FileSystemPath& download_directory_path, 
			std::ostream& os) -> Pointer {
		return Pointer(new RequestFileAction(download_directory_path, os));
	}

	auto SetDownloadDirectoryPath(const FileSystemPath& path) -> void {
		this->download_directory_path = path;
	}

	auto Bind(neuria::network::Client::Pointer client) -> void {
		this->client = client;	
	}

	auto RequestFile(const database::FileKeyHash& key_hash, 
			OnRepliedFileFunc on_replied_file_func) -> void {
		const auto node_id = key_hash.GetOwnerId();
		this->client->Connect(node_id,
			neuria::network::Client::OnConnectedFunc(
				boost::bind(&RequestFileAction::OnConnected, 
				this->shared_from_this(), key_hash, on_replied_file_func, _1)),
			neuria::network::Client::OnFailedConnectFunc([](
					const neuria::network::ErrorCode&){
				//nothing	
			}),
			neuria::network::Session::OnClosedFunc([](
					neuria::network::Session::Pointer){
				//nothing
			})
		);
	}

private:
	auto OnConnected(const database::FileKeyHash& key_hash, 
			OnRepliedFileFunc on_replied_file_func, 
			neuria::network::Session::Pointer session) -> void {
		session->Send(
			command::DispatchCommand(
				command::GetCommandId<command::RequestFileQueryCommand>(),
				command::RequestFileQueryCommand(
					key_hash.GetHashId()).Serialize()
			).Serialize(),
			neuria::network::Session::OnSendFinishedFunc(
				boost::bind(&RequestFileAction::OnRequested, 
					this->shared_from_this(), key_hash, on_replied_file_func, _1)),
			neuria::network::Session::OnFailedSendFunc([](
					const neuria::network::ErrorCode&){
				//nothing
			})
		);	
	}
	
	auto OnRequested(
			const database::FileKeyHash& key_hash, 
			OnRepliedFileFunc on_replied_file_func,
			neuria::network::Session::Pointer session) -> void {
		session->StartReceive(
			boost::bind(&RequestFileAction::OnReceived, 
				this->shared_from_this(), key_hash, on_replied_file_func, _1, _2
			)
		);	
	}
	
	auto OnReceived(const database::FileKeyHash& key_hash, 
			OnRepliedFileFunc on_replied_file_func,
			neuria::network::Session::Pointer session, 
			const neuria::ByteArray& byte_array) -> void {
		const auto command = command::RequestFileAnswerCommand::Parse(byte_array);
		ParseFile(this->download_directory_path, 
			command.GetFilePath(), command.GetFileByteArray());
		this->os << "replied file!: " 
			<< command.GetFilePath().filename() << std::endl;
		on_replied_file_func(key_hash);
		session->Close();
	}

private:
    RequestFileAction(
		const FileSystemPath& download_directory_path, std::ostream& os) 
				: download_directory_path(download_directory_path), os(os){}

	neuria::network::Client::Pointer client;
	FileSystemPath download_directory_path;
	std::ostream& os;
};

}

