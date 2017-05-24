#include"allocator.h"
#include"stdio.h"
#include"stdlib.h"
int main(int argc, char *argv[])
{
    char* str;
    str = (char*)Allocator::myMalloc(6);
    gets(str);

    puts(str);

    char* src;
    src = (char*)Allocator::myCalloc(4,'1');
    src[3] = '\0';
    puts(src);

    str = (char*)Allocator::myRealloc(str,2);
    puts(str);

    Allocator::myFree(str);
    Allocator::myFree(src);
}
