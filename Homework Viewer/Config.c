#include <Windows.h>
#include<WinINet.h>
#include<tchar.h>

#include<StrSafe.h>

#include "Global.h"
#include "EasyWindow.h"
#include<math.h>
#include<time.h>
#include"Gumbo/gumbo.h"
#include"GaussianBlur.h"

#pragma comment(lib,"WinINet.lib")


HINTERNET hInetGlobal, hInetPlatform, hInetLogonIndex, hInetLogonRequest;

BOOL TryLogonPlatform();

EZWNDPROC MaskProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam);

EZWNDPROC ConfigProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case EZWM_CREATE:

		Config = ezWnd;

		AuthorityLevel = 0;//假设默认没配置。

		Caption = CreateEZWindow(ezWnd, 0, 0, ezWnd->Width, CONF_CAP_H, CaptionProc);


		Conf_PageHolder = CreateEZWindow(ezWnd, CONF_LBD_N, CONF_CAP_H, ezWnd->Width - CONF_LBD_N, ezWnd->Height - CONF_CAP_H, PageHolderProc);

		Border = CreateEZWindow(ezWnd, 0, CONF_CAP_H, CONF_LBD_N, ezWnd->Height - CONF_CAP_H, BorderProc);


		MaskWnd = CreateEZWindow(ezWnd, 0, CONF_CAP_H, ezWnd->Width, ezWnd->Height - CONF_CAP_H, MaskProc);

		MaskWnd->ShowState = 2;
		MaskWnd->MouseMsgRecv = 2;
		MaskWnd->Transparent = 0;

		ezWnd->Transparent = 0;
		MaskChangeFlag = 0;
		MaskChangeCount = 0;



		return 0;

	case EZWM_TIMER:
		if (MaskChangeFlag == 0)
		{
			MaskChangeCount++;
			if (MaskChangeCount > TIMER_CHANGE_TIME)
			{
				MaskChangeCount = TIMER_CHANGE_TIME;

				KillEZTimer(ezWnd, MaskTimerID);
				MaskChangeFlag = 1;

				MaskTimerID = -1;
			}

		}
		else if (MaskChangeFlag == 1)
		{
			MaskChangeCount--;
			if (MaskChangeCount < 0)
			{
				MaskChangeCount = 0;
				MaskChangeFlag = 0;
				KillEZTimer(ezWnd, MaskTimerID);
				MaskTimerID = -1;
				MaskWnd->ShowState = 2;
			}

		}

		MaskWnd->Transparent = (MaskChangeCount / TIMER_CHANGE_TIME) * 150;
		EZRepaint(ezWnd, NULL);
		return 0;
	case EZWM_DRAW:

		SelectObject(wParam, hB_ABC);
		PatBlt(wParam, 0, 0, ezWnd->Width, ezWnd->Height, PATCOPY);
		return 0;
	case EZWM_DESTROY:
	{



		PostQuitMessage(0);
	}

	return 0;
	}
	return 0;
}




EZWNDPROC MaskProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case EZWM_DRAW:
		if (MaskWnd)
			BitBlt(wParam, MaskWnd->px - ezWnd->px, MaskWnd->py - ezWnd->py, MaskWnd->Width, MaskWnd->Height, MaskWnd->hdc, 0, 0, SRCCOPY);
		return 0;
	case EZWM_TRANSDRAW:
		PatBlt(wParam, 0, 0, ezWnd->Width, ezWnd->Height, BLACKNESS);
		return 0;
	}
}


