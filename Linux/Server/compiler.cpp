#include "compiler.h"
#define BUFF_SIZE 256
#define PIPE_NAME "./fifofile"

/*
 * Create new Compiler with
 * @param new_count - the count of supported extension:
 * 2 and more - ".c"
 * 3 and more - ".java"
 * 4 and more - ".py"
 * 5 and more - ".asm"
 * 6 and more - ".class"
 * 7 and more - ".js"
 * 8 and more - ".rb"
 * other      - ".cpp"
 */
Compiler::Compiler(int new_count)
{
    printf("Create compilator\n");
    this->count_of_commands = new_count;
    printf("Create commands\n");
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
    printf("creating succesfully\n");
}

/*
 * the destructor of Compiler
 */
Compiler::~Compiler()
{

}

/*
 * this compiler suport this extension ?:
 * @param name - name of file
 * @return: true - if this file is supported
 *         false - if this file isn't supported
 */
bool Compiler::getCommand(char* name)
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

/*
 * compare for error extension - ".pdf"
 */
bool Compiler::cmp(char* str)
{
    Command* error = new Command(".pdf");
        if (error->equals(str))
        {
            return true;
        }
    return false;
}

int Compiler::switchCommand(char* str)
{
    if (this->getCommand(str))                //result
    {
        return 1;
    }
    else
    {
        if (this->cmp(str))
        {
            return -1;
        }
        else
        {
            return 0;
        }
    }
}

int Compiler::run(char* str)
{
    printf("compiler run\n");
    for (int i = 0; i < BUFF_SIZE; i++)
    {
        str[i] = '\0';
    }
    printf("alive\n");
    fread(str,1,BUFF_SIZE,pipe);
    puts(str);
    return this->switchCommand(str);
}

bool Compiler::connectToPipe(char* pipePath)
{
    printf("start Connecting\n");
    this->pipe = fopen(pipePath, "r");
    if(this->pipe == NULL)
    {
        printf("Error! Compiler cannot connect to pipe!\n");
        return false;
    }
    printf("Compiler has been connected to pipe.\n");
    return true;
}

void Compiler::closeConnectionToPipe()
{
    fclose(this->pipe);
    unlink(PIPE_NAME);
}
