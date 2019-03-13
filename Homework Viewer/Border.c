#include<Windows.h>
#include"EasyWindow.h"
#include"Global.h"



HFONT hBtnFont;

BOOL DrawMainBtn(HDC hdc, int x, int y, int Width, int Height);
BOOL DrawLockBtn(HDC hdc, int x, int y, int Width, int Height);
BOOL DrawSettingBtn(HDC hdc, int x, int y, int Width, int Height);
BOOL DrawProgramListBtn(HDC hdc, int x, int y, int Width, int Height);
BOOL DrawProjectionBtn(HDC hdc, int x, int y, int Width, int Height);
BOOL DrawMessageBtn(HDC hdc, int x, int y, int Width, int Height);
BOOL DrawAboutBtn(HDC hdc, int x, int y, int Width, int Height);



struct tagBtnInfo {
	EZWND Btn;
	TCHAR Title[32];
	BOOL(*DrawProc)(HDC hdc, int x, int y, int Width, int Height);
	EZWND * Page;
} BtnInfo[] = {
	0, TEXT("首页"), DrawMainBtn,&Conf_MainPage,
	0, TEXT("登录"), DrawLockBtn,&Conf_Logon,
	0, TEXT("作业"), DrawMessageBtn,& Conf_ViewHomework,
	0, TEXT("设置"), DrawSettingBtn,& Conf_BscStngs,
	0, TEXT("关于"), DrawAboutBtn,& Conf_About
};

int iSelect;






EZWNDPROC BorderProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam)
{

	int i;
	static int TimerID;
	static BOOL IsWide;
	static int iPlace;
	switch (message)
	{
	case EZWM_CREATE:
		iSelect = 0;
		for (i = 0; i < _countof(BtnInfo); i++)
		{
			BtnInfo[i].Btn = CreateEZWindow(ezWnd, 0, 12 + i*(CONF_LBD_N), CONF_LBD_N, CONF_LBD_N, BorderBtnProc);
			BtnInfo[i].Btn->ezID = i;
		}
		BtnInfo[0].Btn->Transparent = 80;
		IsWide = 0;
		iPlace = 0;
		TimerID = -1;

		FontForm.lfHeight = CONF_LBD_N * (4.0 / 7.0);

		hBtnFont = CreateFontIndirect(&FontForm);

		return 0;

	case EZWM_DRAW:
		SelectObject(wParam, hB_ABDC);
		PatBlt(wParam, 0, 0, ezWnd->Width, ezWnd->Height, PATCOPY);


		return 0;
	case EZWM_TIMER:

		if (IsWide == 0)
		{
			iPlace++;
			if (iPlace > TIMER_CHANGE_TIME)
			{
				iPlace = TIMER_CHANGE_TIME;
				IsWide = 1;
				KillEZTimer(ezWnd, TimerID);
				TimerID = -1;
			}

		}
		else if (IsWide == 1)
		{
			iPlace--;
			if (iPlace < 0)
			{
				iPlace = 0;
				IsWide = 0;
				KillEZTimer(ezWnd, TimerID);
				TimerID = -1;
				Mask2->ShowState = 2;
			}

		}

		double Width, X;
		X = (iPlace / TIMER_CHANGE_TIME);
		Width = CONF_LBD_N + (CONF_LBD_W - CONF_LBD_N)*(-(X)*(X - 2));

		Mask2->Transparent = 150 * X;
		
		for (i = 0; i < _countof(BtnInfo); i++)
		{
			MoveEZWindow(BtnInfo[i].Btn, 0, 12 + i*(CONF_LBD_N), Width, CONF_LBD_N, 0);
		}
		MoveEZWindow(ezWnd, 0, CONF_CAP_H, Width, ezWnd->Height, 0);
	
		EZRepaint(Config, 0);
		return 0;

	case EZWM_COMMAND:
		
		for (i = 0; i < _countof(BtnInfo); i++)
		{
			(*(BtnInfo[i].Page))->ShowState = 2;
			(*(BtnInfo[i].Page))->MouseMsgRecv = 2;
			if (lParam == BtnInfo[i].Btn)
			{
				//全部权限
				iSelect = i;

			}
		}
		
			
		(*(BtnInfo[iSelect].Page))->ShowState = 1;
		(*(BtnInfo[iSelect].Page))->MouseMsgRecv = 1;
		EZRepaint(Config, NULL);

		if (lParam == BtnInfo[0].Btn)//主菜单
		{
			if (TimerID == -1)
			{
				TimerID = SetEZTimer(ezWnd, TIMER_TIME_SPACE);
				Mask2->ShowState = 1;
			}
			else
			{
				IsWide = !IsWide;
			}

			if (IsWide)
			{
				(*(BtnInfo[0].Page))->MouseMsgRecv = 1;
			}
			else
			{
				(*(BtnInfo[0].Page))->MouseMsgRecv = 2;
			}
			
		}
		else
		{
			if (IsWide)
			{
				if (TimerID == -1)
				{
					TimerID = SetEZTimer(ezWnd, TIMER_TIME_SPACE);
				}
				else
				{
					IsWide = !IsWide;
				}
			}
			
		}

		EZRepaint(ezWnd, NULL);
		
		return 0;
	case EZWM_DESTROY:
		DeleteObject(hBtnFont);
		return 0;
	}
	return 0;
}


