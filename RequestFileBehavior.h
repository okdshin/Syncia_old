#pragma once
//RequestFileBehavior:20120914
#include <iostream>
#include <fstream>
#include <boost/filesystem/fstream.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "neuria/Neuria.h"
#include "command/Command.h"
#include "BehaviorDispatcher.h"
#include "FileSystemPath.h"

namespace syncia
{

class RequestFileBehavior :
		public boost::enable_shared_from_this<RequestFileBehavior> {
public:
	using Pointer = boost::shared_ptr<RequestFileBehavior>;

	static auto Create(int buffer_size, database::FileKeyHashDb::Pointer file_db, 
			std::ostream& os) -> Pointer {
		return Pointer(new RequestFileBehavior(buffer_size, file_db, os));
	}
	
	auto Bind(BehaviorDispatcher::Pointer dispatcher) -> void {
		dispatcher->RegisterFunc(command::GetCommandId<command::RequestFileQueryCommand>(),
			boost::bind(&RequestFileBehavior::OnReceivedRequestFileQuery, 
				this->shared_from_this(), _1, _2));
	}

private:
	RequestFileBehavior(int buffer_size, database::FileKeyHashDb::Pointer file_db,
		std::ostream& os) 
		:buffer_size(buffer_size), file_db(file_db), os(os){}

	auto OnReceivedRequestFileQuery(neuria::network::Session::Pointer session,
			const neuria::ByteArray& byte_array) -> void {
		auto command = command::RequestFileQueryCommand::Parse(byte_array);	
		auto file_path = FileSystemPath();
		if(!this->file_db->IsContain(command.GetRequestHashId())){
			this->os << "requested but the file is not threre..." << std::endl;	
			file_path = FileSystemPath("./not_found.txt");
		}
		else {
			file_path = this->file_db->Get(command.GetRequestHashId()).GetFilePath();
		}
		auto file_byte_array = database::SerializeFile(file_path, this->buffer_size);
		session->Send(
			command::RequestFileAnswerCommand(
				file_path, file_byte_array).Serialize(), 
			neuria::network::Session::OnSendFinishedFunc([](
					neuria::network::Session::Pointer session){
			}),
			neuria::network::Session::OnFailedSendFunc([](
					const neuria::network::ErrorCode&){
				//nothing	
			})
		);
	}

	int buffer_size;
	database::FileKeyHashDb::Pointer file_db;
	std::ostream& os;
};

}


