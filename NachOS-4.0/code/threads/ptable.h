#ifndef PTABLE_H
#define PTABLE_H

#define MAX_PROCESS 10

#include "bitmap.h"
#include "pcb.h"
class PTable
{
private:
    int psize;
    Bitmap *bm;
    // in pcb
    PCB *pcb[MAX_PROCESS];
    Semaphore *bmsem;
    // processes at the same time
    // mark the locations that have been used
    // used to prevent the case of loading 2
public:
    // Constructor initialize the size of the PCB object to store the process
    //  size. Set the initial value to null
    PTable(int size);
    ~PTable();
    // Destructor
    int ExecUpdate(char *);
    int ExitUpdate(int);
    int JoinUpdate(int);
    int GetFreeSlot();
    // the new process information
    bool IsExist(int pid);
    void Remove(int pid);
    char *GetFileName(int id);
    // Process the syscall SC_Exec
    // Process the syscall SC_Exit
    // Process the syscall SC_Join
    // Find the free slot in PTable to save
    // Check a process exist or not
    // Delete the PID from the PTable
    // Return the process name
};

#endif