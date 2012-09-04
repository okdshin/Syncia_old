#ifdef JSONPARSER_UNIT_TEST
#include "JsonParser.h"
#include <iostream>

using namespace sy;

int main(int argc, char* argv[])
{
	auto json_str = nr::utl::String2ByteArray(
			"{\"command\":\"check_version\", \"version\":\"0.1\"}");

	auto json_parser = JsonParser("command");
	std::cout << json_parser.Parse(json_str) << std::endl;

    return 0;
}

#endif
