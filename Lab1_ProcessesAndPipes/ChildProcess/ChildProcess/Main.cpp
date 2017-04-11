#define CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <iostream>
#include <Windows.h>
#include <cstring>
#include <string>
#include <locale.h>
#include "Compiler.h"
using namespace std;
#define BUFFSIZE 256

int main()
{
	HANDLE hNamedPipe;
	char Buff[BUFFSIZE];
	DWORD iNumBytesToRead = 512, i;
	LPTSTR PipeName = TEXT("\\\\.\\pipe\\TopPipeInDaWorld");
	Compiler *comp = new Compiler(7);

	hNamedPipe = CreateFile(
		PipeName,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);

	ReadFile(hNamedPipe, Buff, iNumBytesToRead, &iNumBytesToRead, NULL);

	std::string str(Buff, iNumBytesToRead);
	cout << str;
	printf("\n");

	DWORD  NumBytesToWrite;
	if (comp->cmp(str))
	{
		str = "Can`t open this format....\n";
		WriteFile(hNamedPipe, str.c_str(), str.length(), &NumBytesToWrite, NULL);
	}
	if (comp->getCommand(str)) {
		str = "Program opened...\n";
		WriteFile(hNamedPipe, str.c_str(), str.length(), &NumBytesToWrite, NULL);
	}
	else
	{
		str = "Not supported...\n";
		WriteFile(hNamedPipe, str.c_str(), str.length(), &NumBytesToWrite, NULL);
	}

	Sleep(5000);
	ExitProcess(WAIT_OBJECT_0);
	return 0;

}





