#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>
#include <windows.h>

#define DIR_LEN MAX_PATH+1
#define STR_LEN 256
#define CMD_TOKEN_NUM 10

TCHAR ERROR_CMD[] = _T("'%s'는 실행할 수 있는 프로그램이 아닙니다. \n");

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
		
		if (tokenNum == 0) // Enter 입력 처리 부분?!
		{
			continue;
		}

		isExit = CmdProcessing(tokenNum);
		if (isExit == TRUE)
		{
			_fputts(_T("명령어 처리를 종료합니다. \n"), stdout);
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
		//"start echo test good" 이라는 문자열이 입력되었다고 가정하자
		

		if (tokenNum > 1)
		{
			//start를 제외한 나머지 문자열을 재구성한다

			for (int i = 1; i < tokenNum; ++i)
			{
				_stprintf_s(optString, _countof(optString),_T("%s %s"), optString, cmdTokenList[i]);
			}
			_stprintf_s(cmdStringWithOptions, _countof(optString), _T("%s %s"), 
					_T("windowsProject.exe"), optString);

		}
		else
		{
			//start만 입력하는 경우
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
		// 입력된 문자열 출력
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

	////문자열을 단어단위로 잘라서 꺼내는 함수, sep은 구분 띄어스기거나 , 이거나 \t 이거나 \n
	//TCHAR* token = _tcstok_s(cmdString, seps, &buffer);

	

	//tokenNum = 0;

	////토큰 있으면 토큰을 소문자로 변환해서 토큰리스트에 하나씩 넣어야겠다. 
	//while (token != NULL)
	//{
	//	_tcscpy_s(cmdTokenList[tokenNum++], StrLower(token));
	//	token = _tcstok_s(NULL, seps, &buffer);// 함수가 첫번째 불렸을 때 나왔던 문자열을 가지고 있다. 
	//										   // 그래서 이후로는 그냥 잘라서 준다. (첫번째 불렸을 때랑 두번째 불렸을 때가 다른거지)
	//}

	//if (!_tcscmp(cmdTokenList[0], _T("exit")))
	//{
	//	return TRUE;
	//}
	//else if (!_tcscmp(cmdTokenList[0], _T("")))
	//{
	//	_tprintf_s(_T("\n"));
	//}

	//else if (!_tcscmp(cmdTokenList[0], _T("추가 되는 명령어 2")))
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