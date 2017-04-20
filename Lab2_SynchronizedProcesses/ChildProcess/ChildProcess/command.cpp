#include "command.h"

Command::Command(std::string new_name)
{
    this->name = new_name;
}
bool Command::equals(std::string new_name)
{
    int j = this->name.length();
    if (j > new_name.length()){
        return false;
    }
    else
    {
        for (int i = new_name.length();i >= new_name.length() - this->name.length();i--)
        {
            if (this->name[j--] != new_name[i])
                return false;
        }
    return true;
    }
}
Command::~Command()
{
}
Command* Command::read(FILE *file)
{
    char c;
    int len = 0;
    do
    {
      fread(&c,1,1,file);
      len++;
    } while(c!='\t');
    char* str = new char[len];
    fseek(file,-len,1);
    fread(str,1,len,file);
    fseek(file,1,1);
    return new Command(str);
}
void Command::write(FILE *file)
{
    //fwrite(this->name,1,strlen(this->name),file);
    //fprintf(file,"\t");
}
