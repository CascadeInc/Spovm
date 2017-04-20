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
#include "Compiler.h"
#include <mutex>
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
#define FILENAME "D:\\LABS_BSUIR\\SPOVM\\Lab3_QueueProcesses\\queueText.txt"
#define BUFFSIZE 512

int main(int argc, char *argv[])
{
#ifdef WIN32
	int num=0;
	HANDLE hSemaphore;
	HANDLE qSemaphore;
	HANDLE lSemaphore;
	HANDLE hEvent[2];
	HANDLE hNamedPipe;
	char Buff[BUFFSIZE];
	DWORD iNumBytesToRead = 512, i;
	BOOL fConnected;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	GetStartupInfo(&si);
	int flag;
	Compiler *comp = new Compiler(7);
	LPTSTR PipeName = TEXT("\\\\.\\pipe\\TopPipeInDaWorld");

	FILE * file = fopen(FILENAME, "w");
	fclose(file);
	CreateProcess(L"D:\\LABS_BSUIR\\SPOVM\\Lab3_QueueProcesses\\LoggerProcess\\Debug\\LoggerProcess.exe", NULL, FALSE, NULL, NULL, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
	while (1) {
		system("cls");
	
		//Создание семафора и ивента
		lSemaphore = CreateSemaphore(NULL, 1, 1, L"LogSemaphore");
		qSemaphore = CreateSemaphore(NULL, 1, 1, L"QueueSemaphore");
		hSemaphore = CreateSemaphore(NULL, 1, 1, L"Semaphore");
		hEvent[0] = CreateEvent(NULL, TRUE, FALSE, L"Event");
		hEvent[1] = CreateEvent(NULL, TRUE, FALSE, L"EventExit");

		//Запуск 3  процессов-клиентов
		for (int i = 0; i < 5; i++) 
		{
			Sleep(400);
			CreateProcess(L"D:\\LABS_BSUIR\\SPOVM\\Lab3_QueueProcesses\\ChildProcess\\Debug\\ChildProcess.exe", NULL, FALSE, NULL, NULL, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
			
		}
		
		
		//создание пайпа
		if (hNamedPipe = CreateNamedPipe(
			PipeName,
			PIPE_ACCESS_DUPLEX,
			PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
			PIPE_UNLIMITED_INSTANCES,
			512, 512, NMPWAIT_USE_DEFAULT_WAIT, NULL)) {
			int n = 5;
			for (int i = 0; i < n; i++)
			{
				cout << "Connecting to client....." << endl;
				//Присоединение сервера к пайпу
				fConnected = FALSE;
				fConnected = ConnectNamedPipe(hNamedPipe, NULL);	//Состояние ожидания коннекта с клиентским процессом
				(fConnected) ? cout << "Connected" << endl : cout << "Not connected" << endl;
				cout << endl;
				//ReleaseSemaphore(hSemaphore, 1, NULL);
				//Ожидание события от клиентского процесса и дальнейшее считывание из пайпа
				while (WaitForMultipleObjects(2, hEvent, FALSE, INFINITE) == WAIT_OBJECT_0 + 1)
				{
					n--;
					if (n == 0)
						break;
				}
				if (ReadFile(hNamedPipe, Buff, iNumBytesToRead, (LPDWORD)&num, NULL))
				{
					cout << "Client`s string:" << endl;
					for (int i = 0; i < num; i++)
					{
						cout << Buff[i];
					}
					cout << endl;
				}
				//Проверка на возможность открыть данный тип файлов
				std::string str(Buff, num);
				if (comp->cmp(str))
				{
					str = "Can`t open this format....\n";
					cout << str;
				}

				else if (comp->getCommand(str))
				{
					str = "Program opened...\n";
					cout << str;
				}
				else
				{
					str = "Not supported...\n";
					cout << str;
				}
				cout << endl;
				cout << "-------------------------------------------------------------" << endl;
				cout << endl;
				DisconnectNamedPipe(hNamedPipe);	//дисконнект от пайпа	
			}
		}
		CloseHandle(hNamedPipe);				//закрытие пайпа
		CloseHandle(hSemaphore);
		Sleep(3000);
		system("cls");
		CloseHandle(hEvent);
		cout << "Do you want to continue?" << endl;
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


