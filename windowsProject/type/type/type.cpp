// type.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#define BUFFERSIZE 1024

void TYPE(TCHAR* fileName)
{
	_ASSERT(fileName);
	
	TCHAR buf[BUFFERSIZE];
	DWORD numberOfByteRead = 0;
	errno_t err;
	FILE* filePtr;
	//내용 출력하는  함수
	err = _tfopen_s(&filePtr,fileName, _T("rt"));

	if (err == 0)
	{
		while (_fgetts(buf, BUFFERSIZE, filePtr))
		{
			_fputts(buf, stdout);
		}
	}
	else
	{
		_tprintf_s(_T("The file '%s' was not opened\n"), fileName);
	}

	

	return;
	

}


int _tmain(int argc, _TCHAR* argv[])
{
	if (argc < 2)
	{
		return -1;
	}

	TYPE(argv[1]);
	return 0;
}

