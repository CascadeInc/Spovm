#ifdef WIN32
#define CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <cstring>
#include <conio.h>
#include <process.h>
#include <locale.h>
#include <fstream>
#include <stdlib.h>
#endif
#ifdef linux
#include <QCoreApplication>
#include "compiler.h"
#include "programmer.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void read_from_pipe(int file, char *str)                 //read from pipe
{
	int len = 0;
	char c;
	do
	{
		read(file, &c, 1);                                  //read one char
		str[len] = c;
		len++;
	} while (c != '\0'&& c != EOF);                         //while not end of string
	(str)[len] = '\0';
}

void clean_stdin(void)
{
	int c;
	do {
		c = getchar();
	} while (c != '\n' && c != EOF);
}


/* Write some random text to the pipe. */

void write_to_pipe(int file, char* msg)                //write messege in pipe
{
	write(file, msg, strlen(msg) + 1);
	return;
}

#endif 

using namespace std;
#define BUFFSIZE 256



int main(int argc, char *argv[])
{
#ifdef WIN32
	BOOL fConnected;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	GetStartupInfo(&si);
	int flag;

	HANDLE hNamedPipe;

	

	while (1) {

		LPTSTR PipeName = TEXT("\\\\.\\pipe\\TopPipeInDaWorld");

		hNamedPipe = CreateNamedPipe(
			PipeName,
			PIPE_ACCESS_DUPLEX,
			PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
			PIPE_UNLIMITED_INSTANCES,
			512, 512, NMPWAIT_USE_DEFAULT_WAIT, NULL);


		CreateProcess(L"D:\\LABS_BSUIR\\SPOVM\\Lab1_ProcessesAndPipes\\ChildProcess\\Debug\\ChildProcess.exe", NULL, FALSE, NULL, NULL, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);

		ConnectNamedPipe(hNamedPipe, NULL);

		DWORD NumBytesToWrite;				
		DWORD dWait = -1;
		string str;
		system("cls");
		cout << "Enter program you want to build:" << endl;
		getline(cin, str);
		WriteFile(hNamedPipe, str.c_str(), str.length(), &NumBytesToWrite, NULL);

		while (dWait != WAIT_OBJECT_0)
		{
			
			system("cls");
			cout << "Waiting..." << endl;
			dWait = WaitForSingleObject(pi.hProcess, INFINITE);
		}
		system("cls");

		DWORD iNumBytesToRead;
		char   Buff[BUFFSIZE];

		ReadFile(hNamedPipe, Buff, 512, &iNumBytesToRead, NULL);

		int i;
		for (i = 0; i < iNumBytesToRead; i++)
		{
			cout << Buff[i];
		}
		printf("\n");
		printf("Do you want to continue?\n");
		cin >> flag;
		cin.ignore(1, '\n');
		if (flag != 1)
			break;
	}
}
#endif

#ifdef linux

	QCoreApplication a(argc, argv);
	printf("run\n");
	pid_t pid;                                          // id of process
														//int station;                                        // station of process (for wait)

	int mypipe[2];                                      // creating pipes

														/* Create the pipe. */
	if (pipe(mypipe))                                  // if creting pipes
	{
		fprintf(stderr, "Pipe failed.\n");
		return -1;
	}

	switch (pid = fork())                                 // create process
	{
	case -1:
	{
		perror("fork");
		exit(1);
	}
	case 0:
	{
		// ----------------CHILD PROCESS---------------------------
		int signo;
		sigset_t newmask;
		if (argc != 2)
			/* открытие очереди, получение атрибутов, выделение буфера */
			sigemptyset(&newmask);
		sigaddset(&newmask, SIGUSR1);
		sigaddset(&newmask, SIGUSR2);
		sigprocmask(SIG_BLOCK, &newmask, NULL); /* блокируем SIGUSR1 */
												/* установка обработчика, включение уведомления */


		char choice = '1';
		char *str = new char[BUFFSIZE];
		while (choice == '1')
		{
			close(mypipe[0]);                   // close 0 end
			printf("\ninput command:\n");
			gets(str);                        // get inquery
			write_to_pipe(mypipe[1], str);    //write in pipe
			kill(getppid(), SIGUSR1);



			sigwait(&newmask, &signo);
			if (signo == SIGUSR1)
			{
				printf("\nget answer - YES\n");
			}
			else
			{
				printf("\nget answer - NO\n");
			}
			printf("\ninput 1 for continue\n");
			scanf("%c", &choice);
			clean_stdin();
		}
		kill(getppid(), SIGUSR2);
		return 0;
	}
	default:
		//---------------PARENT PROCESS------------------------
	{
		Compiler* res = new Compiler(6);     // 6 commands
											 /* This is the parent process.
											 Close other end first. */




		int signo;
		sigset_t newmask;
		if (argc != 2)
			/* открытие очереди, получение атрибутов, выделение буфера */
			sigemptyset(&newmask);
		sigaddset(&newmask, SIGUSR1);
		sigaddset(&newmask, SIGUSR2);
		sigprocmask(SIG_BLOCK, &newmask, NULL); /* блокируем SIGUSR1 */
												/* установка обработчика, включение уведомления */
		char *str = new char[BUFFSIZE];

		while (1)
		{
			sigwait(&newmask, &signo);
			if (signo == SIGUSR1)
			{
				printf("get signal\n");
				close(mypipe[1]);                   // close 1 end
				read_from_pipe(mypipe[0], str);
				printf("%s -- ", str);
				if (res->getCommand(str))                //result
				{
					printf("YES\n");
					kill(pid, SIGUSR1);
				}
				else
				{
					printf("NO\n");
					kill(pid, SIGUSR2);
				}
			}
			else
			{
				break;
			}
		}
		return 0;
	}
	}
	return a.exec();
}

#endif


