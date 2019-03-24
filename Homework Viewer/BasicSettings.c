#include <Windows.h>
#include<tchar.h>
#include<StrSafe.h>
#include "Global.h"
#include "EasyWindow.h"
#include<math.h>
#include"GaussianBlur.h"
#include<WinInet.h>


EZWND AccountPage, AppearancePage,UpdatePage;


#define COLOR_GRID_WIDTH 8
#define COLOR_GRID_HEIGHT 6

COLORREF ColorGrid[COLOR_GRID_HEIGHT][COLOR_GRID_WIDTH] = 
{
	RGB(255,185,0),RGB(255,140,0),RGB(247,99,12),RGB(202,80,16),RGB(218,59,1),RGB(239,105,80),RGB(209,52,56),RGB(255,67,67),
	RGB(231,72,86),RGB(232,17,35),RGB(234,0,94),RGB(195,0,82),RGB(227,0,140),RGB(191,0,119),RGB(194,57,179),RGB(154,0,137),
	RGB(0,120,215),RGB(0,99,177),RGB(142,140,216),RGB(107,105,214),RGB(135,100,184),RGB(166,77,169),RGB(177,70,194),RGB(136,23,152),
	RGB(0,153,188),RGB(45,125,154),RGB(0,183,195),RGB(3,131,135),RGB(0,178,148),RGB(1,133,116),RGB(0,204,106),RGB(16,137,62),
	RGB(122,117,116),RGB(93,90,88),RGB(104,118,138),RGB(81,92,107),RGB(86,124,115),RGB(72,104,96),RGB(73,130,5),RGB(16,124,16),
	RGB(118,118,118),RGB(76,74,72),RGB(105,121,126),RGB(74,84,89),RGB(100,124,100),RGB(82,94,84),RGB(132,117,69),RGB(126,115,95)
};

typedef struct __StngBtn
{
	TCHAR Title[16];
	EZWND* Page;
}SETTINGBTN;

SETTINGBTN StngBtnInfo[] =
{
	TEXT("账号"), &AccountPage,
	TEXT("外观"), &AppearancePage,
	TEXT("更新"),& UpdatePage
};

int SelStng;

EZWND StngBtn[_countof(StngBtnInfo)];

EZWNDPROC SettingsBtnProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam);

EZWNDPROC AccountPageProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam);
EZWNDPROC AppearancePageProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam);
EZWNDPROC ColorBtnProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam);
EZWNDPROC UpdatePageProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam);

