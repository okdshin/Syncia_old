#ifdef FILEKEYHASHDB_UNIT_TEST
#include "FileKeyHashDb.h"
#include <iostream>
#include "../neuria/utility/Utility.h"

using namespace syncia;
using namespace syncia::database;

int main(int argc, char* argv[])
{
	auto database = FileKeyHashDb::Create(0.3, 128, std::cout);
	return 0;
}

#endif