EZWNDPROC PageHolderProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
	case EZWM_CREATE:
		ezWnd->Transparent = 0;
		Conf_MainPage = CreateEZWindow(ezWnd, 0, 0, ezWnd->Width, ezWnd->Height, MainPageProc);
		Conf_Logon = CreateEZWindow(ezWnd, 0, 0, ezWnd->Width, ezWnd->Height, LogonProc);
		//
		//Conf_ProgList = CreateEZWindow(ezWnd, 0, 0, ezWnd->Width, ezWnd->Height, ProgramListProc);
		//Conf_Projection = CreateEZWindow(ezWnd, 0, 0, ezWnd->Width, ezWnd->Height, ProjectionProc);
		Conf_ViewHomework = CreateEZWindow(ezWnd, 0, 0, ezWnd->Width, ezWnd->Height, ViewHomeworkProc);
		Conf_BscStngs = CreateEZWindow(ezWnd, 0, 0, ezWnd->Width, ezWnd->Height, BasicSettingsProc);

		Conf_About = CreateEZWindow(ezWnd, 0, 0, ezWnd->Width, ezWnd->Height, AboutProc);

		Conf_Logon->ShowState = 2;
		Conf_Logon->MouseMsgRecv = 2;
		Conf_ViewHomework->ShowState = 2;
		Conf_ViewHomework->MouseMsgRecv = 2;
		Conf_BscStngs->ShowState = 2;
		Conf_BscStngs->MouseMsgRecv = 2;
		Conf_About->ShowState = 2;
		Conf_About->MouseMsgRecv = 2;
		Mask2 = CreateEZWindow(ezWnd, 0, 0, ezWnd->Width, ezWnd->Height, Mask2Proc);
		Mask2->Transparent = 0;
		Mask2->MouseMsgRecv = 2;


		return 0;
	case EZWM_LBUTTONDOWN:
		EZSendMessage(Border, EZWM_COMMAND, 0, ezWnd);
		return 0;
	case EZWM_TRANSDRAW:
		PatBlt(wParam, 0, 0, ezWnd->Width, ezWnd->Height, BLACKNESS);
		return 0;
	}
}


EZWNDPROC Mask2Proc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case EZWM_DRAW:
		return 0;
	case EZWM_TRANSDRAW:
		PatBlt(wParam, 0, 0, ezWnd->Width, ezWnd->Height, BLACKNESS);
		return 0;
	}
}


EZWNDPROC LogonProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam)
{
	static EZWND LogonBtn;
	static BOOL IsLogOn;
	switch (message)
	{
	case EZWM_CREATE:
		IsLogOn = FALSE;
		LogonBtn = CreateEZStyleWindow(ezWnd, TEXT("登录"), EZS_CHILD | EZS_BUTTON,
			(ezWnd->Width - 120) / 2, (ezWnd->Height - 50) / 2, 120, 50);
		EZSendMessage(LogonBtn, EZWM_SETCOLOR, RGB(0, 0, 0), RGB(0, 0, 0));
		FontForm.lfHeight = 50 * 0.6;
		EZSendMessage(LogonBtn, EZWM_SETFONT, 0, &FontForm);
		return 0;
	case EZWM_COMMAND:
	{
		switch (IsLogOn)
		{
		case FALSE:
			if (TryLogonPlatform())
			{
				IsLogOn = TRUE;
				EZSendMessage(LogonBtn, EZWM_SETTEXT, TEXT("登出"), 0);
				EZRepaint(LogonBtn, 0);
			}
			break;
		case TRUE:
			MessageBox(NULL, TEXT("安全退出功能正在开发中，请直接关闭该软件并重新打开。"), szAppName, 0);
			break;
		}
	}
	}
	return 0;
}