EZWNDPROC BasicSettingsProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam)
{
	static int TimerID;
	static HDC ShadowDC;
	static PBYTE pData;
	switch (message)
	{
	case EZWM_CREATE:
		ezWnd->Transparent = 120;
		ShadowDC = GetMemDC(ezWnd->hdc, 220, ezWnd->Height + 100);
		pData = malloc(220 * (ezWnd->Height + 100) * 4);
		TimerID = -1;
		SelStng = 0;

		for (int i = 0; i < _countof(StngBtnInfo); i++)
		{
			StngBtn[i] = CreateEZWindow(ezWnd, i == 0 ? 25 : 35, 48 + i * 74, i == 0 ? 95 : 85, 42, SettingsBtnProc);
			StngBtn[i]->ezID = i;
			EZSendMessage(StngBtn[i], EZWM_USER_NOTIFY, 0, i);
		}

		AccountPage = CreateEZWindow(ezWnd, 120, 0, ezWnd->Width - 120, ezWnd->Height, AccountPageProc);
		AppearancePage = CreateEZWindow(ezWnd, 120, 0, ezWnd->Width - 120, ezWnd->Height, AppearancePageProc);
		UpdatePage = CreateEZWindow(ezWnd, 120, 0, ezWnd->Width - 120, ezWnd->Height, UpdatePageProc);
		AppearancePage->ShowState = 2;
		AppearancePage->MouseMsgRecv = 2;
		UpdatePage->ShowState = 2;
		UpdatePage->MouseMsgRecv = 2;
		break;
	case EZWM_COMMAND://lParam附加按钮ID

		(*(StngBtnInfo[SelStng].Page))->ShowState = 2;
		(*(StngBtnInfo[SelStng].Page))->MouseMsgRecv = 2;

		(*(StngBtnInfo[lParam].Page))->ShowState = 1;
		(*(StngBtnInfo[lParam].Page))->MouseMsgRecv = 1;
		SelStng = lParam;
		if (TimerID == -1)
		{
			TimerID = SetEZTimer(ezWnd, 80);
		}

		break;
	case EZWM_TRANSDRAW:
		PatBlt(ShadowDC, 0, 0, 220, ezWnd->Height + 100, BLACKNESS);
		//修改透明层
		{

			HBITMAP hSel = SelectObject(ShadowDC, CreateCompatibleBitmap(ShadowDC, 1, 1));
			BITMAP bmp;
			GetObject(hSel, sizeof(BITMAP), &bmp);

			memset(pData, 0, (220 * (ezWnd->Height + 100) * 4));
			//operate
			for (int y = 0; y < bmp.bmHeight; y++)
			{
				for (int x = 0; x < bmp.bmWidth; x++)
				{
					pData[(y * bmp.bmWidth + x) * 4 + 3] = 255;
				}
			}
			for (int y = 50; y < bmp.bmHeight - 50; y++)
			{
				for (int x = 50; x < bmp.bmWidth - 50; x++)
				{
					pData[(y * bmp.bmWidth + x) * 4 + 3] = 0;
				}
			}

			for (int i = 0; i < _countof(StngBtnInfo); i++)
			{
				for (int y = 50 + StngBtn[i]->y; y < 50 + StngBtn[i]->y + StngBtn[i]->Height; y++)
				{
					for (int x = 50 + StngBtn[i]->x; x < 50 + StngBtn[i]->x + StngBtn[i]->Width; x++)
					{
						pData[(y * bmp.bmWidth + x) * 4 + 3] = 255;
					}
				}
			}

			AlphaOnlyGaussianBlurFilter(pData, pData, bmp.bmWidth, bmp.bmHeight, bmp.bmWidthBytes, 3);
			SetBitmapBits(hSel, bmp.bmWidth * bmp.bmHeight * 4, pData);


			DeleteObject(SelectObject(ShadowDC, hSel));
		}
		{
			BLENDFUNCTION bf = { 0 };
			bf.AlphaFormat = AC_SRC_ALPHA;
			bf.SourceConstantAlpha = 255;
			AlphaBlend(wParam, 0, 0, 120, ezWnd->Height, ShadowDC, 50, 46, 120, ezWnd->Height, bf);
			//StretchBlt(wParam, 0, 0, ezWnd->Width, ezWnd->Height, hdcBorder, 50, 50, ezWnd->Width, ezWnd->Height, SRCCOPY);
		}
		break;
	case EZWM_TIMER:
		//枚举每个按钮检查位置，都就位了就关计时器
	{
		BOOL bSet = 1;
		for (int i = 0; i < _countof(StngBtnInfo); i++)
		{
			if (SelStng == i)
			{
				//左基准25
				if (StngBtn[i]->x != 25)
				{
					bSet = 0;
					int xPos = floor(25 + 0.6 * (StngBtn[i]->x - 25));
					MoveEZWindow(StngBtn[i], xPos, StngBtn[i]->y, 120 - xPos, StngBtn[i]->Height, 0);
				}

			}
			else
			{
				//左基准35
				if (StngBtn[i]->x != 35)
				{
					bSet = 0;
					int xPos = ceil(35 + 0.85 * (StngBtn[i]->x - 35));
					MoveEZWindow(StngBtn[i], xPos, StngBtn[i]->y, 120 - xPos, StngBtn[i]->Height, 0);
				}
			}
		}
		if (bSet)
		{
			KillEZTimer(ezWnd, TimerID);
			TimerID = -1;
		}
		EZRepaint(ezWnd, 0);

	}
	break;
	case EZWM_DESTROY:
		free(pData);
		DeleteMemDC(ShadowDC);
		break;
	}


	return 0;
}


