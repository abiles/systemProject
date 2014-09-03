// DeadLock.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <Windows.h>
#include <process.h>

#define NUM_PLAYER 3

struct Player
{
	int m_PlyaerId;
	int m_HP;
	CRITICAL_SECTION m_Lock;
};

Player GameData[NUM_PLAYER];

void Attack(Player* from, Player* to)
{
	//Attack 총 횟수를 알아보기 위해서 
	static int attackNum = 1;

	//임계영역에 들어서기 전에 키를 모두 반납하도록 함
	for (int i = 0; i < NUM_PLAYER; ++i)
	{
		LeaveCriticalSection(&GameData[i].m_Lock);
	}

	
	_tprintf_s(_T("BEGIN %d : from %d to %d\n"), attackNum++,from->m_PlyaerId, to->m_PlyaerId);

	EnterCriticalSection(&from->m_Lock);
	_tprintf_s(_T("--from #%d enter critical section \n"), from->m_PlyaerId);
	EnterCriticalSection(&to->m_Lock);
	_tprintf_s(_T("--to #%d enter critical section \n"), to->m_PlyaerId);
	to->m_HP -= 100;
	from->m_HP += 70;

	Sleep(100);
	LeaveCriticalSection(&to->m_Lock);
	_tprintf_s(_T("---to #%d Leave critical section \n"), to->m_PlyaerId);
	LeaveCriticalSection(&from->m_Lock);
	_tprintf_s(_T("---from #%d Leave critical section \n"), from->m_PlyaerId);
}

unsigned int WINAPI ThreadProc(LPVOID lpparam)
{
	srand(GetCurrentThreadId());
	int from = (int)lpparam;

	for (int i = 0; i < 10; ++i)
	{
		int to = 0;
		while (true)
		{
			to = rand() % NUM_PLAYER;
			if (from != to)
			{
				break;
			}
		}

		Attack(&GameData[from], &GameData[to]);
	}
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
		
	DWORD dwThreadId[NUM_PLAYER];
	HANDLE hThread[NUM_PLAYER];

	for (int i = 0; i < NUM_PLAYER; ++i)
	{
		GameData[i].m_PlyaerId = i + 1001;
		GameData[i].m_HP = 2000;
		InitializeCriticalSection(&GameData[i].m_Lock);
	}

	for (DWORD i = 0; i < NUM_PLAYER; ++i)
	{
		hThread[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, (LPVOID)i, CREATE_SUSPENDED, (unsigned int *)&dwThreadId[i]);
	}

	_tprintf_s(_T("==========BEGIN=======\n"));
	for (DWORD i = 0; i < NUM_PLAYER; ++i)
	{
		ResumeThread(hThread[i]);
	}

	WaitForMultipleObjects(NUM_PLAYER, hThread, TRUE, INFINITE);

	_tprintf_s(_T("==========RESULT=========\n"));

	for (DWORD i = 0; i < NUM_PLAYER; ++i)
	{
		_tprintf_s(_T("--Played# %d, HP: %d\n"), GameData[i].m_PlyaerId, GameData[i].m_HP);
		DeleteCriticalSection(&(GameData[i].m_Lock));
	}

	for (DWORD i = 0; i < NUM_PLAYER; ++i)
	{
		CloseHandle((hThread[i]));
	}

	getchar();
	return 0;
}

