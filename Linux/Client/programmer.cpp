#include "programmer.h"
#define TEMP_PIPE "/home/alexey/Lab_1/queuefile_temp"
#define BUFFSIZE 256
Programmer::Programmer()
{
    printf("create programmer\n");
}
Programmer::~Programmer()
{

}

bool Programmer::connectToPipe(char* pipeName,char* mode)
{
    printf("try connect to pipe\n");
    if (mode != NULL)
    {
        this->pipe = fopen(pipeName,mode);
    }
    printf("connecting to pipe\n");
    if(this->pipe == NULL)
    {
        printf("Error!  cannot connect to pipe!\n");
        return false;
    }
    printf("Client is connected to pipe.\n");
    sleep(1);
    return true;
}

void Programmer::runCompile(char* program)
{
     printf("start sending...\n");
     fputs(program, this->pipe);
     fputs("\0",this->pipe);
     printf("is Send successfully\n");
}

void Programmer::closeConnectionToPipe()
{
    fclose(this->pipe);
    printf("close\n");
}

void Programmer::logIn()
{
    printf("logIn - %d\n",(int)getpid());
    fputs(std::to_string((int)getpid()).c_str(),this->pipe);
    fputs("\n",this->pipe);
    printf("logIn succesfully\n");
}

void Programmer::logOut(char* pipeName)
{
    if (!this->connectToPipe(pipeName,"r"))
    {
        printf("can't connection to temp Pipe \n");
        sleep(1);
        return;
    }
    printf("logOut\n");
    FILE *tempFile = fopen(TEMP_PIPE, "w+");
    if (tempFile == NULL)
    {
        printf("can't create file\n");
        this->closeConnectionToPipe();
        sleep(1);
        return;
    }
    int idBuffer;
    char tempBuffer[BUFFSIZE];
    rewind(this->pipe);
    fseek(this->pipe,0,0);
    do
    {
        fgets(tempBuffer,BUFFSIZE,this->pipe);
        if (feof(this->pipe))
        {
            break;
        }
        //fseek(this->pipe,1,1);
        if (tempBuffer == NULL)
        {
            printf("pidec\n");
        }
        idBuffer = atoi(tempBuffer);
        printf("read from pipe - %ld\n",ftell(this->pipe));
        puts(tempBuffer);
        if (idBuffer != getpid())
        {
            fputs(std::to_string(idBuffer).c_str(),tempFile);
            fputs("\n",tempFile);
        }
    }while(1);

    printf("pos in file - %ld",ftell(this->pipe));
    rewind(this->pipe);
    fclose(tempFile);
    fclose(this->pipe);
    remove(pipeName);
    rename(TEMP_PIPE,pipeName);
    printf("remove and rename\n");
    sleep(1);
}
