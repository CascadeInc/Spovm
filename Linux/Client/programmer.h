#ifndef CLIENT_H
#define CLIENT_H


#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fstream>

using namespace std;

#define FIFO_NAME "./fifofile"

class Programmer
{
private:

    FILE* pipe;

public:

    Programmer();

    ~Programmer();

    bool connectToPipe(char* pipePath, char* mode);

    void runCompile(char* program);

    void closeConnectionToPipe();

    void logIn();

    void logOut(char* str);
};

#endif // CLIENT_H
