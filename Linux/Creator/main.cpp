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
#include <vector>

#define BUFFSIZE 256

#define QUEUE_FILE_SEMAPHORE "/FileSemaphore"
#define FILENAME "/home/alexey/Lab-4/thread.bin"

using namespace std;

void * thread_func(void *arg){                                       // the function for thread
    int i = *(int*)arg;
    while(1)                                                         // always sleep
    {
        sleep(5);                                                    // not working
    }
}

int main(int argc, char *argv[])
{
    FILE *file1 = fopen(FILENAME,"w+b");                             // creating file
    fclose(file1);

    int signo;                                                       // number of Signal
    sigset_t newmask;
    sigemptyset(&newmask);                                           // mask for signals
    sigaddset(&newmask,SIGUSR1);
    sigaddset(&newmask,SIGUSR2);
    sigaddset(&newmask,SIGTERM);
    sigprocmask(SIG_BLOCK, &newmask, NULL);
    bool flag = true;

    int value = 0;
    sem_t *queueFileMutex;
    queueFileMutex = sem_open(QUEUE_FILE_SEMAPHORE,O_CREAT,0644,0);  // creating semaphore for file (mutex)
    if(queueFileMutex == SEM_FAILED)
    {
        perror("unable to open semaphore");
        sem_unlink(QUEUE_FILE_SEMAPHORE);                            // destroying semaphore
        return -1;
    }
    sem_trywait(queueFileMutex);                                     // normalize semaphore
    sem_getvalue(queueFileMutex,&value);
    printf("file semaphore value - %d\n",value);                     // out semaphore
    sem_post(queueFileMutex);


    int timer, out;
    timer = atoi(argv[1]);                                           // timer value from cmd for creating
    out = atoi(argv[2]);                                             // timer value from cmd for outing
    char buf[BUFFSIZE];

    strcpy(buf,"gnome-terminal -x sh -c 'exec /home/alexey/Lab-4/build-Timer-Desktop_Qt_5_8_0_GCC_64bit-Debug/Timer "); // launch timer
    strcat(buf,std::to_string((int)getpid()).c_str());               // add pid to command argument
    strcat(buf, " ");
    strcat(buf, std::to_string(timer).c_str());                      // add timer value in cmd for timer
    strcat(buf,"'");
    system(buf);


    vector<pthread_t> threadList;                                    // lists for saving name of threads
    vector<int> intList;

    int id = 0;
    pthread_t threadid;                                              // id for thread
    pid_t pid;                                                       // pid for fork
    pid = fork();
    switch(pid)
    {
    case -1:
    {
        printf("fork error\n");                                      // fork exception
        return 0;
    }
    case 0:
    {
        queueFileMutex = sem_open(QUEUE_FILE_SEMAPHORE,0,0644,0);    // creating semaphore (mutex)
        if(queueFileMutex == SEM_FAILED)
        {
            perror("unable to open semaphore");
            sem_unlink(QUEUE_FILE_SEMAPHORE);                        // destroying semaphore
            return -1;
        }
        while(1)
        {
            sem_wait(queueFileMutex);                                // waiting for reading
            printf("reading\n");
            FILE *file = fopen(FILENAME,"rb");                       // open file to read
            if (file == NULL)
            {
                break;
            }
            while(!feof(file))
            {
                int i;
                if (!fread(&i,sizeof(int),1,file))                   // read int from file
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
            sem_post(queueFileMutex);
            sleep(out);                                              // wait out seconds
        }
        return 0;
    }
    default:
    {
        while(flag)
        {
            sigwait(&newmask,&signo);                                // wait the signal
            if (signo == SIGUSR1)
            {
                printf("\n");
                int result;
                result = pthread_create(&threadid, NULL, thread_func, &id); // creating new thread
                if (result != 0) {
                    perror("create first thread!");
                    return EXIT_FAILURE;
                }
                threadList.push_back(threadid);                      // adding thread id in the end of list
                id++;
                intList.push_back(id);                               // adding name in the end of list

                sem_wait(queueFileMutex);                            // waiting for writing in file
                printf("writing\n");

                FILE *file;
                file = fopen(FILENAME,"wb");                         // open file for writting
                for (int i = 0; i < intList.size(); i++)
                {
                    int temp = intList[i];
                    fwrite(&temp,sizeof(int),1,file);                // writing all list in file
                }
                rewind(file);
                fclose(file);
                sem_post(queueFileMutex);

            }
            if (signo == SIGUSR2)
            {
                printf("delete\n");
                if (id > 0)
                {
                    pthread_cancel(threadList[threadList.size()]);       // close thread
                    threadList.pop_back();                               // delete last element
                    id--;
                    intList.pop_back();
                    sem_wait(queueFileMutex);
                    printf("writing\n");

                    FILE *file;
                    file = fopen(FILENAME,"wb");                         // write in file
                    for (int i = 0; i < intList.size(); i++)
                    {
                        int temp = intList[i];
                        fwrite(&temp,sizeof(int),1,file);
                    }
                    rewind(file);
                    fclose(file);
                    sem_post(queueFileMutex);
                }
            }
            if (signo == SIGTERM)
            {
                for (int i = 0; i < threadList.size(); i++)
                {
                    pthread_cancel(threadList[i]);
                }
                kill(pid,SIGKILL);
                return 0;
            }
        }
    }
    }
}
