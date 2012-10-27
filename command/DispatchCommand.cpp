#ifdef DISPATCHCOMMAND_UNIT_TEST
#include "DispatchCommand.h"
#include <iostream>

using namespace syncia;
using namespace syncia::command;

int main(int argc, char* argv[])
{
	auto command = DispatchCommand(CommandId("command_id"), 
		neuria::utility::String2ByteArray("byte array"));
	std::cout << command << std::endl;
	std::cout << neuria::utility::ByteArray2String(command.Serialize()) << std::endl;
	std::cout << "parsed:\n" << DispatchCommand::Parse(command.Serialize()) << std::endl;

    return 0;
}

#endif
