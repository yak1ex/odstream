#include "odstream.hpp"

int main(void)
{
	ODS_NOFLUSH(<< "Test1");
	ODS(<< "Test2");
	yak::debug::ods() << "Test3"; yak::debug::ods().flush();
	return 0;
}
