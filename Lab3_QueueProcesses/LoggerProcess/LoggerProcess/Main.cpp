#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#define FILENAME "D:\\LABS_BSUIR\\SPOVM\\Lab3_QueueProcesses\\queueText.txt"
bool logInFile();

int main() 
{
	HANDLE lSemaphore;
	lSemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, false, L"LogSemaphore");
	bool flag = true;
	while (flag)
	{
		WaitForSingleObject(lSemaphore, INFINITE);
		flag = logInFile();
		ReleaseSemaphore(lSemaphore, 1, NULL);
		printf("\n\n");
		Sleep(5000);
	}
	return 0;
}

bool logInFile()
{
	FILE *queueFile = fopen(FILENAME, "r");
	if (queueFile == NULL)
	{
		printf("can't connection to queue Pipe \n");
		return false;
	}
	printf("READ LOG\n");

	char tempBuffer[256];
	rewind(queueFile);
	//fseek(queueFile, 0, 0);
	do
	{
		fgets(tempBuffer, 256, queueFile);
		if (feof(queueFile))
		{
			break;
		}
		if (tempBuffer[0]=='\n')
		{
			printf("pizdec\n");
			continue;
		}

		puts(tempBuffer);
	} while (1);
	rewind(queueFile);
	fclose(queueFile);
	return true;
}