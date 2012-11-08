#ifdef UPLOADACTION_UNIT_TEST
#include "UploadAction.h"
#include <iostream>
#include "neuria/test/CuiShell.h"
#include "neuria/utility/LabeledSink.h"

using namespace syncia;

int main(int argc, char* argv[])
{
	int local_port = 54321;
	if(argc == 2)
	{
		local_port = boost::lexical_cast<int>(argv[1]);
	}

	unsigned int buffer_size = 128;
	auto node_id = neuria::network::CreateSocketNodeId("127.0.0.1", local_port);
	
	auto shell = neuria::test::CuiShell(std::cout);
	neuria::test::RegisterExitFunc(shell);
	auto file_db = database::FileKeyHashDb::Create(0.3, buffer_size, std::cout);
	shell.Register("db", ": list up db contains", 
		[file_db](const neuria::test::CuiShell::ArgList& argument_list){
			std::cout << file_db << std::endl;
		});
	auto upload_action = UploadAction::Create(file_db, node_id, std::cout);
	shell.Register("upf", "<filepath> (<keyward>): upload file.", 
		[upload_action](const neuria::test::CuiShell::ArgList& argument_list){
			auto file_path = boost::filesystem::path(argument_list.at(1));
			database::Keyword::WrappedType keyword;
			if(argument_list.size() == 2){
				keyword = file_path.filename().string();
			}
			else{
				keyword = argument_list.at(2);
			}
			upload_action->UploadFile(file_path);
		});
	shell.Register("updir", "<directorypath>: upload directory.", 
		[upload_action](const neuria::test::CuiShell::ArgList& argument_list){
			//upload_action->UploadDirectory(boost::filesystem::path(argument_list.at(1)));
		});
	shell.Start();

    return 0;
}

#endif
