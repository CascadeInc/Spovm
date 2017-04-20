#include <stdio.h>
#include <windows.h>
#include <locale.h>
#include <conio.h>
#include <vector>
#include <process.h>
#include <iostream>
using namespace std;

#define BUFFSIZE 256
#define FILENAME "D:\\LABS_BSUIR\\SPOVM\\Lab4_Threads\\threads.bin"
#define TIMEFILENAME "D:\\LABS_BSUIR\\SPOVM\\Lab4_Threads\\timer.bin"


int main(int argc, char *argv[])
{
	
	int n = atoi(argv[1]);
	
	FILE *file = fopen(TIMEFILENAME, "w+b");
	if (file == NULL)
	{
		return -1;
	}
	fwrite(&n, sizeof(int), 1, file);
	rewind(file);
	fclose(file);
	HANDLE hSemaphore;
	HANDLE hEvent;
	HANDLE hThread;
	DWORD dwThreadId, dwThrdParam = 1;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	GetStartupInfo(&si);

	FILE *file1 = fopen(FILENAME, "w+b");                             // creating file
	fclose(file1);

	HANDLE hEvents[3];
	hEvents[0] = CreateEvent(NULL, TRUE, FALSE, L"Create");
	hEvents[1] = CreateEvent(NULL, TRUE, FALSE, L"Delete");
	hEvents[2] = CreateEvent(NULL, TRUE, FALSE, L"Terminate");
	
	hSemaphore= CreateSemaphore(NULL, 1, 1, L"Semaphore");
	if (!hSemaphore)
	{
		printf("unable to open semaphore");
		return -1;
	}
	

	int out;
	out = atoi(argv[2]);									//time for out all threads

	CreateProcess(L"D:\\LABS_BSUIR\\SPOVM\\Lab4_Threads\\Timer\\Debug\\Timer.exe", NULL, FALSE, NULL, NULL, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
	CreateProcess(L"D:\\LABS_BSUIR\\SPOVM\\Lab4_Threads\\ActionThreads\\Debug\\ActionThreads.exe", NULL, FALSE, NULL, NULL, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
	CreateProcess(L"D:\\LABS_BSUIR\\SPOVM\\Lab4_Threads\\Cheker\\Debug\\Cheker.exe", NULL, FALSE, NULL, NULL, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
		
	vector <DWORD> threadList;                           // lists for saving name of threads
	vector <int> intList;
	
	while (1)
	{
		printf("wait main semaphore %d\n", (int)hSemaphore);
		WaitForSingleObject(hSemaphore, INFINITE);                                // waiting for reading
		printf("reading\n");
		FILE *file = fopen(FILENAME, "rb");                       // open file to read
		if (file == NULL)
		{
			break;
		}
		while (!feof(file))
		{
			int i;
			if (!fread(&i, sizeof(int), 1, file))                   // read int from file
			{
				break;
			}

			if (!feof(file))
			{
				cout << "Thread " << i << " is working\n";
			}
			else
			{
				break;
			}
		}
		rewind(file);
		fclose(file);
		cout << "\n------------------------------\n";
		ReleaseSemaphore(hSemaphore, 1, NULL);
		Sleep(out);                                              // wait out seconds
	}
	return 0;
}