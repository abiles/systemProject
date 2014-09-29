// sort.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#define STR_NUM 200
#define STR_LEN 200

void Sorting()
{
	TCHAR strArr[STR_NUM][STR_LEN] = { 0, };

	int strNum;

	for (strNum = 0; strNum < STR_NUM; ++strNum)
	{
		TCHAR* isEOF = _fgetts(strArr[strNum], STR_LEN, stdin);
		if (isEOF == NULL)
		{
			break;
		}
	}

	TCHAR strTemp[STR_NUM];

	for (int i = 0; i < strNum; ++i)
	{
		for (int j = strNum - 1; j > i; --j)
		{
			if ((_tcscmp(strArr[j - 1], strArr[j]) > 0))
			{
				_tcscpy_s(strTemp, strArr[j - 1]);
				_tcscpy_s(strArr[j - 1], strArr[j]);
				_tcscpy_s(strArr[j], strTemp);
			}
		}
	}

	for (int i = 0; i < strNum; ++i)
	{
		_fputts(strArr[i], stdout);
	}

}


int _tmain(int argc, _TCHAR* argv[])
{

	Sorting();
	return 0;
}

