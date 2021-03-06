#ifdef SEARCHKEYHASHQUERYCOMMAND_UNIT_TEST
#include "SearchKeyHashQueryCommand.h"
#include <iostream>
#include <sstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

using namespace syncia;
using namespace syncia::command;

template<class Type>
void TestSerialize(const Type& target)
{
	std::cout << "src:" << target << std::endl;

	std::stringstream ss;
	boost::archive::text_oarchive oa(ss);
	oa << (const Type&&)target;
	std::cout << "serialized:" << ss.str() << std::endl;

	auto dst_target = Type();
	boost::archive::text_iarchive ia(ss);
	ia >> dst_target;
	std::cout << "dst:" << dst_target << std::endl;
	
}

int main(int argc, char* argv[])
{
	auto command = SearchKeyHashQueryCommand({"hello", "world"});
	command.AddFindKeyHashList({nr::db::FileKeyHash(
		nr::db::FileKeyHash::HashId("hash_id"), 
		nr::db::FileKeyHash::Keyward("keyward"), 
		nr::NodeId("owner_id"), boost::filesystem::path("./"))});
	command.AddRouteNodeId("owner_id");
	//TestSerialize(command);
	auto byte_array = command.Serialize();
	std::cout << SearchKeyHashQueryCommand::Parse(byte_array) << std::endl;

    return 0;
}

#endif
