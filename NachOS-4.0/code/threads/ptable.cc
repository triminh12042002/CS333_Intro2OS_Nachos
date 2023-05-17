#include "synch.h"
#include "ptable.h"

char *PTable::GetFileName(int id) { return pcb[id]->GetFileName(); }

int PTable::GetFreeSlot()
{
    int i;
    for (i = 0; i < psize; i++)
    {
        if (!pcb[i])
            return i;
    }
    return -1;
}

int PTable::ExecUpdate(char *name)
{
    // ● Call bmsem->P() to avoid loading 2 processes at the same time.
    bmsem->P();

    // ● Check the validity of the program "name".
    if (name == NULL || strlen(name) == 0)
    {
        printf("Invalid program name.\n");
        bmsem->V();
        return -1; // in valid
    }
    // ● Check the existence of the program "name" by calling the Open method of
    // FileSystem class.

    int id = kernel->fileSystem->Open(name, 0);
    if (id == -1)
    {
        // printf("Fail to open file.\n");
        bmsem->V();
        return -1; // fail to open
    }

    // ● Compare program name and currentThread name to make sure this program
    // is not called Exec itself.
    if (strcmp(name, kernel->currentThread->getName()) == 0)
    {
        // printf("same name");
        bmsem->V();
        return -1;
    }

    // ● Find the empty position in the Ptable table. -
    int indexInPtable = this->GetFreeSlot();
    if (indexInPtable == -1)
    {
        // printf("no free slot");
        return -1;
    }
    //  If there is an empty slot, create a
    // new PCB with indexInPtable being the index of this slot,
    PCB *newPCB = new PCB(indexInPtable);
    pcb[indexInPtable] = newPCB;

    // parentID is indexInPtable of
    // currentThread. Mark used.
    pcb[indexInPtable]->parentID = kernel->currentThread->processID;

    // ● Call the method Exec of PCB class
    int pid = pcb[indexInPtable]->Exec(name, indexInPtable);

    // ● Call msem->V().
    bmsem->V();
    // ● Return the test results of PCB->Exec.
    return pid;
}

int PTable::JoinUpdate(int id)
{
    // ● Check the validity of the processID id and check if the process calling Join is
    // the parent of the processID = id:
    // ○ If not, report a reasonable error and return -1. Increment numwait and
    if (id < 0 || id >= psize || pcb[id] == NULL || pcb[id]->parentID != kernel->currentThread->processID)
    {
        // printf('fail');
        return -1;
    }
    pcb[pcb[id]->parentID]->IncNumWait();
    // call JoinWait() to wait for the child process to execute.
    pcb[id]->JoinWait();
    // ● After the child process finished executing, the process was released.
    // ● Handle exitcode.
    int exit_code = pcb[id]->GetExitCode();
    // ● Call ExitRelease() to allow the child process to exit
    return exit_code;
}

void PCB::IncNumWait()
{
    mutex->P();
    ++numwait;
    mutex->V();
}

int PTable::ExitUpdate(int exitcode)
{
    // ● Check the validity of the processID id and check if the process calling Join is
    // the parent of the processID = id:

    // ○ If not, report a reasonable error and return -1. Increment numwait and
    // call JoinWait() to wait for the child process to execute.
    // ● After the child process finished executing, the process was released.
    // ● Handle exitcode.
    // ● Call ExitRelease() to allow the child process to exit

    // i think below instruction is wrong, which is same as joinupdate
    return exitcode;
    
}

