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

	auto SetOnRepliedFileFunc(OnRepliedFileFunc on_replied_file_func) -> void {
		this->on_replied_file_func = on_replied_file_func;
	}

	auto SetDownloadDirectoryPath(const FileSystemPath& path) -> void {
		this->download_directory_path = path;
	}

	auto Bind(neuria::network::Client::Pointer client) -> void {
		this->client = client;	
	}

	auto RequestFile(const database::FileKeyHash& key_hash) -> void {
		auto node_id = key_hash.GetOwnerId();
		this->client->Connect(node_id,
			neuria::network::Client::OnConnectedFunc([this, key_hash](
					neuria::network::Session::Pointer session){
				session->Send(
					command::DispatchCommand(
						command::GetCommandId<command::RequestFileQueryCommand>(),
						command::RequestFileQueryCommand(
							key_hash.GetHashId()).Serialize()
					).Serialize(),					
					neuria::network::Session::OnSendFinishedFunc([this, key_hash](
							neuria::network::Session::Pointer session){
						session->StartReceive(
							neuria::network::Session::OnReceivedFunc([this, key_hash](
									neuria::network::Session::Pointer session,
									const neuria::ByteArray& byte_array){
								auto command = command::RequestFileAnswerCommand::Parse(byte_array);
								ParseFile(this->download_directory_path, command.GetFilePath(), command.GetFileByteArray());
								this->os << "replied file!: " 
									<< command.GetFilePath().filename() << std::endl;
								this->on_replied_file_func(key_hash);
								//session->Close();
							})
						);
					}),
					neuria::network::Session::OnFailedSendFunc([](
							const neuria::network::ErrorCode&){
						//nothing
					})
				);
			}),
			neuria::network::Client::OnFailedConnectFunc([](
					const neuria::network::ErrorCode&){
				//nothing	
			}),
			neuria::network::Session::OnClosedFunc([](neuria::network::Session::Pointer){
				//nothing
			})
		);
	}


private:
    RequestFileAction(
		const FileSystemPath& download_directory_path, std::ostream& os) 
				: download_directory_path(download_directory_path), os(os){
		this->SetOnRepliedFileFunc(
			[this](const database::FileKeyHash& key_hash){
				auto file_path = key_hash.GetFilePath();
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

