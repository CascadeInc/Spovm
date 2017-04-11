#include <string.h>
#include <stdio.h>
#include <string.h>
#include <string>

#ifndef MEAL_H
#define MEAL_H


class Command
{
private:
	std::string name;
public:
    Command(std::string);
    ~Command();
    bool equals(std::string);
    static Command* read(FILE*);
    void write(FILE*);
};

#endif // MEAL_H
