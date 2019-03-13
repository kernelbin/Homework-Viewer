#include<Windows.h>
#include"Global.h"



#define REG_STORE_PLACE HKEY_LOCAL_MACHINE


int SettingsRead()
{
	HKEY hKey, hSubKey;

	LONG l;

	DWORD dwValue;

	DWORD Length;
	DWORD Type;
	TCHAR ConvertStr[512];

	l = RegOpenKeyEx(REG_STORE_PLACE, TEXT("Software\\HomeworkViewer"), 0, KEY_QUERY_VALUE, &hKey);


	if (l == ERROR_SUCCESS)
	{
		TCHAR Base64Buf[60] = { 0 };
		Length = sizeof(TCHAR) * 60;
		l = RegQueryValueEx(hKey, TEXT("PlatformUsername"), 0, &Type, &Base64Buf, &Length);
		if (l != ERROR_SUCCESS)
		{
			//������
			RegCloseKey(hKey);
			return 1;
		}
		Base64Decode(Base64Buf, PlatformUsername);
		Length = sizeof(TCHAR) * 60;
		memset(Base64Buf, 0, sizeof(TCHAR) * 60);
		l = RegQueryValueEx(hKey, TEXT("PlatformPassword"), 0, &Type, &Base64Buf, &Length);
		if (l != ERROR_SUCCESS)
		{
			//������
			RegCloseKey(hKey);
			return 1;
		}
		Base64Decode(Base64Buf, PlatformPassword);
		


	}
	else if (l == ERROR_FILE_NOT_FOUND)
	{
		if (hKey)
		{
			RegCloseKey(hKey);
		}
		return 1;
	}
	else
	{
		if (hKey)
		{
			RegCloseKey(hKey);
		}
		return 2;
	}
	RegCloseKey(hKey);
	return 0;
}


BOOL SettingsWrite()
{
	HKEY hKey, hSubKey;

	LONG l;



	l = RegDeleteTree(REG_STORE_PLACE, TEXT("Software\\HomeworkViewer"));
	l = RegCreateKeyEx(REG_STORE_PLACE, TEXT("Software\\HomeworkViewer"), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hKey, NULL);
	if (l == ERROR_SUCCESS)
	{
		/*�ɹ�������������*/

		size_t b_UsernameLen, b_PasswordLen;

		TCHAR Base64Buf[60] = { 0 };
		Base64Encode(PlatformUsername, Base64Buf);

		if (StringCbLength(Base64Buf, 60 * sizeof(TCHAR), &b_UsernameLen) == S_OK)
		{
			RegSetValueEx(hKey, TEXT("PlatformUsername"), 0, REG_SZ, Base64Buf, b_UsernameLen);
		}

		Base64Encode(PlatformPassword, Base64Buf);
		if (StringCbLength(Base64Buf, 60 * sizeof(TCHAR), &b_PasswordLen) == S_OK)
		{
			RegSetValueEx(hKey, TEXT("PlatformPassword"), 0, REG_SZ, Base64Buf, b_PasswordLen);
		}

	}

	RegCloseKey(hKey);

}
