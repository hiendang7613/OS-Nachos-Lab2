#include "pcb.h"
#include "addrspace.h"
#include "utility.h"
#include "system.h"

extern void StartProcess_byid(int pid);
//ProcTable *procTable;

PCB::PCB(int pid)
{
    this->pid = pid;
    parentID = -1;

    numwait = exitcode = 0;
    thread = NULL;

    joinSema = new Semaphore(" joinSema", 0); // 0 - chua thuc hien
    mutex = new Semaphore("mutex", 1); // 1 - dang thuc hien
}

PCB::~PCB()
{

    delete joinSema;
    delete mutex;
    if (thread != NULL && thread != currentThread)
    {
        thread->FreeSpace();
    }
}

int PCB::Exec(char *procName, int pid)
{
    // tranh nap dong thoi 2 process
    mutex->P();

    thread = new Thread(procName);
    // neu cap phat that bai, bao loi khong du bo nho, mutex->V() và return -1.
    if (thread == NULL)
    {
        printf("PCB::Exec:: Not enough memory..!\n");
        mutex->V();
        return -1;
    }

    this->pid = pid;
    parentID = currentThread->pcb->GetID();
    thread->pcb = this;
    thread->Fork(StartProcess_byid, pid);

    // giai mutex
    mutex->V();

    return pid;
}

#pragma region SET_GET_METHOD
int PCB::GetID()
{
    return pid;
}

int PCB::GetParentID()
{
    return parentID;
}

int PCB::GetNumWait()
{
    return numwait;
}

void PCB::SetExitCode(int ec)
{
    exitcode = ec;
}

int PCB::GetExitCode()
{
    return exitcode;
}

char *PCB::GetName()
{
    return thread->getName();
}   

#pragma endregion SET_GET_METHOD


void PCB::JoinWait()
{
    joinSema->P();
}

void PCB::JoinRelease()
{
    PCB* parent = procTable->GetPCB(parentID);
    if(parent==NULL){
        return;
    }
    parent->DecNumWait();
    if(parent->GetNumWait()==0){
        parent->joinSema->V();
    }
}

void PCB::IncNumWait()
{
    mutex->P();
    numwait++;
    mutex->V();
}

void PCB::DecNumWait()
{
    mutex->P();
    numwait--;
    mutex->V();
}

