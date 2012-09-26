#ifdef REQUESTFILEBEHAVIOR_UNIT_TEST
#include "RequestFileBehavior.h"
#include <iostream>

using namespace sy;

int main(int argc, char* argv[])
{
	std::cout << nr::utl::ByteArray2String(nr::db::SerializeFile("Syncia.out", 128)) << std::endl;

    return 0;
}

#endif
