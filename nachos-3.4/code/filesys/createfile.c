#include "syscall.h"
//#include "copyright.h"
#define maxlen 32

int main()
{
    int stdin;	
    int len;
    char filename[maxlen + 1];

    PrintConsole("\n\n\t\t <<<< Create file >>>> \n\n");

    PrintConsole(">Enter file name from stdin: ");

    stdin = Open("stdin", 2); // Invoke Open func to get stdin

    // exception : full openTable
    if (stdin == -1)
    {
        PrintConsole(">Exception!!: Full openTable \n");
        return -1;
    }

    len = Read(filename, maxlen, stdin); // read function
    
    // exception : read file
    if (len < 0)
    {
        return -1;
    }

    // Test CreateFile
    if (Create(filename) == 0)
    {
        // success !!
        PrintConsole(">Create file: ");
        PrintConsole(filename);
        PrintConsole(" success !!\n\n");
    }
    else
    {
        // fail !!
        PrintConsole(">Create file: ");
        PrintConsole(filename);
        PrintConsole(" fail !!\n\n");
    }

    Close(stdin); // close stdin
    
    return 0;
}
