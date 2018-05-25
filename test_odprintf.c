#define UNICODE
#include "odprintf.h"

int main(void)
{
	OutputDebugPrintf(TEXT("Test ") PRITS TEXT(" %d"), TEXT("test1"), sizeof(TCHAR));
	OutputDebugPrintfA("Test " PRIAS " %d", "test1", sizeof(char));
	OutputDebugPrintfW(L"Test " PRIWS " %d", L"test1", sizeof(WCHAR));
	return 0;
}
