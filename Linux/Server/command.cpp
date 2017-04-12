#include "command.h"

Command::Command(char* new_name)
{
    this->name = new char[strlen(new_name) + 1];
    strcpy(this->name,new_name);
}
bool Command::equals(char* new_name)
{
    int j = strlen(this->name);
    if (j > strlen(new_name)){
        return false;
    }
    else
    {
        for (int i = strlen(new_name);i >= strlen(new_name) - strlen(this->name);i--)
        {
            if (this->name[j--] != new_name[i])
                return false;
        }
    return true;
    }
}
Command::~Command()
{
    delete this->name;
}
