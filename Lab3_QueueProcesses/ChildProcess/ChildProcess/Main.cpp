#define CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <iostream>
#include <Windows.h>
#include <cstring>
#include <string>
#include <locale.h>
#include "Compiler.h"
#include <mutex>

#define FILENAME "D:\\LABS_BSUIR\\SPOVM\\Lab3_QueueProcesses\\queueText.txt"
#define TEMPFILE "D:\\LABS_BSUIR\\SPOVM\\Lab3_QueueProcesses\\queueTextTemp.txt"

using namespace std;
void logIn();
bool logOut(char*);
BOOL CtrlHandler(DWORD);


int main()
{
	char c;
	HANDLE hProcess;
	string str;
	HANDLE hEvent;
	HANDLE hSemaphore;
	DWORD NumBytesToWrite;
	HANDLE hNamedPipe;
	HANDLE qSemaphore;
	HANDLE lSemaphore;
	LPTSTR PipeName = TEXT("\\\\.\\pipe\\TopPipeInDaWorld");

	

	//Открытие ивента
	hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"Event");
	if (hEvent == NULL) {
		cout << "FAILED" << endl;
		cout << "Input smth to exit" << endl;
		cin >> c;
		return GetLastError();
	}

	

	//Открытие семафоров
	hSemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, false, L"Semaphore");
	qSemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, false, L"QueueSemaphore");
	lSemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, false, L"LogSemaphore");
	
	WaitForSingleObject(qSemaphore, INFINITE);
	logIn();
	ReleaseSemaphore(qSemaphore, 1, NULL);
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);
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
	WaitForSingleObject(lSemaphore, INFINITE);
	logOut(FILENAME);
	ReleaseSemaphore(lSemaphore, 1, NULL);
	ReleaseSemaphore(hSemaphore, 1, NULL);		//увеличение значения счетчика
	PulseEvent(hEvent);
	CloseHandle(hNamedPipe);					//Закрытие пайп
	return 0;

}

void logIn()
{
	FILE * file = fopen(FILENAME, "a");
	printf("logIn - %d\n",(int)GetCurrentProcessId());
	//Sleep(4000);
	fprintf(file,"%d\n",GetCurrentProcessId());
	printf("logIn succesfully\n");
	fclose(file);
}

bool logOut(char* fileName)
{
	FILE *file = fopen(fileName, "r");
	if (file == NULL)
	{
		return false;
	}
	printf("logOut\n");
	FILE *tempFile = fopen(TEMPFILE, "w+");
	if (tempFile == NULL)
	{
		printf("can't create file\n");
		fclose(file);
		return false;
	}
	int idBuffer;
	char tempBuffer[256];
	rewind(file);
	//fseek(file, 0, 0);
	do
	{
		fgets(tempBuffer, 256, file);
		if (feof(file))
		{
			break;
		}
		//fseek(this->pipe,1,1);
		if (tempBuffer == NULL)
		{
			printf("pizdec\n");
		}
		idBuffer = atoi(tempBuffer);
		printf("read from pipe - %ld\n", ftell(file));
		//puts(tempBuffer);
		if (idBuffer != (int)GetCurrentProcessId())
		{
			fputs(std::to_string(idBuffer).c_str(), tempFile);
			fputs("\n", tempFile);
		}
	} while (1);

	//printf("pos in file - %ld", ftell(this->pipe));
	rewind(file);
	fclose(tempFile);
	fclose(file);
	remove(fileName);
	rename(TEMPFILE, fileName);
	printf("remove and rename\n");
	return true;
}


BOOL CtrlHandler(DWORD fdwCtrlType)
{
	HANDLE hEvent;
	hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"EventExit");
	if (hEvent == NULL) {
		cout << "FAILED" << endl;
		cout << "Input smth to exit" << endl;
		//cin >> c;
		return GetLastError();
	}
	HANDLE hSemaphore;
	hSemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, false, L"Semaphore");
	HANDLE lSemaphore;
	
	switch (fdwCtrlType)
	{
		// Handle the CTRL-C signal. 
	case CTRL_C_EVENT:
		printf("Ctrl-C event\n\n");
		Beep(750, 300);
		return(TRUE);

		// CTRL-CLOSE: confirm that the user wants to exit. 
	case CTRL_CLOSE_EVENT:
		Beep(222, 888);
		printf("Ctrl-Close event\n\n");
		//ReleaseSemaphore(hSemaphore, 1, NULL);
		lSemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, false, L"LogSemaphore");
		WaitForSingleObject(lSemaphore, INFINITE);
		logOut(FILENAME);
		ReleaseSemaphore(lSemaphore, 1, NULL);
		PulseEvent(hEvent);
		return(TRUE);

		// Pass other signals to the next handler. 
	case CTRL_BREAK_EVENT:
		Beep(900, 200);
		printf("Ctrl-Break event\n\n");
		return FALSE;

	case CTRL_LOGOFF_EVENT:
		Beep(1000, 200);
		printf("Ctrl-Logoff event\n\n");
		return FALSE;

	case CTRL_SHUTDOWN_EVENT:
		Beep(750, 500);
		printf("Ctrl-Shutdown event\n\n");
		return FALSE;

	default:
		return FALSE;
	}
}




