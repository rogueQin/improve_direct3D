

#include <windows.h>
#include "D3DApp.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow) 
{
	D3DApp theApp = D3DApp(hInstance, nCmdShow);

	if (!theApp.Initlize())
		return 0;

	return theApp.Run();
}