EZWNDPROC ViewHomeworkProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam)
{
	static EZWND YearText, MonthText, DayText;
	static EZWND QueryBtn;
	static EZWND ViewPageWnd;
	switch (message)
	{
	case EZWM_CREATE:
		FontForm.lfHeight = 35;
		YearText = CreateEZStyleWindow(ezWnd, TEXT("年："), EZS_CHILD | EZS_STATIC, 60, 22, 45, 35);
		EZSendMessage(YearText, EZWM_SETTEXTALIGN, DT_LEFT | DT_VCENTER | DT_SINGLELINE, 0);
		YearText->Transparent = 0;
		EZSendMessage(YearText, EZWM_SETFONT, 0, &FontForm);

		YearEdit = CreateEZStyleWindow(ezWnd, TEXT(""), EZS_CHILD | EZS_EDIT | EZES_SINGLELINE, 110, 22, 80, 35);
		EZSendMessage(YearEdit, EZWM_SETFONT, 0, &FontForm);
		EZSendMessage(YearEdit, EZWM_SETMAXTEXT, 4, 0);

		MonthText = CreateEZStyleWindow(ezWnd, TEXT("月："), EZS_CHILD | EZS_STATIC, 205, 22, 45, 35);
		EZSendMessage(MonthText, EZWM_SETTEXTALIGN, DT_LEFT | DT_VCENTER | DT_SINGLELINE, 0);
		MonthText->Transparent = 0;
		EZSendMessage(MonthText, EZWM_SETFONT, 0, &FontForm);


		MonthEdit = CreateEZStyleWindow(ezWnd, TEXT(""), EZS_CHILD | EZS_EDIT | EZES_SINGLELINE, 255, 22, 45, 35);
		EZSendMessage(MonthEdit, EZWM_SETFONT, 0, &FontForm);
		EZSendMessage(MonthEdit, EZWM_SETMAXTEXT, 2, 0);

		DayText = CreateEZStyleWindow(ezWnd, TEXT("日："), EZS_CHILD | EZS_STATIC, 310, 22, 45, 35);
		EZSendMessage(DayText, EZWM_SETTEXTALIGN, DT_LEFT | DT_VCENTER | DT_SINGLELINE, 0);
		DayText->Transparent = 0;
		EZSendMessage(DayText, EZWM_SETFONT, 0, &FontForm);


		DayEdit = CreateEZStyleWindow(ezWnd, TEXT(""), EZS_CHILD | EZS_EDIT | EZES_SINGLELINE, 360, 22, 45, 35);
		EZSendMessage(DayEdit, EZWM_SETFONT, 0, &FontForm);
		EZSendMessage(DayEdit, EZWM_SETMAXTEXT, 2, 0);

		//获取现在的年月日并填入
		SYSTEMTIME CurrentTime;
		GetLocalTime(&CurrentTime);

		TCHAR DateStrBuf[16];
		wsprintf(DateStrBuf, TEXT("%d"), CurrentTime.wYear);
		EZSendMessage(YearEdit, EZWM_SETTEXT, DateStrBuf, 0);
		wsprintf(DateStrBuf, TEXT("%d"), CurrentTime.wMonth);
		EZSendMessage(MonthEdit, EZWM_SETTEXT, DateStrBuf, 0);
		wsprintf(DateStrBuf, TEXT("%d"), CurrentTime.wDay);
		EZSendMessage(DayEdit, EZWM_SETTEXT, DateStrBuf, 0);

		QueryBtn = CreateEZStyleWindow(ezWnd, TEXT("查询!!"), EZS_CHILD | EZS_BUTTON, 460, 15, 120, 50);
		EZSendMessage(QueryBtn, EZWM_SETTEXTALIGN, DT_CENTER | DT_VCENTER | DT_SINGLELINE, 0);
		EZSendMessage(QueryBtn, EZWM_SETFONT, 0, &FontForm);
		EZSendMessage(QueryBtn, EZWM_SETCOLOR, RGB(0, 0, 0), RGB(0, 0, 0));

		ViewPageWnd = CreateEZWindow(ezWnd, 110, 110, 650, 300, HomeworkViewHoldProc);

		//

		return 0;
	case EZWM_DRAW:
		SelectObject(wParam, CreatePen(PS_SOLID | PS_INSIDEFRAME, 2, APP_COLOR));
		SelectObject(wParam, GetStockObject(NULL_BRUSH));

		if (YearEdit)
		{
			Rectangle(wParam, YearEdit->x - 2, YearEdit->y - 2, YearEdit->x + YearEdit->Width + 2, YearEdit->y + YearEdit->Height + 2);
		}

		if (MonthEdit)
		{
			Rectangle(wParam, MonthEdit->x - 2, MonthEdit->y - 2, MonthEdit->x + MonthEdit->Width + 2, MonthEdit->y + MonthEdit->Height + 2);
		}

		if (DayEdit)
		{
			Rectangle(wParam, DayEdit->x - 2, DayEdit->y - 2, DayEdit->x + DayEdit->Width + 2, DayEdit->y + DayEdit->Height + 2);
		}

		DeleteObject(SelectObject(wParam, GetStockObject(WHITE_BRUSH)));
		DeleteObject(SelectObject(wParam, GetStockObject(BLACK_PEN)));
		return 0;

	case EZWM_COMMAND:
		if (lParam == QueryBtn)
		{
			EZSendMessage(HomeworkViewWnd, EZWM_USER_NOTIFY, 1, 0);
			EZRepaint(ezWnd, 0);
		}
	}
	return 0;
}




