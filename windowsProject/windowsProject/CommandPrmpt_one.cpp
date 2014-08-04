#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>
#include <windows.h>

#define DIR_LEN MAX_PATH+1
#define STR_LEN 256
#define CMD_TOKEN_NUM 10

TCHAR ERROR_CMD[] = _T("'%s'�� ������ �� �ִ� ���α׷��� �ƴմϴ�. \n");

int CmdProcessing(void);
TCHAR * StrLower(TCHAR *);

int _tmain(int argc, TCHAR * argv[])
{
	_tsetlocale(LC_ALL, _T("Korean"));

	DWORD isExit;
	while (1)
	{
		isExit = CmdProcessing();
		if (isExit == TRUE)
		{
			_fputts(_T("��ɾ� ó���� �����մϴ�. \n"), stdout);
			break;
		}
	}
	return 0;
}

TCHAR cmdString[STR_LEN];
TCHAR cmdTokenList[CMD_TOKEN_NUM][STR_LEN];
TCHAR seps[] = _T(" ,\t\n");

int CmdProcessing(void)
{
	_fputts(_T("Best command prompt >> "), stdout);
	_getts_s(cmdString);

	TCHAR* buffer;

	//���ڿ��� �ܾ������ �߶� ������ �Լ�, sep�� ���� ����ų� , �̰ų� \t �̰ų� \n
	TCHAR* token = _tcstok_s(cmdString, seps, &buffer);

	TCHAR cDir[DIR_LEN];

	int tokenNum = 0;

	//��ū ������ ��ū�� �ҹ��ڷ� ��ȯ�ؼ� ��ū����Ʈ�� �ϳ��� �־�߰ڴ�. 
	while (token != NULL)
	{
		_tcscpy_s(cmdTokenList[tokenNum++], StrLower(token));
		token = _tcstok_s(NULL, seps, &buffer);// �Լ��� ù��° �ҷ��� �� ���Դ� ���ڿ��� ������ �ִ�. 
											   // �׷��� ���ķδ� �׳� �߶� �ش�. (ù��° �ҷ��� ���� �ι�° �ҷ��� ���� �ٸ�����)
	}

	if (!_tcscmp(cmdTokenList[0], _T("exit")))
	{
		return TRUE;
	}
	else if (!_tcscmp(cmdTokenList[0], _T("")))
	{
		_tprintf_s(_T("\n"));
	}
	else if (!_tcscmp(cmdTokenList[0], _T("pwd")))
	{
		GetCurrentDirectory(DIR_LEN, cDir);
		_tprintf_s(_T("%s\n"), cDir);
	}
	else if (!_tcscmp(cmdTokenList[0], _T("�߰� �Ǵ� ��ɾ� 2")))
	{
	}
	else
	{
		STARTUPINFO si = { 0, };
		PROCESS_INFORMATION pi;
		si.cb = sizeof(si);
		BOOL state = CreateProcess(NULL, cmdTokenList[0], NULL, NULL,
			TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);

		if (!state)
		{
			_tprintf(ERROR_CMD, cmdTokenList[0]);
		}
	}

	return 0;
	
}

TCHAR * StrLower(TCHAR *pStr)
{
	TCHAR *ret = pStr;

	while (*pStr)
	{
		if (_istupper(*pStr))
		{
			*pStr = _totlower(*pStr);
		}
		pStr++;
	}

	return ret;
}