#pragma once
//AddRemoveFileInDirectoryWatcher:20121007
#include <iostream>
#include "Routine.h"
#include "../FileSystemPathList.h"

namespace syncia{
namespace filesystem{

class AddRemoveFileInDirectoryWatcher{
public:
	using OnAddedFileListFunc = boost::function<void (const FileSystemPathList&)>;
	using OnRemovedFileListFunc = boost::function<void (const FileSystemPathList&)>;

    AddRemoveFileInDirectoryWatcher(const FileSystemPath& dir_path)
			:dir_path(dir_path){
		this->Check();	
		this->SetOnAddedFileListFunc([](const FileSystemPathList& file_path_list){
				for(const auto& file_path : file_path_list){
					std::cout << file_path << " was added" << std::endl;	
				}
			}
		);
		this->SetOnRemovedFileListFunc([](const FileSystemPathList& file_path_list){
				for(const auto& file_path : file_path_list){
					std::cout << file_path << " was removed" << std::endl;	
				}
			}
		);
	}

	auto SetOnAddedFileListFunc(OnAddedFileListFunc on_added_file_list_func) -> void {
		this->on_added_file_list_func = on_added_file_list_func;
	}

	auto SetOnRemovedFileListFunc(
			OnRemovedFileListFunc on_removed_file_list_func) -> void {
		this->on_removed_file_list_func = on_removed_file_list_func;	
	}

	auto Check() -> void {
		assert(IsExist(this->dir_path));
		this->newer_file_path_list = this->GetCurrentFileListInDirectory();		
	}

	auto Call() -> void {
		this->on_added_file_list_func(this->GetFileListInFormerNotContainedByLater(
			this->newer_file_path_list, this->older_file_path_list));
		this->on_removed_file_list_func(this->GetFileListInFormerNotContainedByLater(
			this->older_file_path_list, this->newer_file_path_list));	
	}

	auto Update() -> void {
		this->older_file_path_list = this->newer_file_path_list;	
	}

private:
	auto GetFileListInFormerNotContainedByLater(
			FileSystemPathList former, 
			const FileSystemPathList& later) -> FileSystemPathList {
		auto is_in_later = [&later](const FileSystemPath& e){
				return later.end() != std::find(later.begin(), later.end(), e);
			};
		auto end = std::remove_if(former.begin(), former.end(), is_in_later);

		return FileSystemPathList(former.begin(), end);	
	}

	auto GetCurrentFileListInDirectory() -> FileSystemPathList {
		FileSystemPathList file_path_list;
		ForEachFilesInDirectory(this->dir_path, 
			[&file_path_list](const FileSystemPath& file_path){ 
				file_path_list.push_back(file_path); 
			}
		);
		return file_path_list;
	}

	FileSystemPath dir_path;
	FileSystemPathList older_file_path_list;
	FileSystemPathList newer_file_path_list;

	OnAddedFileListFunc on_added_file_list_func;
	OnRemovedFileListFunc on_removed_file_list_func;

};

void SetOnAddedFileFunc(AddRemoveFileInDirectoryWatcher& watcher, 
		boost::function<void (const FileSystemPath&)> on_added_file_func){
	watcher.SetOnAddedFileListFunc(
		[on_added_file_func](const FileSystemPathList& file_path_list){
			for(const auto& file_path : file_path_list){
				on_added_file_func(file_path);
			}
		}
	);
}

void SetOnRemovedFileFunc(AddRemoveFileInDirectoryWatcher& watcher, 
		boost::function<void (const FileSystemPath&)> on_removed_file_func){
	watcher.SetOnRemovedFileListFunc(
		[on_removed_file_func](const FileSystemPathList& file_path_list){
			for(const auto& file_path : file_path_list){
				on_removed_file_func(file_path);
			}
		}
	);
}

}
}

