#include <Windows.h>
#include "DemoApp.hpp"

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR strCmdLine, INT nCmdShow)
{
	DemoApp demo;
	try
	{
		demo.startDemo();
	}
	catch (std::exception& e)
	{
		MessageBox(NULL, e.what(), "An exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
	}

	return 0;
}