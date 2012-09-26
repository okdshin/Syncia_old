#ifdef UPLOADACTION_UNIT_TEST
#include "UploadAction.h"
#include <iostream>
#include "neuria/utility/Shell.h"
#include "neuria/utility/LabeledSink.h"

using namespace sy;

int main(int argc, char* argv[])
{
	int local_port = 54321;
	if(argc == 2)
	{
		local_port = boost::lexical_cast<int>(argv[1]);
	}

	unsigned int buffer_size = 128;
	auto node_id = nr::utl::CreateSocketNodeId("127.0.0.1", local_port);
	
	auto shell = nr::utl::Shell(std::cout);
	nr::utl::RegisterExitFunc(shell);
	auto file_db = nr::db::FileKeyHashDb::Create(0.3, buffer_size, std::cout);
	shell.Register("db", ": list up db contains", 
		[file_db](const nr::utl::Shell::ArgumentList& argument_list){
			std::cout << file_db << std::endl;
		});
	auto upload_action = UploadAction::Create(file_db, node_id, std::cout);
	shell.Register("upf", "<filepath> (<keyward>): upload file.", 
		[upload_action](const nr::utl::Shell::ArgumentList& argument_list){
			auto file_path = boost::filesystem::path(argument_list.at(1));
			nr::db::Keyward::WrappedType keyward;
			if(argument_list.size() == 2){
				keyward = file_path.filename().string();
			}
			else{
				keyward = argument_list.at(2);
			}
			upload_action->UploadFile(nr::db::Keyward(keyward), file_path);
		});
	shell.Register("updir", "<directorypath>: upload directory.", 
		[upload_action](const nr::utl::Shell::ArgumentList& argument_list){
			upload_action->UploadDirectory(boost::filesystem::path(argument_list.at(1)));
		});
	shell.Start();

    return 0;
}

#endif
