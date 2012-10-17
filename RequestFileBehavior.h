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
    using FilePathFetcher = boost::function<
		FileSystemPath (const database::HashId&)>;

	static auto Create(int buffer_size, FilePathFetcher file_fetcher, 
			std::ostream& os) -> Pointer {
		return Pointer(new RequestFileBehavior(buffer_size, file_fetcher, os));
	}
	
	auto Bind(BehaviorDispatcher::Pointer dispatcher) -> void {
		dispatcher->RegisterFunc(command::GetCommandId<command::RequestFileQueryCommand>(),
			boost::bind(&RequestFileBehavior::OnReceiveRequestFileQuery, 
				this->shared_from_this(), _1, _2));
	}

private:
	RequestFileBehavior(int buffer_size, FilePathFetcher file_fetcher, 
		std::ostream& os) 
		:buffer_size(buffer_size), file_fetcher(file_fetcher), os(os){}

	auto OnReceiveRequestFileQuery(neuria::network::Session::Pointer session,
			const neuria::ByteArray& byte_array) -> void {
		auto command = command::RequestFileQueryCommand::Parse(byte_array);	
		auto file_path = this->file_fetcher(command.GetRequestHashId());
		auto file_byte_array = database::SerializeFile(file_path, this->buffer_size);
		session->Send(command::RequestFileAnswerCommand(
			file_path, file_byte_array).Serialize(), 
			[](neuria::network::Session::Pointer session){
				session->Close();	
			});
	}

	int buffer_size;
	FilePathFetcher file_fetcher;
	std::ostream& os;
};

}

