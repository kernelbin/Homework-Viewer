#include<Windows.h>
#include"EasyWindow.h"
#include"Global.h"



EZWNDPROC CaptionProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam)
{
	static HFONT hFont;
	switch (message)
	{
	case EZWM_CREATE:

		CloseBtn = CreateEZWindow(ezWnd, ezWnd->Width - ezWnd->Height, 0, ezWnd->Height, ezWnd->Height, CloseBtnProc);

		SmallBtn = CreateEZWindow(ezWnd, ezWnd->Width - 2 * ezWnd->Height, 0, ezWnd->Height, ezWnd->Height, SmallBtnProc);

		FontForm.lfHeight = ezWnd->Height * (4.0 / 7.0);

		hFont = CreateFontIndirect(&FontForm);

		return 0;
	case EZWM_DRAW:
		SelectObject(wParam, hB_AC);
		PatBlt(wParam, 0, 0, ezWnd->Width, ezWnd->Height, PATCOPY);

		SelectObject(wParam, hFont);

		SetBkMode(wParam, TRANSPARENT);
		SetTextColor(wParam, RGB(255, 255, 255));
		TextOut(wParam, 20, ezWnd->Height * (3.0 / 14.0), TEXT("tyfУ԰����ҵ�鿴�� - Made by yh"), lstrlen(TEXT("tyfУ԰����ҵ�鿴�� - Made by yh")));

		return 0;
	case EZWM_LBUTTONDOWN:
		SendMessage(ezWnd->hParent, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);//�϶����ڡ�

	case EZWM_COMMAND:
		if (lParam == SmallBtn)
		{
			//��С
			ShowWindow(ezWnd->hParent, SW_MINIMIZE);
		}
		else if (lParam == CloseBtn)
		{

			if (Config)
			{
				DestroyEZWindow(Config);
			}

		}
		return 0;
	case EZWM_DESTROY:
		DeleteObject(hFont);

		return 0;
	}
	return 0;
}



EZWNDPROC CloseBtnProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam)
{

	static HBRUSH hBrush;
	static HPEN hPen;
	static BOOL MouseHold;
	switch (message)
	{
	case EZWM_CREATE:
		SetEZWndTransparent(ezWnd, 0);
		hBrush = CreateSolidBrush(RGB(255, 0, 0));
		hPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
		return 0;

	case EZWM_DRAW:

		SelectPen(wParam, hPen);
		MoveToEx(wParam, ezWnd->Width * 0.25, ezWnd->Height * 0.25, NULL);
		LineTo(wParam, ezWnd->Width * 0.75 + 1, ezWnd->Height * 0.75 + 1);

		MoveToEx(wParam, ezWnd->Width * 0.75, ezWnd->Height * 0.25, NULL);
		LineTo(wParam, ezWnd->Width * 0.25 - 1, ezWnd->Height * 0.75 + 1);
		return 0;
	case EZWM_TRANSDRAW:

		SelectObject(wParam, hBrush);
		PatBlt(wParam, 0, 0, ezWnd->Width, ezWnd->Height, PATCOPY);


		return 0;
	case EZWM_LBUTTONDOWN:
		if (MouseHold == FALSE)
		{
			EZCaptureMouse(ezWnd);

			MouseHold = TRUE;
			SetEZWndTransparent(ezWnd, 180);
			EZRepaint(ezWnd, NULL);
		}

		return 0;
	case EZWM_LBUTTONUP:

		if (MouseHold == TRUE)
		{
			EZReleaseMouse(ezWnd);
			if ((LOWORD(lParam) > 0) && (LOWORD(lParam) < ezWnd->Width) && (HIWORD(lParam) > 0) && (HIWORD(lParam) < ezWnd->Height))
			{

				MouseHold = FALSE;
				//��ť�������ˣ�

				//����ܻᴥ���������٣�������ˢ��
				SetEZWndTransparent(ezWnd, 45);
				EZRepaint(ezWnd, NULL);

				//��������
				EZSendMessage(ezWnd->ezParent, EZWM_COMMAND, 0, ezWnd);
			}

		}



		return 0;
	case EZWM_MOUSECOME:
		SetEZWndTransparent(ezWnd, 45);
		EZRepaint(ezWnd, NULL);
		return 0;

	case EZWM_RELEASEMOUSE:
		MouseHold = FALSE;
		//��������°벿�����ú�MOUSELEAVE�ظ����Ͳ���һ��

	case EZWM_MOUSELEAVE:
		SetEZWndTransparent(ezWnd, 0);
		EZRepaint(ezWnd, NULL);
		return 0;
	case EZWM_DESTROY:
		DeleteObject(hBrush);
		DeleteObject(hPen);
		return 0;
	}
	return 0;
}


EZWNDPROC SmallBtnProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam)
{

	static HBRUSH hBrush;
	static HPEN hPen;
	static BOOL MouseHold;
	switch (message)
	{
	case EZWM_CREATE:
		SetEZWndTransparent(ezWnd, 0);
		hBrush = CreateSolidBrush(RGB(255, 255, 255));
		hPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
		return 0;

	case EZWM_DRAW:
		SelectPen(wParam, hPen);
		MoveToEx(wParam, ezWnd->Width * 0.25, ezWnd->Height * 0.75, NULL);
		LineTo(wParam, ezWnd->Width * 0.75, ezWnd->Height * 0.75);

		return 0;
	case EZWM_TRANSDRAW:

		SelectObject(wParam, hBrush);
		PatBlt(wParam, 0, 0, ezWnd->Width, ezWnd->Height, PATCOPY);


		return 0;
	case EZWM_LBUTTONDOWN:
		if (MouseHold == FALSE)
		{
			EZCaptureMouse(ezWnd);

			MouseHold = TRUE;
			SetEZWndTransparent(ezWnd, 90);
			EZRepaint(ezWnd, NULL);
		}

		return 0;
	case EZWM_LBUTTONUP:

		if (MouseHold == TRUE)
		{
			EZReleaseMouse(ezWnd);
			if ((LOWORD(lParam) > 0) && (LOWORD(lParam) < ezWnd->Width) && (HIWORD(lParam) > 0) && (HIWORD(lParam) < ezWnd->Height))
			{

				MouseHold = FALSE;
				//��ť�������ˣ�

				//��������
				EZSendMessage(ezWnd->ezParent, EZWM_COMMAND, 0, ezWnd);
			}
		}
		//��������°벿�����ú�MOUSECOME�ظ����Ͳ���һ��

	case EZWM_MOUSECOME:
		SetEZWndTransparent(ezWnd, 45);
		EZRepaint(ezWnd, NULL);
		return 0;

	case EZWM_RELEASEMOUSE:
		MouseHold = FALSE;
		//��������°벿�����ú�MOUSELEAVE�ظ����Ͳ���һ��

	case EZWM_MOUSELEAVE:
		SetEZWndTransparent(ezWnd, 0);
		EZRepaint(ezWnd, NULL);
		return 0;
	case EZWM_DESTROY:
		DeleteObject(hBrush);
		DeleteObject(hPen);
		return 0;
	}
	return 0;
}
