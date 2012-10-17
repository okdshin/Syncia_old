#pragma once
//Common:20120913
#include <iostream>
#include "DispatchCommand.h"

namespace syncia{
namespace command{

template<class Command>
auto GetCommandId() -> CommandId {
	return CommandId("not registered function");	
}

template<class Command>
auto WrapWithDispatchCommand(const Command& command) -> DispatchCommand {
	return DispatchCommand(GetCommandId<Command>(), command.Serialize());
}

}
}
