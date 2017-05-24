#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>

class Allocator
{
public:
    Allocator();
public:
    static void* myMalloc(size_t size);
    static void* myRealloc(void* src, size_t newSize);
    static void* myCalloc(int size, char byte);
    static void myFree(void* mem);
};

#endif // ALLOCATOR_H
