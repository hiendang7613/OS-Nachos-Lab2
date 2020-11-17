#ifndef PCB_H
#define PCB_H

#include "thread.h"
#include "synch.h"

// Process Control Block
class PCB
{
private:

    Semaphore *joinSema;    // semaphore khi join
    Semaphore *mutex;      // semaphore khi doc quyen truy xuat

    int exitcode;       // ma tra ve
    int numwait;        // so process con no dang cho

    int pid;      // ID cua process
    int parentID; // ID cua process cha


public:
    Thread *thread;     // luong cua process

    PCB(int pid = 0);   // Contructor
    ~PCB();             // Destructor

    // === set/get method ===
    int GetID();                // tra ve processID
    int GetParentID();          // tra ve parentID 

    char *GetName();            // tra ve ten process

    void SetExitCode(int code); // set exitcode
    int GetExitCode();          // tra ve exitcode

    int GetNumWait();           // tra ve so luong process cho

    // === ===
    int Exec(char *filename, int pid);     // tao process con

    void JoinWait();            // doi process con
    void JoinRelease();         // bao process cha k can cho

    void IncNumWait();          // Tăng số tiến trình chờ
    void DecNumWait();          // Giảm số tiến trình chờ

};

#endif // PCB_H
