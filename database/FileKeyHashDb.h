#pragma once
//FILEKEYHASHDB:20120831
#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <iterator>
#include <algorithm>
#include <boost/format.hpp>
#include "../neuria/Neuria.h"
#include "FileKeyHash.h"

namespace syncia{
namespace database{


using FileKeyHashList = std::vector<FileKeyHash>;
class IsErasedType{};
using IsErased = neuria::utility::TypeWrapper<bool, IsErasedType>;

class FileKeyHashDb{
public:
	using Pointer = boost::shared_ptr<FileKeyHashDb>;
	using ApplyFunc = boost::function<void (FileKeyHash&)>;
	using IsErasedDecider = boost::function<IsErased (const FileKeyHash&)>;
	static auto Create(double threshold, int buffer_size, 
			std::ostream& os) -> Pointer {
		return Pointer(new FileKeyHashDb(threshold, buffer_size, os));	
	}

	auto Add(FileKeyHash key_hash) -> void {
		auto is_equal_to_key_hash = 
			[&key_hash](const FileKeyHash& e) -> bool {
				return key_hash.GetHashId()() == e.GetHashId()()
					&& key_hash.GetOwnerId() == e.GetOwnerId(); };

		// when same owner_id and hash_id has already existed.
		if(std::find_if(this->hash_list.begin(), this->hash_list.end(), 
				is_equal_to_key_hash) != this->hash_list.end()){
			std::replace_if(this->hash_list.begin(), this->hash_list.end(), 
				is_equal_to_key_hash, key_hash);
		}
		else{
			key_hash.SetBirthTimeNow();
			this->hash_list.push_back(key_hash);
		}
	}

	auto Add(const FileKeyHashList& key_hash_list) -> void {
		for(auto& key_hash : key_hash_list){
			this->Add(key_hash);
		}
	}
	
	auto Add(const Keyword& keyword, 
			const FileSystemPath& file_path, 
			const neuria::network::NodeId& node_id) -> void {
		this->Add(FileKeyHash(HashId(CalcHashId(
				SerializeFile(file_path, this->buffer_size))), 
			keyword, node_id, file_path));
	}
/*
	auto UpdateBirthTime() -> void {
		for(auto& key_hash : this->hash_list){
			key_hash.SetBirthTimeNow();
		}	
	}
*/
	auto Apply(ApplyFunc apply_func) -> void {
		std::for_each(this->hash_list.begin(), this->hash_list.end(), apply_func);
	}

	auto Erase(IsErasedDecider decider) -> void {
		this->hash_list.erase(
			std::remove_if(this->hash_list.begin(), this->hash_list.end(), 
				[decider](const FileKeyHash& key_hash) -> bool {
					return decider(key_hash)();
				}),
			this->hash_list.end()
		);
	}

	auto Search(const KeywordList& search_keyword_list) -> FileKeyHashList {
		for(const auto& key_hash : this->hash_list){
			this->os << key_hash.GetKeyword() << "|";
			for(const auto& keyword : search_keyword_list()){
				this->os << keyword << " ";	
			}
			/*
			this->os << "/similarity:" 
				<< CalcSimilarity(
					search_keyword_list, key_hash.GetKeyword()) << std::endl;
			*/
		}
		auto end = std::partition(this->hash_list.begin(), this->hash_list.end(), 
			[&](const FileKeyHash& key_hash){
				return CalcSimilarity(
					search_keyword_list, key_hash.GetKeyword()) > this->threshold;	
			});
		return FileKeyHashList(this->hash_list.begin(), end);
	}

	auto Get(const HashId& hash_id) -> FileKeyHash {
		auto found = std::find_if(this->hash_list.begin(), this->hash_list.end(), 
			[&hash_id](const FileKeyHash& key_hash){
				return key_hash.GetHashId()() == hash_id(); });	
		if(found == this->hash_list.end()){
			assert(!"not found");	
		}
		return *found;
	}

	auto GetNewer(unsigned int max_count) -> FileKeyHashList {
		if(max_count > this->hash_list.size()){
			return this->hash_list;	
		}

		std::nth_element(this->hash_list.begin(), 
			this->hash_list.begin()+max_count, 
			this->hash_list.end(), 
			[](const FileKeyHash& left, const FileKeyHash& right){
				return left.GetBirthTime() > right.GetBirthTime();	
			});
		auto newer_list = FileKeyHashList();
		std::copy(this->hash_list.begin(), 
			this->hash_list.begin()+max_count, std::back_inserter(newer_list));
		return newer_list;
	}

	auto Erase(const HashId& hash_id) -> void {
		this->hash_list.erase(
			std::remove_if(this->hash_list.begin(), this->hash_list.end(), 
				[&hash_id](const FileKeyHash& key_hash){
					return key_hash.GetHashId()() == hash_id(); }), 
			this->hash_list.end());	
	}

	auto Erase(const neuria::ByteArray& src_data) -> void {
		this->hash_list.erase(
			std::remove_if(this->hash_list.begin(), this->hash_list.end(), 
				[&src_data](const FileKeyHash& key_hash)
					{ return key_hash.GetHashId()() == CalcHashId(src_data)(); }), 
			this->hash_list.end());	
	}

	auto Erase(const FileSystemPath& file_path) -> void {
		this->hash_list.erase(
			std::remove_if(this->hash_list.begin(), this->hash_list.end(), 
				[&file_path](const FileKeyHash& key_hash)
					{ return key_hash.GetFilePath() == file_path; }), 
			this->hash_list.end());		
	}

	auto Clear() -> void {
		this->hash_list.clear();	
	}

private:
	friend auto operator<<(std::ostream& os, 
		const FileKeyHashDb::Pointer file_key_hash_db) -> std::ostream&;

	FileKeyHashDb(double threshold, int buffer_size, std::ostream& os)
		:hash_list(), threshold(threshold), buffer_size(buffer_size), os(os){}

	FileKeyHashList hash_list;
	double threshold;
	int buffer_size;
	std::ostream& os;
};

inline auto operator<<(std::ostream& os, 
		const FileKeyHashDb::Pointer file_key_hash_db) -> std::ostream& {
	for(const auto& key_hash : file_key_hash_db->hash_list){	
		os << key_hash << std::endl;
	}

	return os;
}

}
}
