#include "compiler.h"

Compiler::Compiler(int new_count)
{
    this->count_of_commands = new_count;
    this->commands = new Command*[new_count];
    for (int i = 0; i < new_count; i++)
    {
        switch(i)
        {
        case 0:
        {
            commands[i] = new Command(".cpp");
            break;
        }
        case 1:
        {
            commands[i] = new Command(".c");
            break;
        }
        case 2:
        {
            commands[i] = new Command(".java");
            break;
        }
        case 3:
        {
            commands[i] = new Command(".py");
            break;
        }
        case 4:
        {
            commands[i] = new Command(".asm");
            break;
        }
        case 5:
        {
            commands[i] = new Command(".class");
            break;
        }
        case 6:
        {
            commands[i] = new Command(".js");
            break;
        }
        case 7:
        {
            commands[i] = new Command(".rb");
            break;
        }
        default:
        {
            commands[i] = new Command(".cpp");
            break;
        }
        }
    }
}
bool Compiler::getCommand(std::string name)
{
    for (int i = 0; i < this->count_of_commands; i++)
    {
        if (commands[i]->equals(name))
        {
            return true;
        }
    }
    return false;
}
bool Compiler::isContain(std::string name)
{
   for (int i = 0; i < this->count_of_commands; i++)
   {
        if (commands[i]->equals(name))
        {
           return true;
        }
   }
   return false;
}
Compiler::~Compiler()
{
}
void Compiler::write()
{
    FILE *file;
    file = fopen("compiler.com","wb+");
    fwrite(&this->count_of_commands,sizeof(int),1,file);
    for (int i = 0; i < this->count_of_commands; i++)
    {
        this->commands[i]->write(file);
        fwrite("\n",1,1,file);
    }
    fclose(file);
}
Compiler* Compiler::read()
{
    FILE *file;
    file = fopen("compiler.com.res","rb+");
    int count;
    fread(&count,sizeof(int),1,file);
    Compiler *res = new Compiler(count);
    for (int i = 0; i < count; i++)
    {
        res->commands[i]->read(file);
        fseek(file,1,1);
    }
    fclose(file);
    return res;
}
bool Compiler::cmp(std::string str)
{
	Command* error = new Command(".pdf");
	return (error->equals(str));
}
