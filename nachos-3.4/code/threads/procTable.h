#ifndef PROCTABLE_H
#define PROCTABLE_H

#include "bitmap.h"
#include "pcb.h"
#include "synch.h"

#define ProcTableSize 10

class ProcTable
{
    BitMap *bitmap;                 // danh dau vi tri 
    PCB **pcbTable;
    Semaphore *bitmapSema;          // ngan nap dong thoi 2 process

public:
    PCB* GetPCB(int pid);
    
    ProcTable(int size = ProcTableSize);    // Contructor
    ~ProcTable();                           // Destructor

    int ExecUpdate(char *);         // Thuc thi process moi
    int ExitUpdate(int exitcode);   // Tat process voi exitcode
    int JoinUpdate(int);            // Process[pid] joinwait
    int JoinReleaseUpdate(int pid); // Process[pid] joinrelease

    int GetFreeSlot();      // Tim cho trong
    bool IsExist(int pid);  // pID co ton tai trong pcbTable ?
    void Remove(int pid);   // Ket thuc process va xoa bo nho cua pcb do
    char *GetProcessName(int pid); // Ten process[pid]

};
#endif // PROCTABLE_H
