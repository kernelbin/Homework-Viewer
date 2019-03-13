#include<Windows.h>
#include"Global.h"



EZWNDPROC MainPageProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam)
{
	static EZWND LoginShowText;
	static EZWND StartBtn,ReloadBtn;

	static EZWNDPROC OldStartBtnProc;
	switch (message)
	{
	case EZWM_CREATE:
		LoginShowText = CreateEZStyleWindow(ezWnd, TEXT("欢迎使用 tyf校园网作业查看器！\r\n部分功能仍在开发，如果出现Bug，请联系开发者yh"), EZS_CHILD | EZS_STATIC, 50, 30, ezWnd->Width, 200);
		FontForm.lfHeight = 60 * (4.0 / 7.0);
		EZSendMessage(LoginShowText, EZWM_SETTEXTALIGN, DT_LEFT, 0);
		EZSendMessage(LoginShowText, EZWM_SETFONT, 0, &FontForm);
		LoginShowText->Transparent = 0;
		//EZSendMessage(LoginShowText, EZWM_SETCOLOR, SYSBKC_3, SYSFTC_H1);

		
		return 0;
	}
	return 0;
}





