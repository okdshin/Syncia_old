#pragma once
//UploadAction:20120913
#include <iostream>
#include <boost/filesystem.hpp>
#include "neuria/Neuria.h"

namespace sy
{

class UploadAction : public boost::enable_shared_from_this<UploadAction> {
public:
	using Pointer = boost::shared_ptr<UploadAction>;

	static auto Create(nr::db::FileKeyHashDb::Pointer file_db, 
			const nr::NodeId& node_id, 
			std::ostream& os) -> Pointer {	
		return Pointer(new UploadAction(file_db, node_id, os));
	}

	auto UploadFile(const nr::db::Keyward& keyward, 
			const boost::filesystem::path& file_path) -> void {
		std::cout << "SelfNodeId: " << this->node_id << std::endl;
		this->file_db->Add(keyward, file_path, this->node_id);
	}
	
	auto UploadDirectory(const boost::filesystem::path& upload_directory_path) -> void {
		const auto end = boost::filesystem::recursive_directory_iterator();
		for(auto path_iter = boost::filesystem::recursive_directory_iterator(
				upload_directory_path); path_iter != end; ++path_iter){
			if(boost::filesystem::is_regular_file(path_iter->status())){
				const auto file_path = boost::filesystem::path(*path_iter);
				this->UploadFile(
					nr::db::Keyward(file_path.filename().string()), file_path);
			}
		}
	}

private:
    UploadAction(nr::db::FileKeyHashDb::Pointer file_db, const nr::NodeId& node_id,
			std::ostream& os)
			: file_db(file_db), node_id(node_id), os(os){}
	
	nr::db::FileKeyHashDb::Pointer file_db;
	nr::NodeId node_id;
	std::ostream& os;
};

}

