#ifndef RESTORAN_H
#define RESTORAN_H

#include "command.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

class Compiler
{
private:
    int count_of_commands;
    Command **commands;
    FILE* pipe;
public:
    bool connectToPipe(char* pipePath);
    char* getStringFromPipe();
    void closeConnectionToPipe();
    int switchCommand(char* str);
    static bool cmp(char* str);
    Compiler(int);
    ~Compiler();
    bool getCommand(char*);
    int run(char* str);
};

#endif // COMPILER_H
