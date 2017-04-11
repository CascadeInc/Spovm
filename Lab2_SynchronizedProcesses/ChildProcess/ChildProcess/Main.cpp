#define CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <iostream>
#include <Windows.h>
#include <cstring>
#include <string>
#include <locale.h>
#include "Compiler.h"
#include <mutex>
using namespace std;


int main()
{
	char c;
	HANDLE hProcess;
	string str;
	HANDLE hEvent, hSemaphore;
	DWORD NumBytesToWrite;
	HANDLE hNamedPipe;
	LPTSTR PipeName = TEXT("\\\\.\\pipe\\TopPipeInDaWorld");

	//Открытие ивента
	hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"Event");
	if (hEvent == NULL) {
		cout << "FAILED" << endl;
		cout << "Input smth to exit" << endl;
		cin >> c;
		return GetLastError();
	}

	//Открытие семафора
	hSemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, false, L"Semaphore");
	cout << "Wait in a queue" << endl;
	WaitForSingleObject(hSemaphore, INFINITE);	//Ожидание семафора
	Sleep(5000);
	//Открытие канала
	hNamedPipe = CreateFile(					
		PipeName,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);
	system("cls");
	cout << "Now enter program you want to build:" << endl;
	getline(cin, str);
	cout << "\n" << str << endl;
	Sleep(500);
	
	/*if (TerminateProcess(hProcess, WAIT_OBJECT_0)) {
		PulseEvent(hEvent);
		ReleaseSemaphore(hSemaphore,1,NULL);
	}*/
	//Запись введенной строки в файл
	if (WriteFile(hNamedPipe, str.c_str(), str.length(), &NumBytesToWrite, NULL)) {
		cout << "Loaded sucessfully" << endl;
	}
	else
	{
		cout << "Can`t write" << endl;
		cout << GetLastError();
	}

	Sleep(100);
	ReleaseSemaphore(hSemaphore, 1, NULL);		//увеличение значения счетчика
	PulseEvent(hEvent);
	CloseHandle(hNamedPipe);					//Закрытие пайп
	return 0;

}





