#pragma once
//UploadAction:20120913
#include <iostream>
#include <boost/filesystem.hpp>
#include "neuria/Neuria.h"
#include "database/DataBase.h"
#include "FileSystemPathList.h"

namespace syncia
{

class UploadAction : public boost::enable_shared_from_this<UploadAction> {
public:
	using Pointer = boost::shared_ptr<UploadAction>;

	static auto Create(database::FileKeyHashDb::Pointer file_db, 
			const neuria::network::NodeId& node_id, 
			std::ostream& os) -> Pointer {	
		return Pointer(new UploadAction(file_db, node_id, os));
	}

	auto UploadFile(const FileSystemPath& file_path) -> void {
		this->file_db->Add(database::Keyword(file_path.filename().string()), 
			file_path, this->node_id);
	}
/*	
	auto UploadDirectory(const nr::FileSystemPath& upload_directory_path) -> void {
		const auto end = boost::filesystem::recursive_directory_iterator();
		for(auto path_iter = boost::filesystem::recursive_directory_iterator(
				upload_directory_path); path_iter != end; ++path_iter){
			if(boost::filesystem::is_regular_file(path_iter->status())){
				const auto file_path = nr::FileSystemPath(*path_iter);
				this->UploadFile(
					nr::db::Keyword(file_path.filename().string()), file_path);
			}
		}
	}
*/
private:
    UploadAction(database::FileKeyHashDb::Pointer file_db, const neuria::network::NodeId& node_id,
			std::ostream& os)
			: file_db(file_db), node_id(node_id), os(os){}
	
	database::FileKeyHashDb::Pointer file_db;
	neuria::network::NodeId node_id;
	std::ostream& os;
};
/*
auto UploadManyFiles(UploadAction::Pointer upload_action, 
		const FileSystemPathList& file_path_list) -> void {
	for(const auto& file_path : file_path_list){
		upload_action->UploadFile(file_path);
	}
}
*/
}

