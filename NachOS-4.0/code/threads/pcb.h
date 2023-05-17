#ifndef PCB_H
#define PCB_H

class PCB
{
private:
    Semaphore *joinsem;
    Semaphore *exitsem;
    Semaphore *mutex;
    int exitcode;
    int numwait;

    Thread *thread;
    char filename[128];

public:
    int processID;

    int parentID;
    PCB(int id);
    ~PCB();
    // semaphore for join process
    // semaphore for exit process
    // exclusive access semaphore
    // the number of join process
    // The parent process’s ID
    // Constructor
    // Destructor
    // Load the program has the name is “filename” and the process id is pid
    int Exec(char *name, int pid);
    // Create a thread with the name is
    //  filename and the process id is pid

    int ExecUpdate(char* name);


    int GetID();
    // Return the PID of the current process
    int GetNumWait();
    // Return the number of the waiting process
    void JoinWait();
    // The parent process wait for the child
    // process finishes
    void ExitWait();
    // The child process finishes
    void JoinRelease();
    // The child process notice the parent
    // process
    void ExitRelease();
    // The parent process accept to exit the
    // child process
    void IncNumWait();
    // Increase the number of the waiting process
    void DecNumWait();
    // Decrease the number of the waiting process
    void SetExitCode(int);
    // Set the exit code for the process
    int GetExitCode();
    // Return the exitcode
    void SetFileName(char *);
    // Set the process name
    char *GetFileName();
    // Return the process name
};

#endif