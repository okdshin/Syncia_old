#pragma once
//Common:20120913
#include <iostream>

namespace sy{
namespace cmd{

template<class Command>
auto GetCommandId() -> nr::ntw::DispatchCommand::CommandId {
	return nr::ntw::DispatchCommand::CommandId("not registered function");	
}

}
}
