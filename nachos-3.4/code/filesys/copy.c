#include "syscall.h"
#define MAXSIZE 32

int main()
{
    int idSrc;
    int idDes;
    int size;
    char srcFilename[MAXSIZE];     
    char desFilename[MAXSIZE];     
    char c;                  // temp char
    int pos;

    PrintConsole("\n\n\t\t <<<< COPY >>>> \n\n");

    // Get old and new filename from console
    PrintConsole(">Input filename: ");
    ReadConsole(srcFilename, MAXSIZE);
    PrintConsole(">Input filename: ");
    ReadConsole(desFilename, MAXSIZE);

    idSrc = Open(srcFilename, 1);   // open source file
    idDes = Create(desFilename);    // create destination file
    Close(idDes);                       
    idDes = Open(desFilename, 1);   // open destination file by read&write mode


    // exception : full openTable
    if (idSrc == -1 || idDes == -1 )
    {
        PrintConsole(">Exception!!: Full openTable !!\n");
        return -1;
    }

    size = Seek(-1, idSrc); // size of file
    Seek(0, idSrc);             // seek cursor to first character of source file
    Seek(0, idDes);             // seek cursor to first character of destinationdest file

    // Print file Content
    PrintConsole(">Content of file: \n");
    for (pos = 0; pos < size; pos++)
    {
        // Copy each character
        Read(&c, 1, idSrc);
        Write(&c, 1, idDes);
    }
    PrintConsole(">Copied successfully !!\n\n"); // Thong bao thanh cong

    Close(idSrc); // Close source file
    Close(idDes); // Close destinationdest file

    return 0;
}
