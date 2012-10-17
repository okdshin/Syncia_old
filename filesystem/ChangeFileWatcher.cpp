#ifdef CHANGEFILEWATCHER_UNIT_TEST
#include "ChangeFileWatcher.h"
#include <iostream>
#include <fstream>

using namespace syncia;
using namespace syncia::filesystem;

int main(int argc, char* argv[])
{
	const auto file_path = FileSystemPath("./test_file.dat");
	auto watcher = ChangeFileWatcher(file_path);
	watcher.SetOnFileChangedFunc([](const FileSystemPath& file_path){
			std::cout << file_path << "was changed!" << std::endl;
		}
	);
	
	while(true)
	{
		sleep(1);
		if(!IsExist(file_path)){
			std::cout << file_path << " was removed." << std::endl;
			break;	
		}
		watcher.Check();
		watcher.Call();
		watcher.Update();
	}

    return 0;
}

#endif
