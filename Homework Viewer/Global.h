#include<Windows.h>


#include"EasyWindow.h"

//***************************************常量定义

/* * *程序名称定义和版本号* * */

static TCHAR szAppName[] = TEXT("Homework Viewer");

#define lfVersion 0.98
#define szVersion  "0.98"
#define szwVersion  L"0.98"


/* * *界面大小定义* * */

#define CONF_WND_X 919
#define CONF_WND_Y 499

#define CONF_LBORDER 130

#define RBORDER_WIDTH ((float)(400.0 / 1920.0))

#define ICONDISP_HEIGHT ((float)(300.0 / 1080.0))

#define MSGDISP_HEIGHT ((float)(480.0 / 1080.0))

#define INFODISP_HEIGHT ((float)(300.0 / 1080.0))

#define PIC_WIDTH ((float)(1440.0 / 1520.0))

#define PIC_HW_RATIO ((float)(960.0 / 1440.0))

#define CONF_CAP_H 37

#define CONF_LBD_N  47 //NARROW
#define CONF_LBD_W 210 //WIDE

//#define CONF_WND_W 800
//#define CONF_WND_H 520


/* * *界面色彩定义* * */


COLORREF APP_COLOR;

#define APP_BK_COLOR RGB(239,235,239)

#define APP_BKD_COLOR RGB(158,158,158)



HBRUSH hB_AC, hB_ABC, hB_ABDC;

/* * *STRETCH抗锯齿定义* * */

#define STRETCH 3.0


/* * *渐变计时器* * */

#define TIMER_MOVE_TIME 100.0
#define TIMER_TIME_SPACE 15.0

#define TIMER_CHANGE_TIME (TIMER_MOVE_TIME / TIMER_TIME_SPACE)



/* * *FileTime用定义* * */

#define FTTICKSPERHOUR (60 * 60 * (LONGLONG) 10000000)


//***************************************函数定义


EZWNDPROC ConfigProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam);

EZWNDPROC PageHolderProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam);

EZWNDPROC Mask2Proc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam);

EZWNDPROC MainPageProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam);

EZWNDPROC LogonProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam);

EZWNDPROC ViewHomeworkProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam);
EZWNDPROC HomeworkViewHoldProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam);
EZWNDPROC HomeworkViewScrollLayer(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam);
EZWNDPROC HomeworkViewProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam);


EZWNDPROC BasicSettingsProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam);


EZWNDPROC AboutProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam);

BOOL PopupWin(int x, int y, int w, int h, EZWNDPROC Proc);

BOOL UnPopupWin();



EZWNDPROC CaptionProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam);
EZWNDPROC CloseBtnProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam);
EZWNDPROC SmallBtnProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam);

EZWNDPROC BorderProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam);
EZWNDPROC BorderBtnProc(EZWND ezWnd, int message, WPARAM wParam, LPARAM lParam);


BOOL GDIObjectCreate();

BOOL GDIObjectClean();





#ifdef UNICODE
#define Base64Decode Base64DecodeW
#define Base64Encode Base64EncodeW
#else
#define Base64Decode Base64DecodeA
#define Base64Encode Base64EncodeA
#endif

//***************************************全局变量

/* * *窗口句柄* * */


EZWND Config, Caption, SmallBtn, CloseBtn, Border,MaskWnd,PopupWnd,Mask2;

BOOL MaskChangeFlag;

int MaskChangeCount;
int MaskTimerID;

EZWND Conf_PageHolder;

EZWND Conf_MainPage, Conf_Logon, Conf_BscStngs, Conf_ViewHomework,Conf_About;

EZWND HomeworkViewWnd;
EZWND YearEdit, MonthEdit, DayEdit;
int UpdateTimer;



/* * *GDI 对象* * */

LOGFONT FontForm;

HPEN DefPen;



/* * *登录管理* * */

DWORD AuthorityLevel;//0：正在初始配置，1：没登录，2：低级权限，3：高级权限

TCHAR PlatformUsername[30];
TCHAR PlatformPassword[30];



int URLEncode(const char* str, const int strSize, char* result, const int resultSize);

int SettingsRead();

BOOL SettingsWrite();
