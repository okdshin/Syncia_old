#pragma once
//Routine:20121007
#include <iostream>
#include <boost/filesystem.hpp>
#include "../neuria/Neuria.h"
#include "../FileSystemPath.h"

namespace syncia{
namespace filesystem{

using Time = neuria::Time;

auto ForEachFilesInDirectory(const FileSystemPath& dir_path, 
		boost::function<void(const FileSystemPath&)> func) -> void{
	const auto end = boost::filesystem::recursive_directory_iterator();
	for(auto path_iter = boost::filesystem::recursive_directory_iterator(
			dir_path); path_iter != end; ++path_iter){
		if(boost::filesystem::is_regular_file(path_iter->status())){
			const auto file_path = FileSystemPath(*path_iter);
			func(file_path);
		}
	}	
	
}

auto IsExist(const FileSystemPath& file_path) -> bool {
	return boost::filesystem::exists(file_path);	
}

auto GetLastChangeTime(const FileSystemPath& file_path) -> Time {
	return boost::posix_time::from_time_t(
		boost::filesystem::last_write_time(file_path));
}

}
}