EZWNDPROC HomeworkViewHoldProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam)
{

	static EZWND ScrollBar, ScrollLayer;
	switch (message)
	{
	case EZWM_CREATE:

		ScrollBar = CreateEZStyleWindow(ezWnd, TEXT(""), EZS_CHILD | EZS_CHILD_VSCROLL, ezWnd->Width - 24, 0, 24, ezWnd->Height);
		ScrollLayer = CreateEZWindow(ezWnd, 0, 0, ezWnd->Width - 24, ezWnd->Height, HomeworkViewScrollLayer);
		//MoveEZWindow(ezWnd->Extend->hExtend[1],//竖直，V
		//	ezWnd->Width - 15, 0,
		//	CHK_ALT_STYLE(ezWnd->EZStyle, EZS_VSCROLL) ? (15) : (0),
		//	CHK_ALT_STYLE(ezWnd->EZStyle, EZS_HSCROLL) ? (ezWnd->Height - 15) : (ezWnd->Height)
		//	, 0);


		EZSendMessage(ScrollBar, EZWM_SETSCROLLRANGE, 1200, 0);
		EZSendMessage(ScrollBar, EZWM_SETSCROLLPOS, 0, ezWnd->Height);

		return 0;
	case EZWM_DRAW:
		PatBlt(wParam, 0, 0, ezWnd->Width, ezWnd->Height, WHITENESS);
		return 0;
	case EZWM_SCROLLPOSCHANGE:
		ScrollEZWindow(ScrollLayer, 0, -(int)wParam, 0);
		/*ScrollLayer->ScrollY = ;*/

		return 0;


	}
}


EZWNDPROC HomeworkViewScrollLayer(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam)
{
	static HDC hdcBorder;
	switch (message)
	{
	case EZWM_CREATE:
		hdcBorder = GetMemDC(ezWnd->hdc, ezWnd->Width + 100, ezWnd->Height + 100);
		PatBlt(hdcBorder, 0, 0, ezWnd->Width + 100, ezWnd->Height + 100, BLACKNESS);
		//修改透明层
		{

			HBITMAP hSel = SelectObject(hdcBorder, CreateCompatibleBitmap(hdcBorder, 1, 1));
			BITMAP bmp;
			GetObject(hSel, sizeof(BITMAP), &bmp);
			PBYTE pData = malloc(bmp.bmWidth * bmp.bmHeight * 4);

			GetBitmapBits(hSel, bmp.bmWidth * bmp.bmHeight * 4, pData);
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

			AlphaOnlyGaussianBlurFilter(pData, pData, bmp.bmWidth, bmp.bmHeight, bmp.bmWidthBytes, 5);
			SetBitmapBits(hSel, bmp.bmWidth * bmp.bmHeight * 4, pData);

			free(pData);
			DeleteObject(SelectObject(hdcBorder, hSel));
		}
		ezWnd->Transparent = 100;
		HomeworkViewWnd = CreateEZWindow(ezWnd, 0, 0, ezWnd->Width, 1200, HomeworkViewProc);
		return 0;
	case EZWM_TRANSDRAW:
	{
		BLENDFUNCTION bf = { 0 };
		bf.AlphaFormat = AC_SRC_ALPHA;
		bf.SourceConstantAlpha = 255;
		AlphaBlend(wParam, 0, 0, ezWnd->Width, ezWnd->Height, hdcBorder, 50, 44, ezWnd->Width, ezWnd->Height, bf);
		//StretchBlt(wParam, 0, 0, ezWnd->Width, ezWnd->Height, hdcBorder, 50, 50, ezWnd->Width, ezWnd->Height, SRCCOPY);
	}

	//PatBlt(wParam, 0, 0, ezWnd->Width, ezWnd->Height, BLACKNESS);
	break;
	case EZWM_DESTROY:
		DeleteMemDC(hdcBorder);
		break;
	}
	return 0;
}