EZWNDPROC BorderBtnProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case EZWM_CREATE:
		ezWnd->Transparent = 0;
		return 0;

	case EZWM_TRANSDRAW:

		PatBlt(wParam, 0, 0, ezWnd->Width, ezWnd->Height, WHITENESS);

		return 0;
	case EZWM_MOUSECOME:
		if (BtnInfo[iSelect].Btn != ezWnd)
		{
			ezWnd->Transparent = 50;
			EZRepaint(ezWnd, 0);
		}
		return 0;
	case EZWM_MOUSELEAVE:
		if (BtnInfo[iSelect].Btn != ezWnd)
		{
			ezWnd->Transparent = 0;
			EZRepaint(ezWnd, 0);
		}
		return 0;
	
	case EZWM_LBUTTONDOWN:
		EZCaptureMouse(ezWnd);
		return 0;
	case EZWM_LBUTTONUP:
		if (ezWnd->ezRootParent->TopWndExtend->CptMouseWindow == ezWnd)
		{
			EZReleaseMouse(ezWnd);
			int OldSelect = iSelect;
		//	BtnInfo[iSelect].Btn->Transparent = 0;
			EZSendMessage(ezWnd->ezParent, EZWM_COMMAND, 0, ezWnd);
			if (OldSelect != iSelect)
			{
				ezWnd->Transparent = 80;
				BtnInfo[OldSelect].Btn->Transparent = 0;
				EZRepaint(ezWnd, 0);
				EZRepaint(BtnInfo[OldSelect].Btn, 0);
			}
		}
		return 0;
	case EZWM_DRAW:

		if (BtnInfo[iSelect].Btn == ezWnd)
		{
			//SelectPen(wParam, GetStockObject);
			//SelectBrush(wParam, hB_AC);
		//	Rectangle(wParam, 0, 0, ezWnd->Width, ezWnd->Height);
			PatBlt(wParam, 0, 0, ezWnd->Height / 12, ezWnd->Height, PATCOPY);
			//DeletePen(SelectPen(wParam, GetStockPen(WHITE_PEN)));
		}
		
		BtnInfo[ezWnd->ezID].DrawProc(wParam, ezWnd->Height/6.0, ezWnd->Height / 6.0, ezWnd->Height* 2 / 3.0, ezWnd->Height* 2 / 3.0);

		SelectObject(wParam, hBtnFont);

		SetBkMode(wParam, TRANSPARENT);
		SetTextColor(wParam, RGB(255, 255, 255));
		TextOut(wParam, CONF_LBD_N*(1.618), ezWnd->Height*(3.0 / 14.0), BtnInfo[ezWnd->ezID].Title, lstrlen(BtnInfo[ezWnd->ezID].Title));


		return 0;
	}
	return 0;
}




BOOL DrawMainBtn(HDC hdc, int x, int y, int Width, int Height)
{
	//PatBlt(hdc, x, y, Width, Height, BLACKNESS);
	SelectPen(hdc, GetStockObject(WHITE_PEN));
	MoveToEx(hdc, x + Width * 1.0 / 4.0, y + Height * 3.0 / 8.0, 0);
	LineTo(hdc, x + Width * 3.0 / 4.0, y + Height * 3.0 / 8.0);

	MoveToEx(hdc, x + Width * 1.0 / 4.0, y + Height * 4.0 / 8.0, 0);
	LineTo(hdc, x + Width * 3.0 / 4.0, y + Height * 4.0 / 8.0);

	MoveToEx(hdc, x + Width * 1.0 / 4.0, y + Height * 5.0 / 8.0, 0);
	LineTo(hdc, x + Width * 3.0 / 4.0, y + Height * 5.0 / 8.0);
	return 0;
}


