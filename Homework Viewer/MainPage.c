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
		LoginShowText = CreateEZStyleWindow(ezWnd, TEXT("��ӭʹ�� tyfУ԰����ҵ�鿴����\r\n���ֹ������ڿ������������Bug������ϵ������yh"), EZS_CHILD | EZS_STATIC, 50, 30, ezWnd->Width, 200);
		FontForm.lfHeight = 60 * (4.0 / 7.0);
		EZSendMessage(LoginShowText, EZWM_SETTEXTALIGN, DT_LEFT, 0);
		EZSendMessage(LoginShowText, EZWM_SETFONT, 0, &FontForm);
		LoginShowText->Transparent = 0;
		//EZSendMessage(LoginShowText, EZWM_SETCOLOR, SYSBKC_3, SYSFTC_H1);

		
		return 0;
	}
	return 0;
}





