#pragma once
//Routine:20121011
#include <iostream>
#include "../neuria/Neuria.h"

namespace syncia{
namespace database{

class HashIdType{};
using HashId = neuria::utility::TypeWrapper<std::string, HashIdType>;

class KeywardType{};
using Keyward = neuria::utility::TypeWrapper<std::string, KeywardType>;

auto CalcHashId(const neuria::ByteArray& src_data) -> HashId {
	return HashId(neuria::hash::CalcHashStr(src_data));	
}

class KeywardListType{};
using KeywardList = 
	neuria::utility::TypeWrapper<std::vector<Keyward::WrappedType>, KeywardListType>;

auto CalcSimilarity(const std::string& left, const std::string& right) -> double {
	double shorter_length = 
		left.length() < right.length() ? left.length() : right.length();
	double longer_length = 
		left.length() > right.length() ? left.length() : right.length();
	auto str = left.length() < right.length() ? left : right;
	auto similarity_unit = shorter_length*shorter_length/longer_length;
	return (std::string::npos != left.find(right)) 
		|| (std::string::npos != right.find(left)) 
			? shorter_length*shorter_length/longer_length : -similarity_unit;	
}

auto CalcSimilarity(const std::vector<std::string>& search_keyward_list, 
		const std::string& target_keyward) -> double {
	double similarity = 0.0;
	for(const auto& search_keyward : search_keyward_list){
		similarity += CalcSimilarity(search_keyward, target_keyward);
	}
	return similarity
		/ std::max_element(search_keyward_list.begin(), search_keyward_list.end(), 
			[](const std::string& left, const std::string& right)
				{ return left.size() < right.size(); }
			)->size();
}

auto CalcSimilarity(const KeywardList& keyward_list, 
		const Keyward& keyward) -> double{
	return CalcSimilarity(keyward_list(), keyward());
}

}
}