EZWNDPROC HomeworkViewProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam)
{
	static char TextBuffer[32768];
	RECT rect;
	switch (message)
	{
	case EZWM_DRAW:
	{
		FontForm.lfHeight = 28;
		HFONT hFont = CreateFontIndirect(&FontForm);
		SetRect(&rect, 30, 30, ezWnd->Width - 60, ezWnd->Height - 60);
		SelectFont(wParam, hFont);
		SetBkMode(wParam, TRANSPARENT);
		DrawTextA(wParam, TextBuffer, -1, &rect, DT_LEFT | DT_WORDBREAK | DT_NOCLIP | DT_NOPREFIX);
		DeleteFont(hFont);
		return 0;
	}


	case EZWM_USER_NOTIFY:
		if (wParam == 1)//查询作业
		{

			if (!hInetPlatform)
			{
				MessageBox(NULL, TEXT("您尚未登陆！请先登录！"), szAppName, 0);
				return 0;
			}
			HINTERNET GetHomeworkReq = 0;
			__try
			{
				PCWSTR szAcceptTypes[] = { L"text/*",L"*/*", NULL };
				//TCHAR DateStrBuf[32];
				int iYear, iMonth, iDay;


				iYear = _ttoi(YearEdit->Extend->Title);
				iMonth = _ttoi(MonthEdit->Extend->Title);
				iDay = _ttoi(DayEdit->Extend->Title);

				WCHAR HomeworkRequsetURI[256];
				wsprintfW(HomeworkRequsetURI, L"/cjcx/jiaoshi/plshuru/zybz/zybz.asp?rq=%d-%d-%d", iYear, iMonth, iDay);
				GetHomeworkReq = HttpOpenRequestW(hInetPlatform, L"GET", HomeworkRequsetURI, NULL, NULL, szAcceptTypes, 0, 0);
				if (!GetHomeworkReq)
				{
					MessageBox(NULL, TEXT("HttpOpenRequestW函数出错"), szAppName, 0);
					__leave;
				}
				if (HttpSendRequestW(GetHomeworkReq, NULL, 0, 0, 0) == 0)
				{
					MessageBox(NULL, TEXT("发送连接请求失败！请检查网络！\r\n具体信息：HttpSendRequestW执行出错"), szAppName, 0);
					__leave;
				}

				BYTE DataBuf[32768] = { 0 };
				PBYTE rp = DataBuf;
				BOOL bRet;
				DWORD num = 0, tnum = 0;
				do
				{
					bRet = InternetReadFile(GetHomeworkReq, rp, 1024, &num);
					tnum += num;
					rp += num;

				} while (!(bRet && num == 0));
				GumboOutput * PhraseResult = gumbo_parse(DataBuf);
				//dfs文本节点
				memset(TextBuffer, 0, 32768);
				TextNodeDFS(PhraseResult->document, TextBuffer);
				gumbo_destroy_output(&kGumboDefaultOptions, PhraseResult);
			}
			__finally
			{
				if (GetHomeworkReq)
				{
					InternetCloseHandle(GetHomeworkReq);
				}
			}

		}
		return 0;
	}
}


