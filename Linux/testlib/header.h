#define BUFFER_SIZE 512
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <termios.h>
#include <pthread.h>
#include <dirent.h>
#include <string.h>
#include <aio.h>
#include <dlfcn.h>

struct Info
{
  int threadsNumber;
  pthread_t readThread;
  pthread_t writeThread;
  pthread_mutex_t mutex;
  void *library;
  char **fileNames;
  int size;
  char buffer[512];
  volatile int flag;
};

void *threadReader(void *);
void *threadWriter(void *);

void initInfo(struct Info *);
void createMutex(struct Info *);
void runThreads(struct Info *);
void waitThreads(struct Info *);
