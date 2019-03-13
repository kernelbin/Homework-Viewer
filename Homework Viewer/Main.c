#include<Windows.h>
#include"EasyWindow.h"
#include"Global.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int iCmdShow)
{
	MSG          msg;
	InitEZWindow();
	if (SettingsRead() != 0)
	{
		APP_COLOR = RGB(20, 122, 204);
	}
	//
	GDIObjectCreate();

	Config = CreateEZParentWindow(
		(GetSystemMetrics(SM_CXSCREEN) - CONF_WND_X) / 2,              // initial x position
		(GetSystemMetrics(SM_CYSCREEN) - CONF_WND_Y) / 2,              // initial y position
		CONF_WND_X,              // initial x size
		CONF_WND_Y,              // initial y size)
		WS_POPUP,
		ConfigProc,
		0, 0);

	ShowWindow(Config->hParent, iCmdShow);
	UpdateWindow(Config->hParent);
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	GDIObjectClean();

	return 0;
}