#define CRT_SECURE_NO_WARNINGS
#include <QCoreApplication>
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
#include "programmer.h"


#define BUFFSIZE 256                                                 // bufsize
#define PIPE_NAME "/home/alexey/Lab_1/fifofile"
#define SEMAPHORE_NAME "/my_named_semaphore"
#define QUEUE_PIPE "/home/alexey/Lab_1/queuefile"
#define QUEUE_SEMAPHORE "/queue_semaphore"
#define QUEUE_FILE_SEMAPHORE "/queueFile_semaphore"
using namespace std;

int parentID = 0;

void queueHandler(int i)
{
    sem_t *queueFileMutex;
    queueFileMutex = sem_open(QUEUE_FILE_SEMAPHORE,0,0644,0);                 // creating semaphore (mutex)
    if(queueFileMutex == SEM_FAILED)
    {
        perror("unable to open semaphore");
        sem_unlink(QUEUE_FILE_SEMAPHORE);                                  // destroying semaphore
        return;
    }
    Programmer *programmer = new Programmer();
    sem_wait(queueFileMutex);
    programmer->logOut(QUEUE_PIPE);
    sem_post(queueFileMutex);
    kill((pid_t)parentID,SIGUSR2);
    exit(EXIT_SUCCESS);
}

void workHandler(int i)
{
    sem_t *mutex, *queueFileMutex;
    queueFileMutex = sem_open(QUEUE_FILE_SEMAPHORE,0,0644,0);                 // creating semaphore (mutex)
    if(queueFileMutex == SEM_FAILED)
    {
        perror("unable to open semaphore");
        sem_unlink(QUEUE_FILE_SEMAPHORE);                                  // destroying semaphore
        return;
    }

    mutex = sem_open(SEMAPHORE_NAME,0,0644,0);
    if(mutex == SEM_FAILED)
    {
        perror("reader:unable to execute semaphore");
        sem_close(mutex);
        return;
    }

    Programmer *programmer = new Programmer();
    sem_wait(queueFileMutex);
    programmer->logOut(QUEUE_PIPE);
    sem_post(queueFileMutex);
    kill((pid_t)parentID,SIGUSR2);
    sem_post(mutex);
    exit(EXIT_SUCCESS);
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // ----------------------CLIENT PROCESS------------------------------------
    parentID = atoi(argv[1]);                                        // get ppid from a command line
    printf("command - %d\nnatural parent - %d\n",parentID,(int)getppid());
    char str[BUFFSIZE];
    sem_t *mutex, *queueMutex, *queueFileMutex;                                                    // semaphore
    mutex = sem_open(SEMAPHORE_NAME,0,0644,0);
    if(mutex == SEM_FAILED)
    {
        perror("reader:unable to execute semaphore");
        sem_close(mutex);
        exit(-1);
    }

    queueMutex = sem_open(QUEUE_SEMAPHORE,0,0644,0);                 // creating semaphore (mutex)
    if(queueMutex == SEM_FAILED)
    {
        perror("unable to open semaphore");
        sem_unlink(QUEUE_SEMAPHORE);                                  // destroying semaphore
        return -1;
    }

    queueFileMutex = sem_open(QUEUE_FILE_SEMAPHORE,0,0644,0);                 // creating semaphore (mutex)
    if(queueFileMutex == SEM_FAILED)
    {
        perror("unable to open semaphore");
        sem_unlink(QUEUE_FILE_SEMAPHORE);                                  // destroying semaphore
        return -1;
    }

    Programmer *programmer = new Programmer();

    sem_wait(queueMutex);
    if (!programmer->connectToPipe(QUEUE_PIPE,"a"))
    {
        printf("cannot connect\n");
        sem_post(queueMutex);
        return -1;
    }
    programmer->logIn();
    programmer->closeConnectionToPipe();
    sem_post(queueMutex);

    printf("Wait...\n");    


    struct sigaction sa;
    sigset_t newset;
    sigemptyset(&newset);   // чистимся
    sigaddset(&newset, SIGHUP);  // добавляем сигнал SIGHUP
    sigprocmask(SIG_BLOCK, &newset, 0);   // блокируем его
    sa.sa_handler = queueHandler;  // указываем обработчик
    sigaction(SIGCONT, &sa, 0);    // обрабатываем сигнал SIGTERM

    sem_wait(mutex);                       // waiting semaphore

    sa.sa_handler = workHandler;  // указываем обработчик
    sigaction(SIGCONT, &sa, 0);    // обрабатываем сигнал SIGTERM

    printf("Input Program:\n");
    gets(str);
    printf("\ngood job\n");
    sleep(1);
    if(!programmer->connectToPipe(PIPE_NAME,"w+"))                        // connect to pipe
    {
        printf("cannot connect\n");
        sem_post(mutex);
        sleep(5);
        return -1;
    }
    printf("running compile\n");
    sleep(1);
    programmer->runCompile(str);                                     // write string in pipe
    programmer->closeConnectionToPipe();                             // close pipe

    sem_wait(queueFileMutex);
    programmer->logOut(QUEUE_PIPE);
    sem_post(queueFileMutex);
    printf("kill\n");
    kill((pid_t)parentID,SIGUSR1);                                   // send to parent process SIGUSR1
    sem_close(mutex);                                                // close semaphore
    sem_close(queueMutex);
    sem_close(queueFileMutex);
    sleep(1);                                                        // for reading messages
}

