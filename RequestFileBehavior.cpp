#ifdef REQUESTFILEBEHAVIOR_UNIT_TEST
#include "RequestFileBehavior.h"
#include <iostream>

using namespace syncia;

int main(int argc, char* argv[])
{
	std::cout << neuria::utility::ByteArray2String(database::SerializeFile("Syncia.out", 128)) << std::endl;

    return 0;
}

#endif