EZWNDPROC SettingsBtnProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case EZWM_USER_NOTIFY:
	{
		if (wParam == 0)
		{
			EZWND StaticText;
			StaticText = CreateEZStyleWindow(ezWnd, StngBtnInfo[lParam].Title, EZS_CHILD | EZS_STATIC, 0, 0, ezWnd->Width, ezWnd->Height);
			FontForm.lfHeight = ezWnd->Height * (4.0 / 7.0);
			EZSendMessage(StaticText, EZWM_SETFONT, 0, &FontForm);
			EZSendMessage(StaticText, EZWM_SETTEXTALIGN, DT_CENTER | DT_VCENTER | DT_SINGLELINE, 0);
			StaticText->Transparent = 0;
			StaticText->MouseMsgRecv = 2;
			break;
		}
	}
	case EZWM_DRAW:
		PatBlt(wParam, 4, 0, ezWnd->Width - 4, ezWnd->Height, WHITENESS);
		{
			HBRUSH OldBrush;
			OldBrush = SelectObject(wParam, CreateSolidBrush(ezWnd->ezID == SelStng ? APP_COLOR : RGB(170, 170, 170)));
			PatBlt(wParam, 0, 0, 4, ezWnd->Height, PATCOPY);
			DeleteBrush(SelectObject(wParam, OldBrush));
		}
		break;
	case EZWM_LBUTTONUP:
		EZSendMessage(ezWnd->ezParent, EZWM_COMMAND, ezWnd, ezWnd->ezID);
		break;
	}
}


EZWNDPROC AccountPageProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam)
{
	static EZWND TitleStatic, AccountUsageStatic, UserNameText, UserNameEdit = 0, PasswdText, PasswdEdit = 0, SaveBtn;
	switch (message)
	{
	case EZWM_CREATE:
		TitleStatic = CreateEZStyleWindow(ezWnd, TEXT("校园网账号"), EZS_CHILD | EZS_STATIC, 50, 30, ezWnd->Width, 60);
		FontForm.lfHeight = 60 * (4.0 / 7.0);
		EZSendMessage(TitleStatic, EZWM_SETTEXTALIGN, DT_LEFT, 0);
		EZSendMessage(TitleStatic, EZWM_SETFONT, 0, &FontForm);
		TitleStatic->Transparent = 0;

		AccountUsageStatic = CreateEZStyleWindow(ezWnd, TEXT("该账号用于登陆校园网并获取作业，不会用于其他用途。暂不支持教师账号。"), EZS_CHILD | EZS_STATIC, 50, 80, ezWnd->Width, 35);
		FontForm.lfHeight = 35 * (4.0 / 7.0);
		EZSendMessage(AccountUsageStatic, EZWM_SETTEXTALIGN, DT_LEFT, 0);
		EZSendMessage(AccountUsageStatic, EZWM_SETFONT, 0, &FontForm);
		EZSendMessage(AccountUsageStatic, EZWM_SETCOLOR, RGB(0, 0, 0), RGB(160, 160, 160));
		AccountUsageStatic->Transparent = 0;


		FontForm.lfHeight = 30 * (6.0 / 7.0);
		UserNameText = CreateEZStyleWindow(ezWnd, TEXT("用户名:"), EZS_CHILD | EZS_STATIC, 50, 125, 90, 30);
		EZSendMessage(UserNameText, EZWM_SETFONT, 0, &FontForm);
		EZSendMessage(UserNameText, EZWM_SETTEXTALIGN, DT_LEFT | DT_VCENTER | DT_SINGLELINE, 0);
		UserNameText->Transparent = 0;
		PasswdText = CreateEZStyleWindow(ezWnd, TEXT("密码:"), EZS_CHILD | EZS_STATIC, 50, 170, 90, 30);
		EZSendMessage(PasswdText, EZWM_SETFONT, 0, &FontForm);
		EZSendMessage(PasswdText, EZWM_SETTEXTALIGN, DT_LEFT | DT_VCENTER | DT_SINGLELINE, 0);
		PasswdText->Transparent = 0;

		UserNameEdit = CreateEZStyleWindow(ezWnd, TEXT(""), EZS_CHILD | EZS_EDIT | EZES_SINGLELINE, 132, 125, 270, 30);
		EZSendMessage(UserNameEdit, EZWM_SETFONT, 0, &FontForm);
		EZSendMessage(UserNameEdit, EZWM_SETMAXTEXT, 18, 0);
		PasswdEdit = CreateEZStyleWindow(ezWnd, TEXT(""), EZS_CHILD | EZS_EDIT | EZES_SINGLELINE | EZES_PASSWORD, 132, 170, 270, 30);
		EZSendMessage(PasswdEdit, EZWM_SETFONT, 0, &FontForm);
		EZSendMessage(PasswdEdit, EZWM_SETMAXTEXT, 18, 0);

		SaveBtn = CreateEZStyleWindow(ezWnd, TEXT("保存"), EZS_CHILD | EZS_BUTTON, 202, 215, 130, 40);
		EZSendMessage(SaveBtn, EZWM_SETFONT, 0, &FontForm);
		EZSendMessage(SaveBtn, EZWM_SETCOLOR, RGB(0, 0, 0), RGB(0, 0, 0));
		
		EZSendMessage(UserNameEdit, EZWM_SETTEXT, PlatformUsername, 0);
		EZSendMessage(PasswdEdit, EZWM_SETTEXT, PlatformPassword, 0);

		//读取配置，填入
		break;
	case EZWM_COMMAND:
		if (lParam == SaveBtn)
		{
			//写入注册表，写入全局变量。
			lstrcpy(PlatformUsername, UserNameEdit->Extend->Title);
			lstrcpy(PlatformPassword, PasswdEdit->Extend->Title);
			SettingsWrite();
		}
		break;
	case EZWM_DRAW:
		SelectObject(wParam, CreatePen(PS_SOLID | PS_INSIDEFRAME, 2, APP_COLOR));
		SelectObject(wParam, GetStockObject(NULL_BRUSH));

		if (UserNameEdit)
		{
			Rectangle(wParam, UserNameEdit->x - 2, UserNameEdit->y - 2,
				UserNameEdit->x + UserNameEdit->Width + 2, UserNameEdit->y + UserNameEdit->Height + 2);
		}

		if (PasswdEdit)
		{
			Rectangle(wParam, PasswdEdit->x - 2, PasswdEdit->y - 2,
				PasswdEdit->x + PasswdEdit->Width + 2, PasswdEdit->y + PasswdEdit->Height + 2);
		}

		DeleteObject(SelectObject(wParam, GetStockObject(WHITE_BRUSH)));
		DeleteObject(SelectObject(wParam, GetStockObject(BLACK_PEN)));
		break;
	}
}

