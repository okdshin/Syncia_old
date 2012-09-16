#ifdef REQUESTFILEQUERYBEHAVIOR_UNIT_TEST
#include "RequestFileQueryBehavior.h"
#include <iostream>

using namespace sy;

int main(int argc, char* argv[])
{
	std::cout << nr::utl::ByteArray2String(SerializeFile("Syncia.out", 128)) << std::endl;

    return 0;
}

#endif