int TextNodeDFS(GumboNode * node, char TextBuffer[])
{
	switch (node->type)
	{
	case GUMBO_NODE_DOCUMENT:
		for (int i = 0; i < node->v.document.children.length; i++)
		{
			TextNodeDFS(node->v.document.children.data[i], TextBuffer);
		}

		break;
	case GUMBO_NODE_ELEMENT:
		for (int i = 0; i < node->v.element.children.length; i++)
		{
			switch (node->v.element.tag)
			{
			case GUMBO_TAG_SCRIPT:
			case GUMBO_TAG_STYLE:
				break;
			case GUMBO_TAG_A://TODO:如何处理合适？
				break;
			default:
				TextNodeDFS(node->v.element.children.data[i], TextBuffer);
			}

		}
		break;
	case GUMBO_NODE_TEXT:
		//从origional_text里截获信息

	{
		int iDst = strlen(TextBuffer);
		for (int iSrc = 0; iSrc < node->v.text.original_text.length; iSrc++)
		{
			if (node->v.text.original_text.data[iSrc] == '&')
			{
				if (_strnicmp(node->v.text.original_text.data + iSrc, "&nbsp;", 6) == 0)
				{
					iSrc += 5;
					TextBuffer[iDst] = ' ';
					iDst++;
					continue;
				}
				if (_strnicmp(node->v.text.original_text.data + iSrc, "&quot;", 6) == 0)
				{
					iSrc += 5;
					TextBuffer[iDst] = '\"';
					iDst++;
					continue;
				}
				if (_strnicmp(node->v.text.original_text.data + iSrc, "&amp;", 5) == 0)
				{
					iSrc += 4;
					TextBuffer[iDst] = '&';
					iDst++;
					continue;
				}
				if (_strnicmp(node->v.text.original_text.data + iSrc, "&lt;", 4) == 0)
				{
					iSrc += 3;
					TextBuffer[iDst] = '<';
					iDst++;
					continue;
				}
				if (_strnicmp(node->v.text.original_text.data + iSrc, "&gt;", 4) == 0)
				{
					iSrc += 3;
					TextBuffer[iDst] = '>';
					iDst++;
					continue;
				}
			}
			TextBuffer[iDst++] = node->v.text.original_text.data[iSrc];

		}
		//strncat_s(TextBuffer, 32768, node->v.text.original_text.data, node->v.text.original_text.length);
		//strcat_s(TextBuffer, 32768, "\r\n");
		TextBuffer[iDst++] = '\r';
		TextBuffer[iDst++] = '\n';
		break;
	}

	}
}


EZWNDPROC AboutProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam)
{
	static EZWND AboutTitle, AboutText;
	switch (message)
	{
	case EZWM_CREATE:
		AboutTitle = CreateEZStyleWindow(ezWnd, TEXT("关于Homework Viewer"), EZS_CHILD | EZS_STATIC, 50, 30, ezWnd->Width, 60);
		FontForm.lfHeight = 60 * (4.0 / 7.0);
		EZSendMessage(AboutTitle, EZWM_SETTEXTALIGN, DT_LEFT, 0);
		EZSendMessage(AboutTitle, EZWM_SETFONT, 0, &FontForm);
		AboutTitle->Transparent = 0;

		AboutText = CreateEZStyleWindow(ezWnd, TEXT("版本：Prerelease 0.99 32位\r\n编译日期：")TEXT(__DATE__)TEXT("\r\n\r\n"), EZS_CHILD | EZS_STATIC, 50, 85, ezWnd->Width, ezWnd->Height);
		FontForm.lfHeight = 40 * (4.0 / 7.0);
		EZSendMessage(AboutText, EZWM_SETTEXTALIGN, DT_LEFT, 0);
		EZSendMessage(AboutText, EZWM_SETFONT, 0, &FontForm);
		AboutText->Transparent = 0;
	}
}


BOOL PopupWin(int x, int y, int w, int h, EZWNDPROC Proc)
{
	MaskWnd->ShowState = 1;
	Conf_PageHolder->MouseMsgRecv = 2;
	Border->MouseMsgRecv = 2;
	if (PopupWnd)
	{
		return FALSE;
	}
	PopupWnd = CreateEZWindow(Config, x, y, w, h, Proc);
	MaskChangeFlag = 0;
	MaskTimerID = SetEZTimer(Config, TIMER_TIME_SPACE);
	return TRUE;
}

