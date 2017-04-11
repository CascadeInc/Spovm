#include "command.h"
#ifndef RESTORAN_H
#define RESTORAN_H


class Compiler
{
private:
    int count_of_commands;
    Command **commands;
public:
    Compiler(int);
    ~Compiler();
    bool isContain(std::string);
    bool getCommand(std::string);
    void write();
    static Compiler* read();
	static bool cmp(std::string);
};

#endif // COMPILER_H
