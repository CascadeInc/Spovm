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
#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <termios.h>


#define BUFFSIZE 256

#define SEMAPHORE_NAME "/my_named_semaphore"
#define QUEUE_SEMAPHORE "/queue_semaphore"
#define QUEUE_PIPE "/home/alexey/Lab_4/queuePipe"
#define QUEUE_FILE_SEMAPHORE "/FileSemaphore"

#define FILENAME "/home/alexey/Lab-4/thread.bin"

using namespace std;

int main(int argc, char *argv[])
{
    int id = 1;
    bool flag = true;
    pid_t creatorid = atoi(argv[1]); //
    int timer = atoi(argv[2]);
    pid_t pid;
    pid = fork();
    switch (pid)
    {
    case -1:
    {
        return -1;
    }
    case 0:
    {
        while (flag)
        {
            kill(creatorid,SIGUSR1);
            sleep(timer);
        }
    }
    default:
    {
        int str = 0;
        while (flag)
        {

            struct termios oldt,newt;
            int ch;
            tcgetattr( STDIN_FILENO, &oldt );
            newt = oldt;
            newt.c_lflag &= ~( ICANON | ECHO );
            tcsetattr( STDIN_FILENO, TCSANOW, &newt );
            ch = getchar();
            tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
            printf("input command:\n");
            rewind(stdin);
            if (ch == '+')
            {
                printf("kill USR1\n");
                kill(creatorid,SIGUSR1);
            }
            if (ch == '-')
            {
                printf("kill USR2\n");
                kill(creatorid,SIGUSR2);
            }
            if (ch != '+' && ch != '-')
            {
                printf("kill TERM\n");
                kill(creatorid,SIGTERM);
                flag = false;
                return 0;
            }
        }
    }
    }
}
