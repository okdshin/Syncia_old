#pragma once
//FileKeyHash:20120903
#include <iostream>
#include <string>
#include <boost/serialization/string.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/format.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "../neuria/Neuria.h"
#include "../FileSystemPath.h"
#include "Routine.h"

namespace syncia{
namespace database{

auto SerializeFile(const FileSystemPath& file_path, 
		int buffer_size) -> neuria::ByteArray {
	boost::filesystem::ifstream ifs(file_path, std::ios::binary);
	auto whole_byte_array = neuria::ByteArray();
	auto part_byte_array = neuria::ByteArray(buffer_size);
	int count = 0;
	while(ifs && !ifs.eof()){
		++count;
		const auto read_size = ifs.readsome(
			static_cast<char*>(static_cast<void*>(&part_byte_array.front())), 
			buffer_size);
		std::copy(part_byte_array.begin(), part_byte_array.begin()+read_size, 
			std::back_inserter(whole_byte_array));
		//if(read_size < buffer_size){ break; }
	}
	ifs.close();
	std::cout << count*buffer_size << " bytes." << std::endl;
	return whole_byte_array;
}

class FileKeyHash{
public:
   	FileKeyHash(){}
	FileKeyHash(const HashId& hash_id, const Keyword& keyword, 
			const neuria::network::NodeId& owner_id, const FileSystemPath& file_path) 
		:hash_id(hash_id()), keyword(keyword()), owner_id(owner_id), 
		file_path_str(file_path.string()){
		this->SetBirthTimeNow();
	}
	
	auto GetHashId() const -> HashId { return HashId(hash_id); }
	auto GetKeyword() const -> Keyword { return Keyword(keyword); }
	auto GetOwnerId() const -> neuria::network::NodeId { return owner_id; }
	auto GetFilePath() const -> FileSystemPath { 
		return FileSystemPath(this->file_path_str); }
	auto GetBirthTime() const -> boost::posix_time::ptime { 
		return boost::posix_time::time_from_string(this->birth_universal_time_str); }

	auto SetBirthTimeNow() -> void {
		this->birth_universal_time_str 
			= boost::posix_time::to_simple_string(
				boost::posix_time::second_clock::universal_time());	
	}

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, unsigned int ver){	
		std::cout << "FileKeyHash serializing..." << std::endl;
		ar & hash_id & keyword & owner_id 
			& file_path_str & birth_universal_time_str;
		std::cout << "FileKeyHash serializing finished!!" << std::endl;
	}
	
	HashId::WrappedType hash_id;
	Keyword::WrappedType keyword;
	neuria::network::NodeId owner_id;
	std::string file_path_str;
	std::string birth_universal_time_str;
};

auto CreateTestFileKeyHash() -> FileKeyHash {
	return FileKeyHash(HashId("hash_id"), Keyword("keyword"), 
		neuria::network::NodeId("owner_node_id"), "./test_file.dat");
}

auto IsFileExist(const FileKeyHash& key_hash) -> bool {
	boost::filesystem::ifstream ifs(key_hash.GetFilePath());
	return static_cast<bool>(ifs);
}

auto operator <<(std::ostream& os, const FileKeyHash& key_hash) -> std::ostream& {
	os << boost::format(
		"Keyword:%1%, HashId:%2%, OwnerId:%3%, FilePath:%4%, BirthUniversalTime:%5%") 
		% key_hash.GetKeyword() 
		% key_hash.GetHashId() 
		% key_hash.GetOwnerId() 
		% key_hash.GetFilePath() 
		% boost::posix_time::to_simple_string(key_hash.GetBirthTime());
	return os;
}

//for python binding
inline auto operator==(const syncia::database::FileKeyHash& left, 
		const syncia::database::FileKeyHash& right) -> bool {
	return false;	
}

inline auto operator!=(const syncia::database::FileKeyHash& left, 
		const syncia::database::FileKeyHash& right) -> bool {
	return true;	
}


}
}
