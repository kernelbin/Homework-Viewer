#include <Windows.h>
#include<tchar.h>
#include<StrSafe.h>
#include "Global.h"
#include "EasyWindow.h"
#include<math.h>
#include"GaussianBlur.h"



EZWND AccountPage, AppearancePage;

typedef struct __StngBtn
{
	TCHAR Title[16];
	EZWND* Page;
}SETTINGBTN;

SETTINGBTN StngBtnInfo[] =
{
	TEXT("账号"), &AccountPage,
	TEXT("外观"), &AppearancePage,
};

int SelStng;

EZWND StngBtn[_countof(StngBtnInfo)];

EZWNDPROC SettingsBtnProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam);

EZWNDPROC AccountPageProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam);
EZWNDPROC AppearancePageProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam);

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

		AppearancePage->ShowState = 2;
		AppearancePage->MouseMsgRecv = 2;

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

		AccountUsageStatic = CreateEZStyleWindow(ezWnd, TEXT("该账号用于登陆校园网并获取作业，暂不支持教师账号。"), EZS_CHILD | EZS_STATIC, 50, 80, ezWnd->Width, 30);
		FontForm.lfHeight = 30 * (4.0 / 7.0);
		EZSendMessage(AccountUsageStatic, EZWM_SETTEXTALIGN, DT_LEFT, 0);
		EZSendMessage(AccountUsageStatic, EZWM_SETFONT, 0, &FontForm);
		EZSendMessage(AccountUsageStatic, EZWM_SETCOLOR, RGB(0, 0, 0), RGB(180, 180, 180));
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
		SettingsRead();
		EZSendMessage(UserNameEdit, EZWM_SETTEXT, PlatformUsername, 0);
		EZSendMessage(PasswdEdit, EZWM_SETTEXT, PlatformPassword, 0);

		//读取配置，填入
		break;
	case EZWM_COMMAND:
		if (lParam == SaveBtn)
		{
			//写入注册表，写入全局变量。
			//MessageBox(0, TEXT(""), PasswdEdit->Extend->Title,0);
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
	switch (message)
	{
	case EZWM_DRAW:
		TitleStatic = CreateEZStyleWindow(ezWnd, TEXT("外观设置"), EZS_CHILD | EZS_STATIC, 50, 30, ezWnd->Width, 200);
		FontForm.lfHeight = 60 * (4.0 / 7.0);
		EZSendMessage(TitleStatic, EZWM_SETTEXTALIGN, DT_LEFT, 0);
		EZSendMessage(TitleStatic, EZWM_SETFONT, 0, &FontForm);
		TitleStatic->Transparent = 0;
		break;
	}
}