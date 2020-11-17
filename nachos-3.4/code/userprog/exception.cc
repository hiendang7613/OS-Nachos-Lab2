// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"

#define MaxFileLength 32 // Do dai quy uoc cho file name
//ProcTable *procTable;

//3. Tang program counter truoc khi system call tra ket qua ve
//Doi thanh ghi program counter cua he thong ve sau 4byte de tiep tuc nap lenh
void IncreasePC()
{
    int counter = machine->ReadRegister(PCReg);     //lay gt t_ghi h_tai
    machine->WriteRegister(PrevPCReg, counter);     //gan gt t_ghi trc = h_tai
    counter = machine->ReadRegister(NextPCReg);     //lay gt t_ghi k_tiep
    machine->WriteRegister(PCReg, counter);         //gan gt t_ghi h_tai = t_ghi k_tiep
    machine->WriteRegister(NextPCReg, counter + 4); //gan t_ghi k_tiep ve sau 4 byte de tiep tuc nap lenh
}

// Input: - User space address (int)
// - Limit of buffer (int)
// Output:- Buffer (char*)
// Purpose: Copy buffer from User memory space to System memory space
char *User2System(int virtAddr, int limit)
{
    int i; // index
    int oneChar;
    char *kernelBuf = NULL;
    kernelBuf = new char[limit + 1]; //need for terminal string

    if (kernelBuf == NULL)
        return kernelBuf;
    memset(kernelBuf, 0, limit + 1);
    //printf("\n Filename u2s:");
    for (i = 0; i < limit; i++)
    {
        machine->ReadMem(virtAddr + i, 1, &oneChar);
        kernelBuf[i] = (char)oneChar;
        //printf("%c",kernelBuf[i]);
        if (oneChar == 0)
            break;
    }
    return kernelBuf;
}

// Input: - User space address (int)
// - Limit of buffer (int)
// - Buffer (char[])
// Output:- Number of bytes copied (int)
// Purpose: Copy buffer from System memory space to User memory space
int System2User(int virtAddr, int len, char *buffer)
{
    if (len < 0)
        return -1;
    if (len == 0)
        return len;
    int i = 0;
    int oneChar = 0;
    do
    {
        oneChar = (int)buffer[i];
        machine->WriteMem(virtAddr + i, 1, oneChar);
        i++;
    } while (i < len && oneChar != 0);
    return i;
}

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions
//	are in machine.h.
//----------------------------------------------------------------------

