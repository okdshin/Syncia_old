#pragma once
//Routine:20121011
#include <iostream>
#include "../neuria/Neuria.h"

namespace syncia{
namespace database{

class HashIdType{};
using HashId = neuria::utility::TypeWrapper<std::string, HashIdType>;

class KeywordType{};
using Keyword = neuria::utility::TypeWrapper<std::string, KeywordType>;

auto CalcHashId(const neuria::ByteArray& src_data) -> HashId {
	return HashId(neuria::hash::CalcHashStr(src_data));	
}

class KeywordListType{};
using KeywordList = 
	neuria::utility::TypeWrapper<std::vector<Keyword::WrappedType>, KeywordListType>;

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

auto CalcSimilarity(const std::vector<std::string>& search_keyword_list, 
		const std::string& target_keyword) -> double {
	double similarity = 0.0;
	for(const auto& search_keyword : search_keyword_list){
		similarity += CalcSimilarity(search_keyword, target_keyword);
	}
	return similarity
		/ std::max_element(search_keyword_list.begin(), search_keyword_list.end(), 
			[](const std::string& left, const std::string& right)
				{ return left.size() < right.size(); }
			)->size();
}

auto CalcSimilarity(const KeywordList& keyword_list, 
		const Keyword& keyword) -> double{
	return CalcSimilarity(keyword_list(), keyword());
}

}
}