EZWNDPROC AppearancePageProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam)
{
	static EZWND TitleStatic;
	static EZWND ColorBtn[COLOR_GRID_HEIGHT][COLOR_GRID_WIDTH];
	switch (message)
	{
	case EZWM_CREATE:
		TitleStatic = CreateEZStyleWindow(ezWnd, TEXT("外观设置"), EZS_CHILD | EZS_STATIC, 50, 30, ezWnd->Width, 60);
		FontForm.lfHeight = 60 * (4.0 / 7.0);
		EZSendMessage(TitleStatic, EZWM_SETTEXTALIGN, DT_LEFT, 0);
		EZSendMessage(TitleStatic, EZWM_SETFONT, 0, &FontForm);
		TitleStatic->Transparent = 0;
		for (int y = 0; y < COLOR_GRID_HEIGHT; y++)
		{
			for (int x = 0; x < COLOR_GRID_WIDTH; x++)
			{
				ColorBtn[y][x] = CreateEZWindow(ezWnd, 50 + x * 47, 110 + y * 47, 44, 44, ColorBtnProc);
				ColorBtn[y][x]->ezID = y * COLOR_GRID_WIDTH + x;
				//EZSendMessage(ColorBtn[y][x], EZWM_USER_NOTIFY, 0, 0);
			}
		}
		break;
	case EZWM_COMMAND:
		APP_COLOR = ColorGrid[((EZWND)lParam)->ezID / COLOR_GRID_WIDTH][((EZWND)lParam)->ezID % COLOR_GRID_WIDTH];
		GDIObjectClean();
		GDIObjectCreate();
		EZRepaint(ezWnd->ezRootParent, 0);
		//把颜色往注册表里存
		SettingsWrite();
		break;
	}
}

EZWNDPROC ColorBtnProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case EZWM_DRAW:
	{
		HBRUSH hBrush = CreateSolidBrush(ColorGrid[ezWnd->ezID/ COLOR_GRID_WIDTH][ezWnd->ezID % COLOR_GRID_WIDTH]);
		SelectObject(wParam, hBrush);
		PatBlt(wParam, 0, 0, ezWnd->Width, ezWnd->Height, PATCOPY);
		DeleteBrush(hBrush);
		break;
	}
	case EZWM_LBUTTONUP:
	{
		EZSendMessage(ezWnd->ezParent, EZWM_COMMAND, 0, ezWnd);
		break;
	}
	}
}

