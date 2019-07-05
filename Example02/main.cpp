#include <Windows.h>
#include "DemoApp.hpp"

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
{
	try
	{
		DemoApp demo;
		demo.startDemo();
	}
	catch (std::exception& e)
	{
		MessageBox(NULL, e.what(), "An exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
	}

	return 0;
}