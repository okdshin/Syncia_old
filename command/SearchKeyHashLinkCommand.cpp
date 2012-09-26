#ifdef SEARCHKEYHASHLINKCOMMAND_UNIT_TEST
#include "SearchKeyHashLinkCommand.h"
#include <iostream>
#include <sstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

using namespace sy;
using namespace sy::cmd;

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
	auto command = SearchKeyHashLinkCommand();
	auto byte_array = command.Serialize();
	std::cout << SearchKeyHashLinkCommand::Parse(byte_array) << std::endl;

    return 0;
}

#endif
