#ifdef ADDREMOVEFILEINDIRECTORYWATCHER_UNIT_TEST
#include "AddRemoveFileInDirectoryWatcher.h"
#include <iostream>

using namespace syncia;
using namespace syncia::filesystem;

int main(int argc, char* argv[])
{
	auto watcher = AddRemoveFileInDirectoryWatcher::Create(FileSystemPath("./test/"));

	while(true){
		sleep(1);
		watcher->Check();
		watcher->Call();
		watcher->Update();
		
	}

    return 0;
}

#endif
