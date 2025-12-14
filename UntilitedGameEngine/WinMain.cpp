#include "App.h"
#include <Windows.h>
#include <exception>
#include <iostream>

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nShowCmd)
{
	try
	{
		App app;
		return app.Go();
	}
	catch (const Expection& e)
	{
		MessageBoxA(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONERROR);
	}
	catch (const std::exception& e)
	{
		MessageBoxA(nullptr, e.what(), "Standard Expection", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBoxA(nullptr, "No details", "Unknown Expectation", MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;
}