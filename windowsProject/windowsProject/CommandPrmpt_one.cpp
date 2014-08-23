#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>
#include <windows.h>

#define DIR_LEN MAX_PATH+1
#define STR_LEN 256
#define CMD_TOKEN_NUM 10

TCHAR ERROR_CMD[] = _T("'%s'�� ������ �� �ִ� ���α׷��� �ƴմϴ�. \n");

TCHAR cmdString[STR_LEN];
TCHAR cmdTokenList[CMD_TOKEN_NUM][STR_LEN];
TCHAR seps[] = _T(" ,\t\n");

int CmdReadTokenize(void);
int CmdProcessing(int);
TCHAR* StrLower(TCHAR*);

int _tmain(int argc, TCHAR* argv[])
{
	_tsetlocale(LC_ALL, _T("Korean"));

	if (argc > 2)
	{
		for (int i = 1; i < argc; ++i)
		{
			_tcscpy_s(cmdTokenList[i - 1], argv[i]);
		}

		CmdProcessing(argc - 1);
	}



	DWORD isExit = NULL;
	while (1)
	{
		int tokenNum = CmdReadTokenize();
		
		if (tokenNum == 0) // Enter �Է� ó�� �κ�?!
		{
			continue;
		}

		isExit = CmdProcessing(tokenNum);
		if (isExit == TRUE)
		{
			_fputts(_T("��ɾ� ó���� �����մϴ�. \n"), stdout);
			break;
		}
	}
	return 0;
}


int CmdReadTokenize(void)
{
	TCHAR* token;
	TCHAR* nextToken;

	_fputts(_T("Best command prompt>> "), stdout);
	_getts_s(cmdString);

	token = _tcstok_s(cmdString, seps, &nextToken);

	int tokenNum = 0;

	while (token != NULL)
	{
		_tcscpy_s(cmdTokenList[tokenNum++], StrLower(token));
		token = _tcstok_s(NULL, seps, &nextToken);
	}

	return tokenNum;
}


int CmdProcessing(int tokenNum)
{

	BOOL isRun;
	STARTUPINFO si = { 0, };
	PROCESS_INFORMATION pi;
	TCHAR cDir[DIR_LEN];

	TCHAR cmdStringWithOptions[STR_LEN] = { 0, };
	TCHAR optString[STR_LEN] = { 0, };

	si.cb = sizeof(si);

	if (!_tcscmp(cmdTokenList[0], _T("exit")))
	{
		return TRUE;
	}
	else if (!_tcscmp(cmdTokenList[0], _T("start")))
	{
		//"start echo test good" �̶�� ���ڿ��� �ԷµǾ��ٰ� ��������
		

		if (tokenNum > 1)
		{
			//start�� ������ ������ ���ڿ��� �籸���Ѵ�

			for (int i = 1; i < tokenNum; ++i)
			{
				_stprintf_s(optString, _countof(optString),_T("%s %s"), optString, cmdTokenList[i]);
			}
			_stprintf_s(cmdStringWithOptions, _countof(optString), _T("%s %s"), 
					_T("windowsProject.exe"), optString);

		}
		else
		{
			//start�� �Է��ϴ� ���
			_stprintf_s(cmdStringWithOptions, _countof(cmdStringWithOptions), 
				_T("%s"), _T("windowsProject.exe"));
		}

			isRun = CreateProcess(NULL, cmdStringWithOptions, NULL, NULL, TRUE,
				CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);

			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
		
	}
	else if (!_tcscmp(cmdTokenList[0], _T("pwd")))
	{
		GetCurrentDirectory(DIR_LEN, cDir);
		_tprintf_s(_T("%s\n"), cDir);
	}
	else if (!_tcscmp(cmdTokenList[0], _T("echo")))
	{
		// �Էµ� ���ڿ� ���
		for (int i = 1; i < tokenNum; ++i)
		{
			_stprintf_s(optString,_countof(optString), _T("%s %s"), optString, cmdTokenList[i]);

		}
			_tprintf(_T("echo message: %s \n"), optString);
	}
	else if (!_tcscmp(cmdTokenList[0], _T("lp")))
	{

	}
	else
	{
		_tcscpy_s(cmdStringWithOptions, cmdTokenList[0]);

		for (int i = 1; i < tokenNum; ++i)
		{
			_stprintf_s(cmdStringWithOptions,_countof(cmdStringWithOptions),
				_T("%s %s"), cmdStringWithOptions, cmdTokenList[i]);

		}

		isRun = CreateProcess(NULL, cmdStringWithOptions, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);

		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);

		if (isRun == FALSE)
		{
			_tprintf_s(ERROR_CMD, cmdTokenList[0]);
		}
	}

	

	//_fputts(_T("Best command prompt >> "), stdout);
	//_getts_s(cmdString);

	//TCHAR* buffer;

	////���ڿ��� �ܾ������ �߶� ������ �Լ�, sep�� ���� ����ų� , �̰ų� \t �̰ų� \n
	//TCHAR* token = _tcstok_s(cmdString, seps, &buffer);

	

	//tokenNum = 0;

	////��ū ������ ��ū�� �ҹ��ڷ� ��ȯ�ؼ� ��ū����Ʈ�� �ϳ��� �־�߰ڴ�. 
	//while (token != NULL)
	//{
	//	_tcscpy_s(cmdTokenList[tokenNum++], StrLower(token));
	//	token = _tcstok_s(NULL, seps, &buffer);// �Լ��� ù��° �ҷ��� �� ���Դ� ���ڿ��� ������ �ִ�. 
	//										   // �׷��� ���ķδ� �׳� �߶� �ش�. (ù��° �ҷ��� ���� �ι�° �ҷ��� ���� �ٸ�����)
	//}

	//if (!_tcscmp(cmdTokenList[0], _T("exit")))
	//{
	//	return TRUE;
	//}
	//else if (!_tcscmp(cmdTokenList[0], _T("")))
	//{
	//	_tprintf_s(_T("\n"));
	//}

	//else if (!_tcscmp(cmdTokenList[0], _T("�߰� �Ǵ� ��ɾ� 2")))
	//{
	//}
	//else
	//{
	//	STARTUPINFO si = { 0, };
	//	PROCESS_INFORMATION pi;
	//	si.cb = sizeof(si);
	//	BOOL state = CreateProcess(NULL, cmdTokenList[0], NULL, NULL,
	//		TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);

	//	if (!state)
	//	{
	//		_tprintf(ERROR_CMD, cmdTokenList[0]);
	//	}
	//}

	return 0;
	
}

TCHAR* StrLower(TCHAR* pStr)
{
	TCHAR* ret = pStr;

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