BOOL UnPopupWin()
{
	DestroyEZWindow(PopupWnd);
	PopupWnd = NULL;
	//MaskWnd->ShowState = 2;


	Conf_PageHolder->MouseMsgRecv = 1;
	Border->MouseMsgRecv = 1;
	MaskChangeFlag = 1;
	if (MaskTimerID == -1)
	{
		MaskTimerID = SetEZTimer(Config, TIMER_TIME_SPACE);
	}


	return TRUE;
}


BOOL TryLogonPlatform()
{
	BOOL bRet = FALSE;




	__try
	{
		if (lstrlen(PlatformUsername) == 0 || lstrlen(PlatformPassword) == 0)
		{
			MessageBox(NULL, TEXT("请先到 设置 -> 账号 里输入校园网用户名密码。"), szAppName, 0);
			__leave;
		}
		if (!InternetCheckConnectionW(L"http://www.tjyfz1.edu.sh.cn/", FLAG_ICC_FORCE_CONNECTION, 0))
		{
			MessageBox(NULL, TEXT("尝试建立网络连接失败！请尝试检查网络"), szAppName, 0);
			__leave;
		}

		hInetGlobal = 0;
		hInetGlobal = InternetOpenW(L"Homework Viewer", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
		if (!hInetGlobal)
		{
			MessageBox(NULL, TEXT("创建网络连接时发生异常\r\n具体信息：InternetOpenW执行出错"), szAppName, 0);
			__leave;
		}

		hInetPlatform = InternetConnectW(hInetGlobal, L"www.tjyfz1.edu.sh.cn", INTERNET_DEFAULT_HTTP_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
		if (!hInetPlatform)
		{
			MessageBox(NULL, TEXT("创建网络连接时发生异常\r\n具体信息：InternetConnectW执行出错"), szAppName, 0);
			__leave;
		}

		PCWSTR szAcceptTypes[] = { L"text/*",L"*/*", NULL };
		hInetLogonIndex = HttpOpenRequestW(hInetPlatform, L"GET", L"/index.asp", NULL, NULL, szAcceptTypes, 0, 0);
		if (!hInetLogonIndex)
		{
			MessageBox(NULL, TEXT("HttpOpenRequestW函数出错"), szAppName, 0);
			__leave;
		}
		if (HttpSendRequestW(hInetLogonIndex, NULL, 0, 0, 0) == 0)
		{
			MessageBox(NULL, TEXT("发送连接请求失败！请检查网络！\r\n具体信息：HttpSendRequestW执行出错"), szAppName, 0);
			__leave;
		}
		DWORD error = GetLastError();
		InternetCloseHandle(hInetLogonIndex);
		hInetLogonIndex = 0;


		//合并用户名密码为一个大字符串，（用户名+&+密码） 然后转UTF8，然后URI Encode

		TCHAR EncodeBuffer[60] = { 0 };
		wsprintf(EncodeBuffer, TEXT("%s&%s"), PlatformUsername, PlatformPassword);

		WCHAR* StrToEncode;
		PBYTE UTF8Str;

#ifndef UNICODE
		int len = MultiByteToWideChar(CP_ACP, 0, EncodeBuffer, -1, 0, 0);
		StrToEncode = malloc(sizeof(WCHAR) * (len + 2));
		MultiByteToWideChar(CP_ACP, 0, EncodeBuffer, -1, StrToEncode, len);
#else 
		StrToEncode = EncodeBuffer;
#endif

		//将StrToEncode转为 UTF-8
		char UTF8Buf[256] = { 0 };
		char RawCode[1024] = { 0 };
		WideCharToMultiByte(CP_UTF8, 0, StrToEncode, lstrlenW(StrToEncode), UTF8Buf, 256, 0, 0);
		URLEncode(UTF8Buf, strlen(UTF8Buf), RawCode, 1024);
		//strcpy_s(RawCode,1024, "610111200104253016&T1fPassword");
#ifndef UNICODE
		free(StrToEncode);
#endif

		//随机地址池，26*26
		int AddrPool[676];
		int AddrPoolSize = 676;
		for (int i = 0; i < AddrPoolSize; i++)
		{
			AddrPool[i] = i;
		}
		srand(time(0));

		char sess[1000] = { 0 };
		int SessLen = 0;
		char userIDStr[2002] = { 0 };
		for (int i = 0; i < 2000; i++)
		{
			userIDStr[i] = ((rand() & 1) ? 'A' : 'a') + rand() % 26;
		}
		char AsciiBuf[5] = { 0 };

		for (int i = 0; RawCode[i]; i++)
		{
			sprintf_s(AsciiBuf, 5, "%da", RawCode[i]);
			for (int Asc = 0; AsciiBuf[Asc]; Asc++)
			{
				//从地址池随机选地址出来，然后调整地址池，把字符塞进随机大串
				int AddrRand = rand() % AddrPoolSize;
				userIDStr[AddrPool[AddrRand] + 26] = AsciiBuf[Asc];
				sess[SessLen++] = 'A' + AddrPool[AddrRand] / 26;
				sess[SessLen++] = 'a' + AddrPool[AddrRand] % 26;
				AddrPool[AddrRand] = AddrPool[--AddrPoolSize];
			}
		}
		//sess最后多的那个a扣掉
		sess[SessLen - 2] = 0;

		//这里，设置Cookies
		InternetSetCookieA("http://www.tjyfz1.edu.sh.cn", "sess",
			sess);

		InternetSetCookieW(L"http://www.tjyfz1.edu.sh.cn", L"cok", L"");
		hInetLogonRequest = HttpOpenRequestW(hInetPlatform, L"POST", L"/cjcx/chsr1.asp", NULL, L"http://www.tjyfz1.edu.sh.cn/index.asp", szAcceptTypes, 0,
			0);
		if (!hInetLogonRequest)
		{
			MessageBox(NULL, TEXT("HttpOpenRequestW函数出错"), szAppName, 0);
			__leave;
		}
		char ContentBuf[2048];
		sprintf_s(ContentBuf, 2048, "userid=%s&leibie=xuesheng", userIDStr);
		if (HttpSendRequestW(hInetLogonRequest, L"Content-Type: application/x-www-form-urlencoded\r\n", lstrlenW(L"Content-Type: application/x-www-form-urlencoded\r\n"),
			ContentBuf,
			strlen(ContentBuf)) == 0)
		{
			if (HttpSendRequestW(hInetLogonIndex, NULL, 0, 0, 0) == 0)
			{
				MessageBox(NULL, TEXT("发送连接请求失败！请检查网络！\r\n具体信息：HttpSendRequestW执行出错"), szAppName, 0);
				__leave;
			}
		}

		WCHAR cokBuf[1501] = { 0 };
		DWORD cokSize = 1500;
		InternetGetCookieW(L"http://www.tjyfz1.edu.sh.cn", L"cok", cokBuf, &cokSize);
		if (lstrlenW(cokBuf) <= 100)
		{
			MessageBox(NULL, TEXT("登陆失败！"), szAppName, 0);
			__leave;
		}


		bRet = TRUE;
	}
	__finally
	{
		if (bRet == FALSE)
		{
			//清理
			if (hInetGlobal)
			{
				InternetCloseHandle(hInetGlobal);
				hInetGlobal = 0;
			}
			if (hInetPlatform)
			{
				InternetCloseHandle(hInetPlatform);
				hInetPlatform = 0;
			}
			if (hInetLogonIndex)
			{
				InternetCloseHandle(hInetLogonIndex);
				hInetLogonIndex = 0;
			}
			if (hInetLogonRequest)
			{
				InternetCloseHandle(hInetLogonRequest);
				hInetLogonRequest = 0;
			}
		}
		else
		{
		}
	}

	return bRet;
}