#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>
#include <windows.h>
#include <TlHelp32.h>

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

void ListProcessInfo()
{
	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		_tprintf_s(_T("CreateToolhelp32Snapshot Error! \n"));
		return;
	}

	if (!Process32First(hProcessSnap, &pe32))
	{
		_tprintf_s(_T("Process32First Error! \n"));
		CloseHandle(hProcessSnap);
		return;
	}

	do
	{
		_tprintf_s(_T("%25s %5d \n"), pe32.szExeFile, pe32.th32ProcessID);
	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);
}

void KillProcess()
{
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);


	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		_tprintf_s(_T("CreateToolhelp32Snapshot Error! \n"));
		return;
	}

	if (!Process32First(hProcessSnap, &pe32))
	{
		_tprintf_s(_T("Process32First Error! \n"));
		CloseHandle(hProcessSnap);
		return;
	}
		
	HANDLE hProcess;
	BOOL isKill = FALSE;

	do
	{
		if (_tcscmp(pe32.szExeFile, cmdTokenList[1]) == 0)
		{
			hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
			
			if (hProcess != NULL)
			{
				TerminateProcess(hProcess, -1);
				isKill = TRUE;
			}
			CloseHandle(hProcess);
			break;
		}
	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);
	if (isKill == FALSE)
	{
		_tprintf_s(_T("Kill process fail, Try again!\n"));
	}
	return;
}
int PrintAllFileInCurrentDir()
{
	WIN32_FIND_DATA fileData;
	TCHAR curDirName[DIR_LEN];
	//TCHAR fileInCurDir[MAX_PATH];
	GetCurrentDirectory(DIR_LEN, curDirName);
	_tcsncat_s(curDirName, _T("\\*"), 3);
	HANDLE hFind = FindFirstFile(curDirName, &fileData);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		_tprintf_s(_T("Invalid file handle\n"));
		return -1;
	}
	else
	{
		_tprintf_s(_T("File in Current Directory List\n"));
		_tprintf_s(_T("%s\n"), fileData.cFileName);
		while (FindNextFile(hFind, &fileData))
		{
			_tprintf_s(_T("%s\n"), fileData.cFileName);
		}
		FindClose(hFind);

	}

	return 0;


}

bool IsSameFileInCurDir()
{
	WIN32_FIND_DATA fileData;
	TCHAR curDirName[DIR_LEN];
	TCHAR fileInCurDir[MAX_PATH];
	GetCurrentDirectory(DIR_LEN, curDirName);
	HANDLE hFind = FindFirstFile(fileInCurDir, &fileData);
	
	do 
	{
		if (cmdTokenList[1] == fileData.cFileName)
		{
			_tprintf_s(_T("Same Name Exist\n"));
			return 1;
		}

	} while (FindNextFile(hFind, &fileData));
	FindClose(hFind);

	return 0;
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
		ListProcessInfo();
	}
	else if (!_tcscmp(cmdTokenList[0], _T("kp")))
	{
		if (tokenNum < 2)
		{
			_tprintf_s(_T("usage: kp <process name> \n"));
			return 0;
		}
		KillProcess();
	}
	else if (!_tcscmp(cmdTokenList[0], _T("dir")))
	{
		PrintAllFileInCurrentDir();
	}
	else if (!_tcscmp(cmdTokenList[0], _T("mkdir")))
	{
		if (tokenNum < 2)
		{
			_tprintf_s(_T("usage: mkdir <dir name> \n"));
			return 0;
		}

		/*if (IsSameFileInCurDir())
		{
		return 0;
		}*/

		CreateDirectory(cmdTokenList[1], NULL);
	
	}
	else if (!_tcscmp(cmdTokenList[0], _T("rmdir")))
	{
		if (tokenNum < 2)
		{
			_tprintf_s(_T("usage: rmdir <dir name> \n"));
			return 0;
		}
		/*	if (!IsSameFileInCurDir())
			{
			("Dir doesn't exist\n");
			return 0;
			}*/
		//Path
		GetCurrentDirectory(DIR_LEN, cDir);
		_stprintf_s(cDir, _T("\%s"), cmdTokenList[1]);
		RemoveDirectory(cDir);
	}
	else if (!_tcscmp(cmdTokenList[0], _T("del")))
	{
		if (tokenNum < 2)
		{
			_tprintf_s(_T("usage: del <file name> \n"));
			return 0;
		}
		/*if (!IsSameFileInCurDir())
		{
		("File doesn't exist\n");
		return 0;
		}*/
		
		_tremove(cmdTokenList[1]);
		
	}
	else if (!_tcscmp(cmdTokenList[0], _T("ren")))
	{
		
		if (tokenNum < 3)
		{
			_tprintf_s(_T("usage: ren <cur name> <new name> \n"));
			return 0;
		}
		/*if (!IsSameFileInCurDir())
		{
			("File doesn't exist\n");
			return 0;
		}*/
		
		_trename(cmdTokenList[1], cmdTokenList[2]);
	
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