BOOL DrawLockBtn(HDC hdc, int x, int y, int Width, int Height)
{
	HDC hdcMem = GetMemDC(hdc, Width*STRETCH, Height*STRETCH);
	HPEN hPen = CreatePen(PS_SOLID, STRETCH, RGB(255, 255, 255));
	SetStretchBltMode(hdc, HALFTONE);
	StretchBlt(hdcMem, 0, 0, Width*STRETCH, Height*STRETCH, hdc, x, y, Width, Height, SRCCOPY);
	double nWidth;
	double nHeight;

	nWidth  = Width * STRETCH / 36.0;
	nHeight = Height * STRETCH / 36.0;
	SelectObject(hdcMem, hPen);

	SelectObject(hdcMem, GetStockObject(NULL_BRUSH));

	Rectangle(hdcMem, nWidth * 12, nHeight * 17 , nWidth * 24, nHeight * 27);

	MoveToEx(hdcMem, nWidth * 22, nHeight *  17 , NULL);

	AngleArc(hdcMem, nWidth * 18, nHeight * 13 , nHeight * 4, 0, 180);

	LineTo(hdcMem, nWidth * 14, nHeight *  17 );

	StretchBlt(hdc, x, y, Width, Height, hdcMem, 0, 0, Width * STRETCH, Height * STRETCH, SRCCOPY);
	DeleteDC(hdcMem);
	DeleteObject(hPen);
}


BOOL DrawSettingBtn(HDC hdc, int x, int y, int Width, int Height)
{
	HDC hdcMem = GetMemDC(hdc, Width*STRETCH, Height*STRETCH);
	HPEN hPen = CreatePen(PS_SOLID, STRETCH, RGB(255, 255, 255));
	SetStretchBltMode(hdc, HALFTONE);
	StretchBlt(hdcMem, 0, 0, Width*STRETCH, Height*STRETCH, hdc, x, y, Width, Height, SRCCOPY);

	double nWidth;
	double nHeight;
	nWidth = Width * STRETCH / 16.0;
	nHeight = Height * STRETCH / 16.0;

	SelectObject(hdcMem, hPen);
	MoveToEx(hdcMem, nWidth * 6,nHeight *2* (4 - sqrt(3.0)), 0);
	LineTo(hdcMem, nWidth * 10, nHeight *2* (4 - sqrt(3.0)));
	LineTo(hdcMem, nWidth * 12, nHeight * 8);
	LineTo(hdcMem, nWidth * 10, nHeight *2* (sqrt(3.0) + 4));
	LineTo(hdcMem, nWidth * 6, nHeight *2* (sqrt(3.0) + 4));
	LineTo(hdcMem, nWidth * 4, nHeight * 8);
	LineTo(hdcMem, nWidth * 6, nHeight *2* (4 - sqrt(3.0)));

	SelectObject(hdcMem, GetStockObject(NULL_BRUSH));
	Ellipse(hdcMem, nWidth * 7, nHeight * 7, nWidth * 9, nHeight * 9);
	StretchBlt(hdc, x, y, Width, Height, hdcMem, 0, 0, Width*STRETCH, Height*STRETCH, SRCCOPY);
	DeleteDC(hdcMem);
	DeleteObject(hPen);
}

BOOL DrawProgramListBtn(HDC hdc, int x, int y, int Width, int Height)
{
	HDC hdcMem = GetMemDC(hdc, Width*STRETCH, Height*STRETCH);
	HPEN hPen = CreatePen(PS_SOLID, STRETCH, RGB(255, 255, 255));
	SetStretchBltMode(hdc, HALFTONE);
	StretchBlt(hdcMem, 0, 0, Width*STRETCH, Height*STRETCH, hdc, x, y, Width, Height, SRCCOPY);

	double nWidth;
	double nHeight;
	nWidth = Width * STRETCH / 32.0;
	nHeight = Height * STRETCH / 32.0;


	SelectObject(hdcMem, hPen);
	MoveToEx(hdcMem, nWidth * 10, nHeight *8, 0);
	LineTo(hdcMem, nWidth * 10, ((nHeight * 14) - 3 * STRETCH));

	MoveToEx(hdcMem, nWidth * 10, ((nHeight * 14) + 3 * STRETCH), 0);
	LineTo(hdcMem, nWidth * 10, nHeight * 24);

	Rectangle(hdcMem,
		((nWidth * 10) - STRETCH), ((nHeight * 14) - STRETCH),
		((nWidth * 10) + STRETCH), ((nHeight * 14) + STRETCH));
	
	MoveToEx(hdcMem, nWidth * 13, nHeight *8, 0);
	LineTo(hdcMem, nWidth * 13, nHeight * 12);
	LineTo(hdcMem, nWidth * 24, nHeight * 12);
	LineTo(hdcMem, nWidth * 24, nHeight * 8);

	MoveToEx(hdcMem ,nWidth * 13 , nHeight * 14, 0);
	LineTo(hdcMem, nWidth * 24, nHeight * 14);
	LineTo(hdcMem, nWidth * 24, nHeight * 20);
	LineTo(hdcMem, nWidth * 13, nHeight * 20);
	LineTo(hdcMem, nWidth * 13, nHeight * 14);

	MoveToEx(hdcMem, nWidth * 13,nHeight *24, 0);
	LineTo(hdcMem, nWidth * 13, nHeight * 22);
	LineTo(hdcMem, nWidth * 24, nHeight * 22);
	LineTo(hdcMem, nWidth * 24, nHeight * 24);

	StretchBlt(hdc, x, y, Width, Height, hdcMem, 0, 0, Width*STRETCH, Height*STRETCH, SRCCOPY);
	DeleteDC(hdcMem);
	DeleteObject(hPen);
	return TRUE;
}

