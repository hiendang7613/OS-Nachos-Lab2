#include "syscall.h"
#define MAXSIZE 32

int main()
{
    int id; 
    int size; 
    char c[2];                  // temp char
    char filename[MAXSIZE];     // allocate filename charArr by MAXSIZE
    int pos;

    PrintConsole("\n\n\t\t <<<< CAT >>>> \n\n");

    // Get filename from console
    PrintConsole(">Input filename: ");
    ReadConsole(filename, MAXSIZE);

    id = Open(filename, 1); // fileID

    // exception : full openTable
    if (id == -1)
    {
        PrintConsole(">Exception!!: Full openTable \n");
        return -1;
    }

    size = Seek(-1, id); // size of file
    Seek(0, id);             // seek cursor to first character of file
    c[1] = '\0';                 

    // Print file Content
    PrintConsole(">Content of file: \n");
    for (pos = 0; pos < size; pos++)
    {
        Read(&c, 1, id);    // Read one character
        PrintConsole(c);    // Write one character
    }
    PrintConsole("\n\n");

    Close(id); // Close file

    return 0;
}
