#pragma once
//Syncia:20120913
#include <iostream>
#include <boost/filesystem.hpp>
#include "SearchKeyHashQueryBehavior.h"
#include "SearchKeyHashAnswerBehavior.h"
#include "SearchKeyHashQueryAction.h"
#include "RequestFileQueryAction.h"
#include "RequestFileQueryBehavior.h"
#include "neuria/database/DataBase.h"

namespace sy
{
class Syncia : public boost::enable_shared_from_this<Syncia> {
public:
	using Pointer = boost::shared_ptr<Syncia>;

	static auto Create(boost::asio::io_service& service, 
			const nr::NodeId& node_id, 
			int buffer_size, int max_hop_count, 
			const boost::filesystem::path& upload_directory_path,
			const boost::filesystem::path& download_directory_path, 
			double threshold, std::ostream& os) -> Pointer {
		
		auto file_db = nr::db::FileKeyHashDb::Create(threshold, buffer_size, os);
		auto skhab = SearchKeyHashAnswerBehavior::Create(
			service, node_id, buffer_size, file_db, os);
		
		auto search_link_pool = nr::ntw::SessionPool::Create();	
		auto skhqb = SearchKeyHashQueryBehavior::Create(
			service, node_id, max_hop_count, search_link_pool, file_db, os);
		
		skhqb->SetSearchKeyHashAnswerer(
			[skhab, &os](const cmd::SearchKeyHashQueryCommand& command){
				os << "return back!" << std::endl;
				skhab->AnswerSearchKeyHash(
					cmd::SearchKeyHashQueryCommand2SearchKeyHashAnswerCommand(
						command));});
		
		auto skhqa = SearchKeyHashQueryAction::Create(
			service, node_id, buffer_size, search_link_pool, os);
		
		auto rfqa = RequestFileQueryAction::Create(download_directory_path, os);		
		auto rfqb = RequestFileQueryBehavior::Create(buffer_size, 
			[file_db](const nr::db::FileKeyHash::HashId& hash_id){
				return file_db->Get(hash_id).GetFilePath();
			}, os);
		std::cout << "upload_directory" << upload_directory_path << std::endl;	
		return Pointer(new Syncia(skhab, skhqb, skhqa, rfqa, rfqb, file_db, 
			node_id, upload_directory_path, os));
	}

	auto Bind(nr::ntw::BehaviorDispatcher::Pointer dispatcher) -> void {
		this->skhqb->Bind(dispatcher);
		this->skhab->Bind(dispatcher);
		this->rfqb->Bind(dispatcher);
	}

	auto Bind(nr::ntw::Client::Pointer client) -> void { 
		this->skhab->Bind(client);
		this->skhqa->Bind(client);
		this->rfqa->Bind(client);
	}

	auto ConnectSearchLink(const nr::NodeId& node_id) -> void {
		this->skhqa->ConnectSearchLink(node_id);	
	}

	auto QuerySearchKeyHash(
			const nr::db::FileKeyHashDb::KeywardList& keyward_list) -> void {
		this->skhqa->QuerySearchKeyHash(keyward_list);	
	}

	auto RequestFile(const nr::db::FileKeyHash::HashId& hash_id, 
			const nr::NodeId& node_id) -> void {
		this->rfqa->RequestFile(hash_id, node_id);	
	}

	auto SetDownloadDirectoryPath(
			const std::string& download_directory_path) -> void {
		this->rfqa->SetDownloadDirectoryPath(download_directory_path);	
	}

	auto UploadFile(const nr::db::FileKeyHash::Keyward& keyward, 
			const boost::filesystem::path& file_path) -> void {
		std::cout << "SelfNodeId: " << this->node_id << std::endl;
		this->db->Add(keyward, file_path, this->node_id);
	}
	
	auto UploadDirectory() -> void {
		std::cout << this->upload_directory_path.string() << std::endl;
		const auto end = boost::filesystem::recursive_directory_iterator();
		for(auto path_iter = boost::filesystem::recursive_directory_iterator(
				this->upload_directory_path); path_iter != end; ++path_iter){
			if(boost::filesystem::is_regular_file(path_iter->status())){
				const auto file_path = boost::filesystem::path(*path_iter);
				this->db->Add(
					nr::db::FileKeyHash::Keyward(file_path.filename().string()), 
					file_path, this->node_id);
			}
		}
	}
/*
	auto UpdateDb(directory_path) -> void {
		const auto end = boost::filesystem::recursive_directory_iterator();
		for(auto path_iter = boost::filesystem::recursive_directory_iterator(
				directory_path); path_iter != end; ++path_iter){
			if(boost::filesystem::is_regular_file(path_iter->status())){
				const auto file_path = boost::filesystem::path(*path_iter);
				this->directory_db->Add(file_path, last_write_time);
			}
		}
			
	}
*/
	auto GetFileKeyHashDb() const -> nr::db::FileKeyHashDb::Pointer { return db; }


private:
    Syncia(SearchKeyHashAnswerBehavior::Pointer skhab,
			SearchKeyHashQueryBehavior::Pointer skhqb, 
			SearchKeyHashQueryAction::Pointer skhqa,
			RequestFileQueryAction::Pointer rfqa,
			RequestFileQueryBehavior::Pointer rfqb, 
			nr::db::FileKeyHashDb::Pointer file_db,
			const nr::NodeId& node_id,
			const boost::filesystem::path& upload_directory_path,
			std::ostream& os)
			: skhab(skhab), skhqb(skhqb), skhqa(skhqa), 
			rfqa(rfqa), rfqb(rfqb), db(file_db), 
			node_id(node_id), upload_directory_path(upload_directory_path), os(os){}
	
	nr::ntw::Client::Pointer client;
	
	SearchKeyHashAnswerBehavior::Pointer skhab;
	SearchKeyHashQueryBehavior::Pointer skhqb;
	SearchKeyHashQueryAction::Pointer skhqa;
	
	RequestFileQueryAction::Pointer rfqa;
	RequestFileQueryBehavior::Pointer rfqb; 

	nr::db::FileKeyHashDb::Pointer db;
	nr::NodeId node_id;
	boost::filesystem::path upload_directory_path;
	std::ostream& os;
};

}