// Ham xu ly ngoai le running time cua systemcall
void ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

    // Input: reg4 -filename (string)
    // Output: reg2 -1: error and 0: success
    // Purpose: process the event SC_Create of System call
    // mã system call sẽ được đưa vào thanh ghi r2 (có thể xem lại phần xử lý cho
    // system call Halt trong tập tin start.s ở trên)
    // tham số thứ 1 sẽ được đưa vào thanh ghi r4
    // tham số thứ 2 sẽ được đưa vào thanh ghi r5
    // tham số thứ 3 sẽ được đưa vào thanh ghi r6
    // tham số thứ 4 sẽ được đưa vào thanh ghi r7
    // kết quả thực hiện của system call sẽ được đưa vào thanh ghi r2
    switch (which)
    {
    case NoException:
        return;
    case PageFaultException:
        DEBUG('a', "\n No valid translation found");
        printf("\n\n No valid translation found");
        interrupt->Halt();
        break;
    case ReadOnlyException:
        DEBUG('a', "\n Write attempted to page marked read-only");
        printf("\n\n Write attempted to page marked read-only");
        interrupt->Halt();
        break;
    case BusErrorException:
        DEBUG('a', "\n Write attempted to page marked read-only");
        printf("\n\n Write attempted to page marked read-only");
        interrupt->Halt();
        break;
    case AddressErrorException:
        DEBUG('a', "\n Unaligned reference or one that was beyond the end of the address space");
        printf("\n\n Unaligned reference or one that was beyond the end of the address space");
        interrupt->Halt();
        break;
    case OverflowException:
        DEBUG('a', "\nInteger overflow in add or sub.");
        printf("\n\n Integer overflow in add or sub.");
        interrupt->Halt();
        break;
    case IllegalInstrException:
        DEBUG('a', "\n Unimplemented or reserved instr.");
        printf("\n\n Unimplemented or reserved instr.");
        interrupt->Halt();
        break;
    case NumExceptionTypes:
        DEBUG('a', "\n Number exception types");
        printf("\n\n Number exception types");
        interrupt->Halt();
        break;
    case SyscallException:
    {
        switch (type)
        {
        case SC_Halt:
        {
            DEBUG('a', "\n Shutdown, initiated by user program.");
            printf("\n\n Shutdown, initiated by user program.");
            interrupt->Halt();
            break;
        }
        case SC_Create:
        {
            int virtAddr;
            char *filename;
            DEBUG('a', "\n SC_Create call ...");
            DEBUG('a', "\n Reading virtual address of filename");
            // Lấy tham số tên tập tin từ thanh ghi r4
            virtAddr = machine->ReadRegister(4);
            DEBUG('a', "\n Reading filename.");
            // MaxFileLength là = 32
            filename = User2System(virtAddr, MaxFileLength + 1);
            if (filename == NULL)
            {
                printf("\n Not enough memory in system");
                DEBUG('a', "\n Not enough memory in system");
                machine->WriteRegister(2, -1); // trả về lỗi cho chương trình người dùng
                delete filename;
                return;
            }
            DEBUG('a', "\n Finish reading filename.");
            //DEBUG('a',"\n File name : '"<<filename<<"'");
            // Create file with size = 0
            // Dùng đối tượng fileSystem của lớp OpenFile để tạo file,
            // việc tạo file này là sử dụng các thủ tục tạo file của hệ điều
            // hành Linux, chúng ta không quản ly trực tiếp các block trên
            // đĩa cứng cấp phát cho file, việc quản ly các block của file
            // trên ổ đĩa là một đồ án khác
            if (!fileSystem->Create(filename, 0))
            {
                printf("\n Error create file '%s'", filename);
                machine->WriteRegister(2, -1);
                delete filename;
                return;
            }
            machine->WriteRegister(2, 0); // trả về cho chương trình
                                          // người dùng thành công
            delete[] filename;
            break;
        }
        case SC_Open:
        {
            // Function :  OpenFileId Open(char *name, int type)
            // Input :
            //      name - file path
            //      type - 0: read and write
            //             1: read only
            //             2: stdin
            //             3: stdout
            // Output : OpenFileId ( if fail return -1)

            int virtualAddr = machine->ReadRegister(4); // read name Address from 4th register
            int type = machine->ReadRegister(5);        // read type from 5th register

            char *filename = User2System(virtualAddr, MaxFileLength); // Copy filename charArray form userSpace to systemSpace

            int freeSlot = fileSystem->FindFreeSlot();
            if (freeSlot == -1)
            {
                machine->WriteRegister(2, -1);
                printf(">Exception!!: Full openTable !! \n");
                return;
            }

            switch (type)
            {
            case 0: //  Read and write
            case 1: //  Read only
                if ((fileSystem->openTable[freeSlot] = fileSystem->Open(filename, type)) != NULL)
                {
                    machine->WriteRegister(2, freeSlot);
                }
                else
                {
                    printf("\n>Exception!!: File does not exist !! \n");
                    machine->WriteRegister(2, -1);
                    return;
                }
                break;
            case 2: //  Stdin - read from console
                machine->WriteRegister(2, 0);
                break;
            case 3: //  Stdout - write to console
                machine->WriteRegister(2, 1);
                break;
            default:
                printf("\n>Exception!!: Type parameter of SC_Open syscall is not match !! \n");
                return;
            }
            delete[] filename;
            break;
        }
        case SC_Close:
        {
            // Function :  void Close(OpenFileId id)
            // Input :  id cua file (OpenFileId)
            // Output : success return 0 ( if fail return -1)

            int fid = machine->ReadRegister(4); // read file ID from 4th register

            if (fid < 0 || fid > 10)
            {
                printf("\n>Exception!!: ID parameter of SC_Close syscall not match !! \n");
                machine->WriteRegister(2, -1);
                return;
            }

            if (fileSystem->openTable[fid] == NULL)
            {
                printf("\n>Exception!!: File was not opened !! \n");
                machine->WriteRegister(2, -1);
                return;
            }

            delete fileSystem->openTable[fid];
            fileSystem->openTable[fid] = NULL;
            machine->WriteRegister(2, 0);
            break;
        }
        case SC_Read:
        {
            // Function :  int Read(char *buffer, int charcount, OpenFileId id)
            // Input :
            //      buffer    - read data to buffer
            //      charcount - counter of charater to read
            //      id        - file ID (OpenFileId)
            // Output :  -1: error
            //           -2: EOF
            //            n: number of character

            int virtAddr = machine->ReadRegister(4);  // read buffer Address from 4th register
            int charcount = machine->ReadRegister(5); // read counter of chars from 5th register
            int id = machine->ReadRegister(6);        // read file id from 6th register

            // reuturn exception
            if (id < 0 || id > 10) // wrong ID
            {
                printf("\n>Exception!!: ID parameter of SC_Read syscall not match !! \n");
                machine->WriteRegister(2, -1);
                return;
            }
            else if (fileSystem->openTable[id] == NULL) // ID was not found in fileTable
            {
                printf("\n>Exception!!: File was not opened !! \n");
                machine->WriteRegister(2, -1);
                return;
            }
            else if (fileSystem->openTable[id]->type == 3) // type : stdout
            {
                printf("\n>Exception!!: Can not read from stdout !! \n");
                machine->WriteRegister(2, -1);
                return;
            }

            char *buffer = new char[charcount]; // Init buffer

            // Read data from stdin
            if (fileSystem->openTable[id]->type == 2)
            {
                int count = gSynchConsole->Read(buffer, charcount); // Transfer data to buffer from console
                System2User(virtAddr, count, buffer);               // Copy data Buffer (SystemSpace) to UserSpace
                machine->WriteRegister(2, count);                   // return
                delete buffer;
                break;
            }

            // Read data from file
            if (fileSystem->openTable[id]->type == 0 || fileSystem->openTable[id]->type == 1)
            {
                int count = fileSystem->openTable[id]->Read(buffer, charcount);
                if (count > 0)
                {
                    System2User(virtAddr, count, buffer); // Copy data Buffer (SystemSpace) to UserSpace
                    machine->WriteRegister(2, count);
                    delete buffer;
                    break;
                }
                else
                {
                    printf("\n>Exception!!: EOF !! \n");
                    machine->WriteRegister(2, -2); // EOF
                    delete buffer;
                    return;
                }
            }

            printf("\n>Exception!!: Type parameter of SC_Open syscall is not match !! \n");
            return;
        }
        case SC_Write:
        {
            // Function :   int Write(char* buffer, int charcount, OpenFileId id)
            // Input :
            //      buffer    - write data from buffer
            //      charcount - counter of charater to write
            //      id        - file ID (OpenFileId)
            // Output :  -1: error
            //           -2: EOF
            //            n: number of character ( if fail return -1)

            int virtualAddr = machine->ReadRegister(4); // read buffer Address from 4th register
            int charcount = machine->ReadRegister(5);   // read counter of char from 5th register
            int id = machine->ReadRegister(6);          // read file id from 6th register

            // reuturn exception
            if (id < 0 || id > 10) // wrong ID
            {
                printf("\n>Exception!!: ID parameter of SC_Read syscall not match !! \n");
                machine->WriteRegister(2, -1);
                return;
            }
            else if (fileSystem->openTable[id] == NULL) // ID was not found in fileTable
            {
                printf("\n>Exception!!: File was not opened !! \n");
                machine->WriteRegister(2, -1);
                return;
            }
            else if (fileSystem->openTable[id]->type == 1) // type : read-only
            {
                printf("\n>Exception!!: Can not write to readonly file !! \n");
                machine->WriteRegister(2, -1);
                return;
            }
            else if (fileSystem->openTable[id]->type == 2) // type : stdin
            {
                printf("\n>Exception!!: Can not write to stdin !! \n");
                machine->WriteRegister(2, -1);
                return;
            }

            char *buffer = User2System(virtualAddr, charcount); // Copy data Buffer (UserSpace) to SystemSpace

            // Write buffer to stdout
            if (fileSystem->openTable[id]->type == 3)
            {
                int count = 0;
                while (buffer[count] != '\0' && count < charcount) // loop not '\0' and '\n'
                {
                    count++;
                }
                count = gSynchConsole->Write(buffer, count); // write a character
                buffer[count] = '\n';
                gSynchConsole->Write(buffer + count, 1); // write '\n'
                machine->WriteRegister(2, count - 1);    // return n character
                delete buffer;
                break;
            }

            // Write buffer to file
            if (fileSystem->openTable[id]->type == 0)
            {
                int count = fileSystem->openTable[id]->Write(buffer, charcount); // count of writer

                if (count < 0)
                {
                    printf("\n>Exception!!: Do not read any characters !! \n");
                    machine->WriteRegister(2, -1);
                    delete buffer;
                    return;
                }

                machine->WriteRegister(2, count);
                delete buffer;
                break;
            }

            printf("\n>Exception!!: Type parameter of SC_Open syscall is not match !! \n");
            return;
        }
        case SC_Seek:
        {
            // Function :   int Seek(int pos, OpenFileId id)
            // Input :
            //      pos       - position of cursor
            //      id        - file ID (OpenFileId)
            // Output :  -1: error
            //            n: real position of cursor (success)

            int pos = machine->ReadRegister(4); // Get positive of cursor from 4th register
            int id = machine->ReadRegister(5);  // Get file id from 5th register

            // reuturn exception
            if (id < 0 || id > 10) // wrong ID
            {
                printf("\n>Exception!!: ID parameter of SC_Read syscall not match !! \n");
                machine->WriteRegister(2, -1); // Tra ve -1 that bai
                return;
            }
            else if (fileSystem->openTable[id] == NULL) // ID was not found in fileTable
            {
                printf("\n>Exception!!: File was not opened !! \n");
                machine->WriteRegister(2, -1);
                return;
            }
            else if (fileSystem->openTable[id]->type == 3 || fileSystem->openTable[id]->type == 2) // type : stdout || stdin
            {
                printf("\n>Exception!!: Can not seek in console !! \n");
                machine->WriteRegister(2, -1);
                return;
            }

            // If pos = -1, seek cursor to end of file
            pos = (pos == -1) ? fileSystem->openTable[id]->Length() : pos;
            if (pos > fileSystem->openTable[id]->Length() || pos < 0) // wrong positive
            {
                printf("\n>Exception!!: Pos parameter of SC_Seek syscall not match !! \n");
                machine->WriteRegister(2, -1);
                return;
            }

            // Seek cursor
            fileSystem->openTable[id]->Seek(pos); // Seek cursor to position
            machine->WriteRegister(2, pos);
            break;
        }
        case SC_ReadConsole:
        {
            // Function :   int ReadConsole(char buffer[], int length)
            // Input :
            //      buffer      - read data to buffer
            //      length      - length of string
            // Output :  number of bytes read

            int virtAddr = machine->ReadRegister(4); // read buffer address from 4th register
            int length = machine->ReadRegister(5);   // read lenght of string from 5th register

            char *buffer = new char[length + 1];
            length = gSynchConsole->Read(buffer, length); // read string from console
            System2User(virtAddr, length, buffer);        // Copy string from systemSpace to userSpace
            machine->WriteRegister(2, length - 1);        // return n character
            delete buffer;
            break;
        }
        case SC_PrintConsole:
        {
            // Function :   int PrintConsole(char buffer[])
            // Input :
            //      buffer      - write string from buffer
            // Output :  number of bytes write

            int virtAddr = machine->ReadRegister(4); // Write buffer address from 4th register

            char *buffer = User2System(virtAddr, 255); // Copy string from userSpace

            // calculate length of string
            int length = 0;
            while (buffer[length] != 0)
                length++;

            gSynchConsole->Write(buffer, length + 1); // write string to console
            machine->WriteRegister(2, length - 1);    // return n character
            delete buffer;
            break;
        }
        case SC_Exec:
        {
            // Function :   int Exec(char *filename)
            // Input :
            //    filename - name of executable file
            // Output :  -1: error
            //            n: processID

            int virtAddr = machine->ReadRegister(4);	            // read buffer Address from 4th register
            char* filename = User2System(virtAddr, MaxFileLength + 1);  // Copy filename (UserSpace) to SystemSpace

            if(filename == NULL)
            {
                printf("\n Not enough memory in System");
                machine->WriteRegister(2, -1);
                return; 
            }

            if (fileSystem->Open(filename) == NULL)
            {
                printf("\nExec:: Can't open this file.");
                machine->WriteRegister(2,-1);
                return;
            }

            // Return child process id
            int id = procTable->ExecUpdate(filename); 
            machine->WriteRegister(2,id);
            
            delete[] filename;	
            break;
        }
        case SC_PrintChar:
        {
            // Function :   void PrintChar(char c);
            // Input :
            //    c - character for printing
            // Output :  non
            char c = machine->ReadRegister(4); // Doc ki tu tu thanh ghi r4
            gSynchConsole->Write(&c, 1); // In ky tu tu bien c, 1 byte
        }
        case SC_Join:
        {       
            // Function :   int Join(SpaceId id); 	
            // Input :
            //    id - processID
            // Output :  n : exitcode of childprocess
            int pid = machine->ReadRegister(4);  // read buffer Address from 4th register
			
            int exitcode = procTable->JoinUpdate(pid);
			
            machine->WriteRegister(2, exitcode);
            break;
        }
        case SC_Exit:
        {
            // Function :   void Exit(SpaceId pid);	
            // Input :
            //    id - processID
            int pid = machine->ReadRegister(4);
		
            procTable->ExitUpdate(pid);
            break; 
        }
        default:
            printf("\n Unexpected user mode exception (%d %d)", which, type);
            interrupt->Halt();
        }

        IncreasePC(); // Nap dia chi thanh ghi PC moi vao
    }
    }
}
