#include "syscall.h"
#include "copyright.h"
#define MAXSIZE 255

int main()
{
    int stdin;
    int stdout;
    char buffer[MAXSIZE];
    int len;

    PrintConsole("\n\n\t\t <<<< ECHO >>>> \n\n");

    PrintConsole("Enter string line: ");
    stdin = Open("stdin", 2);   // Invoke Open func to get stdin
    stdout = Open("stdout", 3); // Invoke Open func to get stdout

    // exception : full openTable
    if (stdin == -1 || stdout == -1)
    {
        return -1;
    }

    len = Read(buffer, MAXSIZE, stdin); // read function

    // exception : read file
    if (len < 0)
    {
        return -1;
    }

    // print console
    PrintConsole(">STDOUT: ");
    Write(buffer, len, stdout);
    PrintConsole("\n\n");

    Close(stdin);
    Close(stdout);

    return 0;
}
