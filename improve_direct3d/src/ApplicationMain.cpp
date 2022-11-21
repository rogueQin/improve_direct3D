
#include <windows.h>
#include "BoxApp.h"

#include <wincon.h>
#pragma warning(disable:4996)

void InitConsoleWindow() 
{
	AllocConsole();
	freopen("CONOUT$", "w+t", stdout);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow) 
{
	InitConsoleWindow();
	BoxApp theApp(hInstance, nCmdShow);

	if (!theApp.Initlize())
		return 0;

	return theApp.Run();
}