BOOL DrawProjectionBtn(HDC hdc, int x, int y, int Width, int Height)
{
	HDC hdcMem = GetMemDC(hdc, Width*STRETCH, Height*STRETCH);
	HPEN hPen = CreatePen(PS_SOLID, STRETCH, RGB(255, 255, 255));
	SetStretchBltMode(hdc, HALFTONE);
	StretchBlt(hdcMem, 0, 0, Width*STRETCH, Height*STRETCH, hdc, x, y, Width, Height, SRCCOPY);
	double nWidth;
	double nHeight;
	nWidth = Width * STRETCH / 32.0;
	nHeight = Height * STRETCH / 32.0;

	SelectObject(hdcMem, hPen);
	SelectObject(hdcMem, GetStockObject(NULL_BRUSH));

	Rectangle(hdcMem, nWidth*8, nHeight*16, nWidth*16, nWidth * 22);

	//Rectangle(hdcMem, Width*STRETCH * 3 / 8, Height*STRETCH * 11 / 32, Width*STRETCH * 3 / 4, Width*STRETCH * 20 / 32);
	MoveToEx(hdcMem, nWidth * 11, nHeight * 16, NULL);
	LineTo(hdcMem, nWidth * 11, nHeight * 11);
	LineTo(hdcMem, nWidth * 24, nHeight * 11);
	LineTo(hdcMem, nWidth * 24, nHeight * 20);
	LineTo(hdcMem, nWidth * 16, nHeight * 20);


	StretchBlt(hdc, x, y, Width, Height, hdcMem, 0, 0, Width*STRETCH, Height*STRETCH, SRCCOPY);
	DeleteDC(hdcMem);
	DeleteObject(hPen);
}



BOOL DrawMessageBtn(HDC hdc, int x, int y, int Width, int Height)
{
	HDC hdcMem = GetMemDC(hdc, Width*STRETCH, Height*STRETCH);
	HPEN hPen = CreatePen(PS_SOLID, STRETCH, RGB(255, 255, 255));
	SetStretchBltMode(hdc, HALFTONE);
	StretchBlt(hdcMem, 0, 0, Width*STRETCH, Height*STRETCH, hdc, x, y, Width, Height, SRCCOPY);
	double nWidth;
	double nHeight;
	nWidth = Width * STRETCH / 96.0;//128 * 3
	nHeight = Height * STRETCH / 96.0;

	SelectObject(hdcMem, hPen);
	
	MoveToEx(hdcMem, nWidth * 24, nHeight * 24, NULL);
	LineTo(hdcMem, nWidth * 72, nHeight * 24);
	LineTo(hdcMem, nWidth * 72, nHeight * 63);
	LineTo(hdcMem, nWidth * 57, nHeight * 63);
	LineTo(hdcMem, nWidth * 48, nHeight * 72);
	LineTo(hdcMem, nWidth * 39, nHeight * 63);
	LineTo(hdcMem, nWidth * 24, nHeight * 63);
	LineTo(hdcMem, nWidth * 24, nHeight * 24);

	MoveToEx(hdcMem, nWidth*32, nHeight *34, NULL);//34/32*3
	LineTo(hdcMem, nWidth * 64, nHeight*34);
	MoveToEx(hdcMem, nWidth*32, nHeight*44, NULL);//44/32*3
	LineTo(hdcMem, nWidth* 56,nHeight*44);
	MoveToEx(hdcMem, nWidth*32, nHeight*53, NULL);//53/32*3
	LineTo(hdcMem, nWidth* 60, nHeight*53);
	StretchBlt(hdc, x, y, Width, Height, hdcMem, 0, 0, Width*STRETCH, Height*STRETCH, SRCCOPY);
	DeleteDC(hdcMem);
	DeleteObject(hPen);
}

BOOL DrawAboutBtn(HDC hdc, int x, int y, int Width, int Height)
{
	LOGFONT lf = { 0 };
	lf.lfCharSet = DEFAULT_CHARSET;
	lstrcpy(lf.lfFaceName, TEXT("微软雅黑"));
	lf.lfHeight = Height;
	HFONT hOldFont = SelectFont(hdc, CreateFontIndirect(&FontForm));

	RECT rect;
	SetRect(&rect, x, y, x+Width, y+Height);
	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, RGB(255, 255, 255));
	DrawText(hdc, TEXT("i"), -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	DeleteFont(SelectFont(hdc, hOldFont));
	return 0;
}


