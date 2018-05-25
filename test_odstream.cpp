#include "odstream.hpp"

int main(void)
{
	ODS_NOFLUSH(<< "Test1");
	ODS(<< "Test2");
	*yak::debug::ods() << "Test3"; yak::debug::ods_flush();
	WODS_NOFLUSH(<< L"Test1");
	WODS(<< L"Test2");
	*yak::debug::ods<wchar_t>() << L"Test3"; yak::debug::ods_flush<wchar_t>();
	return 0;
}
