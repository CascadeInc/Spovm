#include "allocator.h"

Allocator::Allocator()
{

}

void* Allocator::myMalloc(size_t size)
{
    if (size <= 0)
    {
        return NULL;
    }
    size_t total = size + sizeof(size_t);
    void* memory;
    memory = mmap(NULL, total, PROT_WRITE | PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS,0,0);

    if (memory == (void*) -1)
    {
        return NULL;
    }

    *(size_t*)memory = size;
    return (void*) memory + sizeof(size_t);
}

void* Allocator::myRealloc(void *src, size_t size)
{
    size_t *sz;
    void *ret;
    if (!src)
        return Allocator::myMalloc(size);
    if (!size)
    {
        Allocator::myFree(src);
        return src;
    }
    sz = (size_t*)src - 1;
    if (*sz >= size)
    {
        ret=Allocator::myMalloc(size);
        memcpy (ret,src,size);
        Allocator::myFree(src);
        return ret;
    }
    ret = Allocator::myMalloc(size);
    memcpy(ret,src,*sz);
    Allocator::myFree(src);
    return ret;
}

void* Allocator::myCalloc(int size, char byte)
{
    void *mem;
    if (!size)
    {
        return NULL;
    }
    mem = Allocator::myMalloc(size);
    if (!mem)
    {
        return NULL;
    }
    memset(mem, byte, size);
    return mem;

}

void Allocator::myFree(void *mem)
{
    if (!mem)
    {
        return;
    }
    size_t *size;
    size = (size_t*)mem - 1;
    munmap(size,*size + (sizeof(size_t)));
}
