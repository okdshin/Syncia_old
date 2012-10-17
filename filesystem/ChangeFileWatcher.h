#pragma once
//ChangeFileWatcher:20121007
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "../neuria/Neuria.h"
#include "../FileSystemPath.h"
#include "Routine.h"

namespace syncia{
namespace filesystem{
	

class ChangeFileWatcher{
public:
	using OnFileChangedFunc = boost::function<void (const FileSystemPath&)>;
	
	explicit ChangeFileWatcher(const FileSystemPath& file_path)
			:file_path(file_path){
		this->older_change_time = GetLastChangeTime(file_path);
		this->on_file_changed_func = [this](const FileSystemPath&){
			std::cout << this->file_path << " was changed." << std::endl;};
	}
	
	auto SetOnFileChangedFunc(OnFileChangedFunc on_file_changed_func) -> void {
		this->on_file_changed_func = on_file_changed_func;
	}
	
	auto Check() -> void {
		assert(IsExist(this->file_path));
		this->newer_change_time = GetLastChangeTime(this->file_path);
	}

	auto Call() -> void {

		if(this->older_change_time != newer_change_time){
			this->on_file_changed_func(this->file_path);
		}
	}

	auto Update() -> void {
		this->older_change_time = this->newer_change_time;		
	}

private:
	OnFileChangedFunc on_file_changed_func;

	FileSystemPath file_path;
	Time newer_change_time;
	Time older_change_time;
};


}
}

