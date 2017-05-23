#include "header.h"
#define SIZE 512


char *files[5] = { "/home/alexey/lab-5/Files/1", "/home/alexey/lab-5/Files/2", "/home/alexey/lab-5/Files/3", "/home/alexey/lab-5/Files/4", "/home/alexey/lab-5/Files/5"};

void *threadReader(void *arg)
{
  struct Info *info = (struct Info *)arg;
  printf("loading read function\n");
  void (*lib_function)(int, char *);
  *((void **)(&lib_function)) = dlsym(info->library, "readFromFile");
  for(int i = 0; i < 5; i++)
  {
    printf("wait info flag\n");
    while(info->flag);
    printf("mutex locking\n");
    pthread_mutex_lock(&info->mutex);
    printf("File name: %s\n", info->fileNames[i]);
    int fd = open(info->fileNames[i], O_RDONLY);
    if(fd < 0)
    {
      printf("File not found");
      exit(-1);
    }
    printf("execute lib function for read\n");
    (*lib_function)(fd, info->buffer);
    printf("close file\n");
    close(fd);
    printf("unlocking mutex\n");
    pthread_mutex_unlock(&info->mutex);
    info->size++;
    info->flag = 1;
    usleep(1);
  }
  info->threadsNumber--;
  return NULL;
}

void *threadWriter(void *arg)
{
  struct Info *info = (struct Info *)arg;
  printf("open library for writing\n");
  void (*lib_function)(int, char *);
  *((void **)(&lib_function)) = dlsym(info->library, "writeToFile");
  printf("locking mutex for writing\n");
  pthread_mutex_lock(&info->mutex);
  int fd = open("/home/alexey/lab-5/Files/All", O_WRONLY | O_CREAT | O_TRUNC | O_APPEND);
  printf("unlocking mutex for writing\n");
  pthread_mutex_unlock(&info->mutex);
  usleep(1);
  while(!info->flag);
  for(int i = 0; i < 5; i++)
  {
    printf("wait infor flag for writing\n");
    while(!info->flag);
    printf("locking mutex for writing in i file\n");
    pthread_mutex_lock(&info->mutex);
    printf("Write message%s\n", info->buffer);
    printf("execute lib function for writing\n");
    (*lib_function)(fd, info->buffer);
    printf("unlocking mutex!\n");
    pthread_mutex_unlock(&info->mutex);
    info->flag = 0;
    usleep(1);
  }
  close(fd);
  info->threadsNumber--;
  return NULL;
}

void initInfo(struct Info *info)
{
  info->fileNames = files;
  info->size = 0;
  printf("open library\n");
  info->library = dlopen("/home/alexey/Labs-4-sem-master/SPOVM/Laba_5/lib_lin.so", RTLD_LAZY);
  info->threadsNumber = 0;
  info->flag = 0;
}

void createMutex(struct Info *info)
{
  printf("initialize mutex\n");
  pthread_mutex_init(&info->mutex, NULL);
}

void runThreads(struct Info *info)
{
  printf("creating read\n");
  pthread_create(&info->readThread, NULL, &threadReader, info);
  info->threadsNumber++;
  printf("creating write\n");
  pthread_create(&info->writeThread, NULL, &threadWriter, info);
  info->threadsNumber++;
}

void waitThreads(struct Info *info)
{
  while(info->threadsNumber);
  printf("All good!\n");
  dlclose(info->library);
}
