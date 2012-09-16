#pragma once
//RequestFileQueryBehavior:20120914
#include <iostream>
#include <fstream>
#include <boost/filesystem/fstream.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "neuria/Neuria.h"
#include "neuria/database/DataBase.h"
#include "command/Command.h"

namespace sy
{

class RequestFileQueryBehavior :
		public boost::enable_shared_from_this<RequestFileQueryBehavior> {
public:
	using Pointer = boost::shared_ptr<RequestFileQueryBehavior>;
    using FilePathFetcher = boost::function<
		boost::filesystem::path (const nr::db::FileKeyHash::HashId&)>;

	static auto Create(int buffer_size, FilePathFetcher file_fetcher, 
			std::ostream& os) -> Pointer {
		return Pointer(new RequestFileQueryBehavior(buffer_size, file_fetcher, os));
	}
	
	auto Bind(nr::ntw::BehaviorDispatcher::Pointer dispatcher) -> void {
		dispatcher->RegisterFunc(cmd::GetCommandId<cmd::RequestFileQueryCommand>(),
			boost::bind(&RequestFileQueryBehavior::OnReceiveRequestFileQuery, 
				this->shared_from_this(), _1, _2));
	}

private:
	RequestFileQueryBehavior(int buffer_size, FilePathFetcher file_fetcher, std::ostream& os) : buffer_size(buffer_size), file_fetcher(file_fetcher), os(os){}

	auto OnReceiveRequestFileQuery(nr::ntw::Session::Pointer session,
			const nr::ByteArray& byte_array) -> void {
		auto command = cmd::RequestFileQueryCommand::Parse(byte_array);	
		auto file_path = this->file_fetcher(command.GetRequestHashId());
		auto file_byte_array = nr::db::SerializeFile(file_path, this->buffer_size);
		session->Send(cmd::RequestFileAnswerCommand(
			file_path, file_byte_array).Serialize());
	}

	int buffer_size;
	FilePathFetcher file_fetcher;
	std::ostream& os;
};
}

