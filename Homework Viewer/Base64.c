#include<Windows.h>
#include <stdio.h>
#include <string.h>
#include<tchar.h>
// ȫ�ֳ�������
const char * base64char = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const char padding_char = '=';

/*�������
* const unsigned char * sourcedata�� Դ����
* char * base64 �����ֱ���
*/
int Base64EncodeA(const unsigned char * sourcedata, char * base64)
{
	int i = 0, j = 0;
	unsigned char trans_index = 0;    // ������8λ�����Ǹ���λ��Ϊ0
	const int datalength = strlen((const char*)sourcedata);
	for (; i < datalength; i += 3) {
		// ÿ����һ�飬���б���
		// Ҫ��������ֵĵ�һ��
		trans_index = ((sourcedata[i] >> 2) & 0x3f);
		base64[j++] = base64char[(int)trans_index];
		// �ڶ���
		trans_index = ((sourcedata[i] << 4) & 0x30);
		if (i + 1 < datalength) {
			trans_index |= ((sourcedata[i + 1] >> 4) & 0x0f);
			base64[j++] = base64char[(int)trans_index];
		}
		else {
			base64[j++] = base64char[(int)trans_index];

			base64[j++] = padding_char;

			base64[j++] = padding_char;

			break;   // �����ܳ��ȣ�����ֱ��break
		}
		// ������
		trans_index = ((sourcedata[i + 1] << 2) & 0x3c);
		if (i + 2 < datalength) { // �еĻ���Ҫ����2��
			trans_index |= ((sourcedata[i + 2] >> 6) & 0x03);
			base64[j++] = base64char[(int)trans_index];

			trans_index = sourcedata[i + 2] & 0x3f;
			base64[j++] = base64char[(int)trans_index];
		}
		else {
			base64[j++] = base64char[(int)trans_index];

			base64[j++] = padding_char;

			break;
		}
	}

	base64[j] = '\0';

	return 0;
}

/** ���ַ����в�ѯ�ض��ַ�λ������
* const char *str ���ַ���
* char c��Ҫ���ҵ��ַ�
*/
inline int num_strchr(const char *str, char c) // 
{
	const char *pindex = strchr(str, c);
	if (NULL == pindex) {
		return -1;
	}
	return pindex - str;
}
/* ����
* const char * base64 ����
* unsigned char * dedata�� ����ָ�������
*/
int Base64DecodeA(const char * base64, unsigned char * dedata)
{
	int i = 0, j = 0;
	int trans[4] = { 0,0,0,0 };
	for (; base64[i] != '\0'; i += 4) {
		// ÿ�ĸ�һ�飬����������ַ�
		trans[0] = num_strchr(base64char, base64[i]);
		trans[1] = num_strchr(base64char, base64[i + 1]);
		// 1/3
		dedata[j++] = ((trans[0] << 2) & 0xfc) | ((trans[1] >> 4) & 0x03);

		if (base64[i + 2] == '=') {
			continue;
		}
		else {
			trans[2] = num_strchr(base64char, base64[i + 2]);
		}
		// 2/3
		dedata[j++] = ((trans[1] << 4) & 0xf0) | ((trans[2] >> 2) & 0x0f);

		if (base64[i + 3] == '=') {
			continue;
		}
		else {
			trans[3] = num_strchr(base64char, base64[i + 3]);
		}

		// 3/3
		dedata[j++] = ((trans[2] << 6) & 0xc0) | (trans[3] & 0x3f);
	}

	dedata[j] = '\0';

	return 0;
}




int Base64EncodeW(WCHAR s[], WCHAR b64[])
{
	char *cs, *cb64;
	int len = WideCharToMultiByte(CP_ACP, 0,s,_tcslen(s),0, 0, 0, 0);
	cs = malloc(len + 2);
	cb64 = malloc((len + 2)<<1);//��������
	WideCharToMultiByte(CP_ACP, 0, s, _tcslen(s), cs, len, 0, 0);
	cs[len] = 0;
	Base64EncodeA(cs, cb64);
	int lenc64= strlen(cb64);
	MultiByteToWideChar(CP_ACP, 0, cb64, lenc64, b64, lenc64);
	b64[lenc64] = '\0';
	free(cs);
	free(cb64);
	return 0;
}

int Base64DecodeW(WCHAR b64[], WCHAR s[])
{
	char *cs, *cb64;
	int len = WideCharToMultiByte(CP_ACP, 0, b64, _tcslen(b64), 0, 0, 0, 0);
	cs = malloc(len + 2);//��������
	cb64 = malloc(len + 2);
	WideCharToMultiByte(CP_ACP, 0, b64, len, cb64, len, 0, 0);
	cb64[len] = '\0';
	cb64[len + 1] = '\0';
	Base64DecodeA(cb64, cs);
	int lencs = strlen(cs);
	MultiByteToWideChar(CP_ACP, 0, cs, lencs, s, lencs);
	s[lencs] = '\0';
	free(cs);
	free(cb64);
	return 0;
}