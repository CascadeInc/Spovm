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
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>

#define PIPE_NAME "/home/alexey/Lab_1/fifofile"
#define SEMAPHORE_NAME "/my_named_semaphore"
#define QUEUE_PIPE "/home/alexey/Lab_1/queuefile"
#define QUEUE_SEMAPHORE "/queue_semaphore"
#define QUEUE_FILE_SEMAPHORE "/queueFile_semaphore"

#endif

using namespace std;

#define BUFFSIZE 256
#define CLIENT_COUNT 3                                               // the count of the clients

int getInt()
{
    sigset_t newmask;                                                // mask for signals
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGUSR1);
    sigaddset(&newmask, SIGUSR2);
    sigaddset(&newmask, SIGINT);
    sigprocmask(SIG_BLOCK, &newmask, NULL);

    int power = 1;
    int sum = 0;
    int getSignal;

    bool flag = true;

    while (flag)
    {
        sigwait(&newmask,&getSignal);
        switch(getSignal)
        {
        case SIGUSR1:
        {
            printf("1");
            sum += power;
            power = power << 1;
            break;
        }
        case SIGUSR2:
        {
            printf("0");
            power = power << 1;
            break;
        }
        case SIGINT:
        {
            flag = false;
            break;
        }
        default:
        {
            break;
        }
        }
    }
    printf("\n");
    return sum;
}


int main(int argc, char *argv[])

#ifdef WIN32
{
    BOOL fConnected;
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));
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
        cout << "Enter program you want to built: " << endl;
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
#endif

#ifdef linux
    {

        //-----------------------MANIPULATOR PROCESS-------------------------------

        int signo;
        sigset_t newmask;                                                // mask for signals
        sigemptyset(&newmask);
        sigaddset(&newmask, SIGUSR1);
        sigaddset(&newmask, SIGUSR2);
        sigprocmask(SIG_BLOCK, &newmask, NULL);

        unlink(PIPE_NAME);
        mkfifo(PIPE_NAME, O_RDWR);                                       // сreating pipe for communicating between clients and server
        unlink(PIPE_NAME);

        unlink(QUEUE_PIPE);
        mkfifo(QUEUE_PIPE, O_RDWR);                                       // сreating pipe for communicating between clients and server
        unlink(QUEUE_PIPE);

        sem_t *mutex, *queueMutex, *queueFileMutex;                                                    // declare semaphore
        int value = 0;
        mutex = sem_open(SEMAPHORE_NAME,O_CREAT,0644,0);                 // creating semaphore (mutex)
        if(mutex == SEM_FAILED)
        {
            perror("unable to create semaphore");
            sem_unlink(SEMAPHORE_NAME);                                  // destroying semaphore
            return -1;
        }
        value = 0;
        sem_trywait(mutex);
        sem_getvalue(mutex,&value);
        printf("main semaphore value - %d\n",value);                                                 // release semaphore
        sem_post(mutex);
        queueMutex = sem_open(QUEUE_SEMAPHORE,O_CREAT,0644,0);                 // creating semaphore (mutex)
        if(queueMutex == SEM_FAILED)
        {
            perror("unable to create semaphore");
            sem_unlink(QUEUE_SEMAPHORE);                                  // destroying semaphore
            return -1;
        }
        sem_trywait(queueMutex);
        sem_getvalue(queueMutex,&value);
        printf("queue semaphore value - %d\n",value);
        sem_post(queueMutex);
        queueFileMutex = sem_open(QUEUE_FILE_SEMAPHORE,O_CREAT,0644,0);                 // creating semaphore (mutex)
        if(queueFileMutex == SEM_FAILED)
        {
            perror("unable to open semaphore");
            sem_unlink(QUEUE_FILE_SEMAPHORE);                                  // destroying semaphore
            return -1;
        }
        sem_trywait(queueFileMutex);
        sem_getvalue(queueFileMutex,&value);
        printf("file semaphore value - %d\n",value);
        sem_post(queueFileMutex);
        char buf[100];

        printf("logger\n");
        strcpy(buf,"gnome-terminal -x sh -c 'exec /home/alexey/Lab_1/build-Logger-Desktop_Qt_5_8_0_GCC_64bit-Debug/Logger ");
        strcat(buf,std::to_string((int)getpid()).c_str());           // add pid to command argument
        strcat(buf,"'");
        system(buf);

        int loggerId = 0;
        loggerId = getInt();
        printf("getting - %d\n",loggerId);


        for(int i = 0; i < CLIENT_COUNT; i++)
        {
            cout << "Client process has been created" << endl;
            strcpy(buf,"gnome-terminal -x sh -c 'exec /home/alexey/Lab_1/build-Client-Desktop_Qt_5_8_0_GCC_64bit-Debug/Client ");
            strcat(buf,std::to_string((int)getpid()).c_str());           // add pid to command argument
            strcat(buf,"'");
            system(buf);                                                 // create client
            printf("%d\n",i);
            printf("Server - %d\n", (int)getpid());                      // server pid
        }

        //-------------------SERVER PROCESS------------------------------------

        printf("after create\n");
        Compiler *compiler = new Compiler(6);                            // 7 commands

        kill(loggerId,SIGUSR1);

        int clientClose = CLIENT_COUNT;
        while (clientClose > 0)                                          // while not all was closed
        {
            printf("WAITING\n");
            sigwait(&newmask,&signo);                                    // wait signal
            printf("get signal\n");
            clientClose--;
            if (signo == SIGUSR1)
            {
                printf("GET SIGNAL USER1\n");
                if(!compiler->connectToPipe(PIPE_NAME)){                 // try to connect to pipe
                    printf("compiler can't connect\n");
                    return -1;
                }
                char str[BUFFSIZE];
                switch( compiler->run(str) )                             // read and compare string
                {
                case 1:
                {
                    printf("YES\n");                                     // if progam is supported
                    break;
                }
                case -1:
                {
                    printf("PDF ERROR\n");                               // if PDF format
                    break;
                }
                case 0:
                {
                    printf("NO\n");                                      // if No supported
                    break;
                }
                }
                compiler->closeConnectionToPipe();                       // close connection
                sem_post(mutex);                                         // release semaphore
            }
            else
            {
                if (signo == SIGUSR2)
                {
                    printf("GET SIGNAL USER2\n");
                }
                else
                {
                    printf("GET OTHER SIGNAL\n");
                }
            }
        }

        sem_wait(mutex);
        sem_close(mutex);                                                // close semaphore
        sem_destroy(mutex);

        sem_wait(queueMutex);// destroy semaphore
        sem_close(queueMutex);
        sem_destroy(queueMutex);

        unlink(PIPE_NAME);                                               // close Pipe
        unlink(QUEUE_PIPE);
        return 0;
    }
#endif