EZWNDPROC UpdatePageProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam)
{
	static EZWND TitleStatic, UpdateBtn;
	switch (message)
	{
	case EZWM_CREATE:
		TitleStatic = CreateEZStyleWindow(ezWnd, TEXT("更新"), EZS_CHILD | EZS_STATIC, 50, 30, ezWnd->Width, 60);
		FontForm.lfHeight = 60 * (4.0 / 7.0);
		EZSendMessage(TitleStatic, EZWM_SETTEXTALIGN, DT_LEFT, 0);
		EZSendMessage(TitleStatic, EZWM_SETFONT, 0, &FontForm);
		TitleStatic->Transparent = 0;

		FontForm.lfHeight = 40 * (4.0 / 7.0);
		UpdateBtn = CreateEZStyleWindow(ezWnd, TEXT("检查更新并下载"), EZS_CHILD | EZS_BUTTON, 50, 100, 180, 40);
		EZSendMessage(UpdateBtn, EZWM_SETCOLOR, RGB(0,0,0), RGB(0, 0, 0));
		EZSendMessage(UpdateBtn, EZWM_SETFONT, 0, &FontForm);
		break;
	case EZWM_COMMAND:
	{
		HINTERNET InternetGlobal = 0, InetUpdateCheck = 0,InternetUpdate = 0;
		HFILE UpdateFile = 0;
		DWORD num, tnum = 0;
		BOOL bSuccess = 0;
		BOOL bHaveUpdate = 0;
		__try
		{
			InternetGlobal = InternetOpenW(L"Homework Viewer", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
			if (!InternetGlobal)__leave;

			InetUpdateCheck = InternetOpenUrl(InternetGlobal, TEXT("http://118.25.77.58/HomeworkViewer/update.txt"), 0, 0, INTERNET_FLAG_NEED_FILE, 0);
			if (!InetUpdateCheck)__leave;

			char UpdateVer[16] = { 0 };
			InternetReadFile(InetUpdateCheck, UpdateVer, 10, &num);
			if (!num)
			{
				__leave;
			}
			double ver;
			sscanf_s(UpdateVer, "%lf", &ver);

			if (ver <= lfVersion)
			{
				MessageBox(NULL, TEXT("看上去好像没什么可以更新的  ╮( •́ω•̀ )╭"), szAppName, MB_TASKMODAL);
				bSuccess = 1;
				__leave;
			}
			bHaveUpdate = 1;
			InternetUpdate = InternetOpenUrl(InternetGlobal, TEXT("http://118.25.77.58/HomeworkViewer/Homework%20Viewer.exe"), 0, 0, INTERNET_FLAG_NEED_FILE, 0);
			if (!InternetUpdate)__leave;

			
			CHAR Buffer[1024];
			DWORD Readed;

			//打开文件
			UpdateFile = CreateFile(TEXT("Homework Viewer RC.exe"), GENERIC_ALL, 0, 0, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);
			if (!UpdateFile)__leave;

			BOOL bRet = 0;
			
			DWORD Written;
			do
			{
				bRet = InternetReadFile(InternetUpdate, Buffer, 1024, &num);
				tnum += num;
				WriteFile(UpdateFile, Buffer, num, &Written, 0);

			} while (!(bRet && num == 0));
			bSuccess = 1;

		}
		__finally
		{
			if (InternetGlobal)InternetCloseHandle(InternetGlobal);
			if (InetUpdateCheck)InternetCloseHandle(InetUpdateCheck);
			if (InternetUpdate)InternetCloseHandle(InternetUpdate);

			if (UpdateFile)
			{
				//TODO:句柄关了，文件删掉了没？
				CloseHandle(UpdateFile);
			}

			
			if (!bSuccess)
			{
				MessageBox(NULL, TEXT("更新失败"), szAppName, MB_TASKMODAL);
			}
			else if(bHaveUpdate)
			{
				//启动进程，关闭自身
				STARTUPINFO si = { 0 };
				si.cb = sizeof(STARTUPINFO);

				PROCESS_INFORMATION pi = { 0 };

				MessageBox(NULL, TEXT("更新完毕"), szAppName, MB_TASKMODAL);
				CreateProcess(TEXT("Homework Viewer RC.exe"), TEXT(" -upd"), 0, 0, 0, 0, 0, 0, &si, &pi);
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
				PostQuitMessage(0);
			}
		}
	}
		break;
	}
}


