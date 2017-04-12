#include <string.h>
#include <stdio.h>
#ifndef MEAL_H
#define MEAL_H


class Command
{
private:
    char* name;
public:
    Command(char*);
    ~Command();
    bool equals(char*);
};

#endif // MEAL_H
