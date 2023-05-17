#include "synch.h"
#include "pcb.h"
#include "ptable.h"

PCB::PCB(int id)
{
    this->processID = kernel->currentThread->processID;
    joinsem = new Semaphore("joinsem", 0);
    exitsem = new Semaphore("exitsem", 0);
    mutex = new Semaphore("multex", 1);
}

void StartProcess_2(void *pid)
{
    int id;
    id = *((int *)pid);
    // get process filename
    char *fileName = kernel->pTab->GetFileName(id);

    AddrSpace *space;
    space = new AddrSpace();
    space->Load(fileName);

    if (space == NULL)
    {
        printf("\nPCB::Exec: Can't create AddSpace.");
        return;
    }

    space->Execute();
    // kernel->currentThread->space = space;

    // space->InitRegisters();	// set the initial register values
    // space->RestoreState();	// load page table register

    // kernel->machine->Run();	// jump to the user progam
    ASSERT(FALSE); // machine->Run never returns;
                   // the address space exits
                   // by doing the syscall "exit"
}

int PCB::Exec(char *name, int pid)
{

    // ● Call mutex->P() to help avoid loading 2 processes at the same time.
    mutex->P();
    // ● Check if the thread has initialized successfully, if not, not enough memory
    // error messages, call mutex->V() and return.
    this->thread = new Thread(name);
    if (this->thread == NULL)
    {
        printf("\nPCB::Exec: Not enough memory!\n");
        mutex->V(); // Release semaphore
        return -1;  // return -1 for fail
    }
    // ● Set processID of this thread is id.
    this->thread->processID = pid;
    // ● Set the parentID of this thread to be the processID of the thread calling Exec
    this->parentID = kernel->currentThread->processID;
    // ● Call Fork(StartProcess_2,id) => We cast the thread to type int, then when
    // processing the StartProcess function we cast the thread to its correct type.
    this->thread->Fork(StartProcess_2, &this->thread->processID);

    mutex->V();
    // ● Returns id.
    return pid;
}

char *PCB::GetFileName()
{
    // cerr << "get file name" << ' ' << filename << endl;
    return filename;
}

void PCB::JoinWait()
{
    // ● Call joinsem->P() to switch to block state and stop, and wait for JoinRelease
    // to continue
    joinsem->P();
}

void PCB::ExitRelease()
{

    // ● Call exitsem->V() to release the waiting process
    exitsem->V();
}

int PCB::GetExitCode()
{
    return exitcode;
}

void PCB::JoinRelease(){
// ● Call joinsem->V() to release the waiting process, which called JoinWait()
    joinsem->V();
}


void PCB::ExitWait(){
// ● Call exitsem->V() to switch to block state and stop, and wait for ExitRelease
// to continue
    exitsem->V();
}


