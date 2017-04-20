#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>
#include <locale.h>
#include <conio.h>
#include <vector>
#include <process.h>
#include <iostream>
using namespace std;

#define FILENAME "D:\\LABS_BSUIR\\SPOVM\\Lab4_Threads\\threads.bin"


DWORD WINAPI ThreadFunc(LPVOID lpParam)
{
	while (1)
	{
		Sleep(5000);
	}
}


int main()
{
	printf("POTOKIIIIIIIIIIIIIIII\n");
	int id = 0;
	HANDLE hSemaphore;
	HANDLE hEvent[3];
	hEvent[0] = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"Create");
	hEvent[1] = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"Delete");
	hEvent[2] = OpenEvent(EVENT_ALL_ACCESS, FALSE, L"Terminate");
	hSemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, false, L"Semaphore");
	vector <DWORD> threadList;                           // lists for saving name of threads
	vector <int> intList;
	bool flag = true;
	while (flag)
	{
		DWORD dWait;                               // wait the signal
		dWait = WaitForMultipleObjects(3, hEvent, FALSE, INFINITE);
		if (dWait == WAIT_OBJECT_0 + 0)
		{
			printf("create\n");
			HANDLE hThread;
			DWORD dwThreadId, dwThrdParam = 1;
			hThread = CreateThread(NULL, 0, ThreadFunc, &dwThrdParam, 0, &dwThreadId); // creating new thread
			threadList.push_back(dwThreadId);                      // adding thread id in the end of list
			id++;
			intList.push_back(id);                               // adding name in the end of list

			WaitForSingleObject(hSemaphore, INFINITE);                            // waiting for writing in file
			printf("writing\n");

			FILE *file;
			file = fopen(FILENAME, "wb");                         // open file for writting
			for (int i = 0; i < intList.size(); i++)
			{
				int temp = intList[i];
				fwrite(&temp, sizeof(int), 1, file);                // writing all list in file
			}
			rewind(file);
			fclose(file);
			ReleaseSemaphore(hSemaphore, 1, NULL);

		}
		if (dWait == WAIT_OBJECT_0 + 1)
		{
			printf("delete\n");
			if (id > 0)
			{
				TerminateThread((HANDLE)threadList[threadList.size()-1],0);       // close thread
				threadList.pop_back();                               // delete last element
				id--;
				intList.pop_back();
				WaitForSingleObject(hSemaphore, INFINITE);
				printf("writing\n");

				FILE *file;
				file = fopen(FILENAME, "wb");                         // write in file
				for (int i = 0; i < intList.size(); i++)
				{
					int temp = intList[i];
					fwrite(&temp, sizeof(int), 1, file);
				}
				rewind(file);
				fclose(file);
				ReleaseSemaphore(hSemaphore, 1, NULL);
			}
		}
		if (dWait == WAIT_OBJECT_0 + 2)
		{
			printf("exit\n");

			for (int i = 0; i < threadList.size(); i++)
			{
				TerminateThread((HANDLE)threadList[i],0);
			}
			return 0;
		}
	}
}