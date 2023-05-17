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
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
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
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions
//	is in machine.h.
//----------------------------------------------------------------------
int MaxFileLength = 32;

void increaseProgramCounter()
{
    // the 3 line code from slide but wrong
    /* Modify return point */

    // /* set previous programm counter (debugging only)*/
    // kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

    // /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
    // kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

    // /* set next programm counter for brach execution */
    // kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);

    // preference by anh duy thuc code
    /* set previous programm counter (debugging only)
     * similar to: registers[PrevPCReg] = registers[PCReg];*/
    kernel->machine->WriteRegister(PrevPCReg,
                                   kernel->machine->ReadRegister(PCReg));

    /* set programm counter to next instruction
     * similar to: registers[PCReg] = registers[NextPCReg]*/
    kernel->machine->WriteRegister(PCReg,
                                   kernel->machine->ReadRegister(NextPCReg));

    /* set next programm counter for brach execution
     * similar to: registers[NextPCReg] = pcAfter;*/
    kernel->machine->WriteRegister(
        NextPCReg, kernel->machine->ReadRegister(NextPCReg) + 4);
}

/*
Input: - User space address (int)
- Limit of content (int)
Output:- content (char*)
Purpose: Copy content from User memory space to System memory space
*/

char *stringUser2System(int addr, int convert_length = -1)
{
    int length = 0;
    bool stop = false;
    char *str;

    do
    {
        int oneChar;
        kernel->machine->ReadMem(addr + length, 1, &oneChar);
        length++;
        // if convert_length == -1, we use '\0' to terminate the process
        // otherwise, we use convert_length to terminate the process
        stop = ((oneChar == '\0' && convert_length == -1) ||
                length == convert_length);
    } while (!stop);

    str = new char[length];
    for (int i = 0; i < length; i++)
    {
        int oneChar;
        kernel->machine->ReadMem(addr + i, 1,
                                 &oneChar); // copy characters to kernel space
        str[i] = (unsigned char)oneChar;
    }
    return str;
}
/*
Input: - User space address (int)
- Limit of content (int)
- content (char[])
Output:- Number of bytes copied (int)
Purpose: Copy content from System memory space to User memory space
*/
void StringSys2User(char *str, int addr, int convert_length = -1)
{
    int length = (convert_length == -1 ? strlen(str) : convert_length);
    for (int i = 0; i < length; i++)
    {
        kernel->machine->WriteMem(addr + i, 1,
                                  str[i]); // copy characters to user space
    }
    kernel->machine->WriteMem(addr + length, 1, '\0');
}

void handle_SC_PrintString()
{
    int memPtr = kernel->machine->ReadRegister(4); // read address of C-string
    char *content = stringUser2System(memPtr);

    SysPrintString(content, strlen(content));
    delete[] content;
    return increaseProgramCounter();
}

void handle_SC_PrintNum()
{
    int num = kernel->machine->ReadRegister(4);
    SysPrintNum(num);
    return increaseProgramCounter();
}

// void Function_SC_Create()
// {
// 	// read first agr = filename
// 	int address = kernel->machine->ReadRegister(4);
// 	char *fileName = User2System(address, MaxFileLength + 1); // MaxFileLength là = 32
// 	printf(fileName);
// 	if (fileName == NULL)
// 	{
// 		printf("\n Not enough memory in system \n");
// 		// return error for program
// 		kernel->machine->WriteRegister(2, -1);
// 		delete fileName;
// 		return;
// 	}
// 	// Create file with size = 0
// 		// Dùng đối tượng fileSystem của lớp OpenFile để tạo file, việc tạo file này là sử dụng các
// 		// thủ tục tạo file của hệ điều hành Linux, chúng ta không quản ly trực tiếp các block trên
// 		// đĩa cứng cấp phát cho file, việc quản ly các block của file trên ổ đĩa là một đồ án khác
// 	if (!SysCreate(fileName))
// 	{

// 		printf("\n Error create file '%s'", fileName);
// 		kernel->machine->WriteRegister(2, -1);
// 		delete fileName;
// 		return;
// 	}
// 	//  else mean success create

// 	kernel->machine->WriteRegister(2,0); // trả về cho chương trình người dùng thành công
// 	delete fileName;
// 	return increaseProgramCounter();

// }

void handle_SC_CreateFile()
{
    int address = kernel->machine->ReadRegister(4);
    char *fileName = stringUser2System(address);

    if (SysCreate(fileName))
        kernel->machine->WriteRegister(2, 0);
    else
        kernel->machine->WriteRegister(2, -1);

    delete[] fileName;
    return increaseProgramCounter();
}

void handle_SC_Open()
{
    int address = kernel->machine->ReadRegister(4);
    char *fileName = stringUser2System(address);
    int type = kernel->machine->ReadRegister(5);

    kernel->machine->WriteRegister(2, SysOpen(fileName, type));

    delete fileName;
    return increaseProgramCounter();
}

void handle_SC_Close()
{
    int id = kernel->machine->ReadRegister(4);
    kernel->machine->WriteRegister(2, SysClose(id));
    return increaseProgramCounter();
}

void handle_SC_Read()
{
    int address = kernel->machine->ReadRegister(4);
    int charCount = kernel->machine->ReadRegister(5);
    char *content = stringUser2System(address, charCount);
    int idOfFile = kernel->machine->ReadRegister(6);
    kernel->machine->WriteRegister(2, SysRead(content, charCount, idOfFile));
    StringSys2User(content, address, charCount);
    delete[] content;
    return increaseProgramCounter();
}

void handle_SC_Write()
{
    int address = kernel->machine->ReadRegister(4);
    int charCount = kernel->machine->ReadRegister(5);
    char *content = stringUser2System(address, charCount);
    int idOfFile = kernel->machine->ReadRegister(6);
    kernel->machine->WriteRegister(2, SysWrite(content, charCount, idOfFile));
    StringSys2User(content, address, charCount);
    delete[] content;
    return increaseProgramCounter();
}

void handle_SC_Seek()
{
    int seekPos = kernel->machine->ReadRegister(4);
    int fileId = kernel->machine->ReadRegister(5);
    kernel->machine->WriteRegister(2, SysSeek(seekPos, fileId));
    return increaseProgramCounter();
}

// ● Declare the function int Exec(SpaceID id)
// ● Read the address of the program name “name” from register r4. The program
// name is now in the user space.(virtAddr)
// ● Move the program name from user space to system space:
// ○ If there is an error, it will report "Cannot open the file" and assign -1 to
// register r2.
// ○ If there is no error, call pTab->ExecUpdate(name), return and save the
// result of executing this method into register r2

void handle_SC_Exec()
{
    int virtAddr;
    virtAddr = kernel->machine->ReadRegister(
        4); // doc dia chi ten chuong trinh tu thanh ghi r4
    char *name;
    name = stringUser2System(virtAddr); // Lay ten chuong trinh, nap vao kernel
    if (name == NULL)
    {
        DEBUG(dbgSys, "\n Not enough memory in System");
        ASSERT(false);
        kernel->machine->WriteRegister(2, -1);
        return increaseProgramCounter();
    }

    kernel->machine->WriteRegister(2, SysExec(name));
    // DO NOT DELETE NAME, THE THEARD WILL DELETE IT LATER
    // delete[] name;

    return increaseProgramCounter();
}

// ● Declare the function int Join(SpaceID id)
// ● Read the id of the process which wants to join from the register r4.
// ● Call pTab->JoinUpdate(id) and save the result in register r2

void handle_SC_Join()
{
    int id = kernel->machine->ReadRegister(4);
    kernel->machine->WriteRegister(2, SysJoin(id));
    return increaseProgramCounter();
}

// ● Read the exitStatus from the register r2
// ● Call pTab-> ExitUpdate(exitStatus) and save the result in the register r2

void handle_SC_Exit()
{
    int id = kernel->machine->ReadRegister(4);
    kernel->machine->WriteRegister(2, SysExit(id));
    return increaseProgramCounter();
}

// Used to create a new semaphore
// ● Declare the function int CreateSemaphore(char* name, int semval)

int CreateSemaphore(char *name, int initialValue)
{
    int res = kernel->semTab->Create(name, initialValue);

    if (res == -1)
    {
        DEBUG('a', "\nError creating semaphore");
        delete[] name;
        return -1;
    }

    return 0;
}
// ● Read the “name” address from the register r4
// ● Read the “semval” value from the register r5
// ● Transfer the “name” from the user space to system space
// ● Call semTab-> Create(name, semval) to create semaphore. If false, given an
// error

// ● Save the result in the register r2

void handle_SC_CreateSemaphore()
{
    // ● Read the “name” address from the register r4
    int nameAddress = kernel->machine->ReadRegister(4);
    // ● Read the “semval” value from the register r5
    int semval = kernel->machine->ReadRegister(5);
    // ● Transfer the “name” from the user space to system space
    char *name = stringUser2System(nameAddress);

    // ● Call semTab-> Create(name, semval) to create semaphore. If false, given an
    kernel->machine->WriteRegister(2, CreateSemaphore(name, semval));
    delete[] name;

    return increaseProgramCounter();
}

// The syscall SC_Up uses the class Stable to release the waiting process
// ● Declare the function int Up(char* name)
int Up(char *name)
{

    return 0;
}
// ● Read the “name” address from the register r4
// ● Transfer the “name” from the user space to system space
// ● Check if this "name" Semaphore is in the sTab table, if not, given an error
// ● Call the method Signal() of the Stable class
// ● Save the result in the register r2

void handle_SC_Up()
{
    // ● Read the “name” address from the register r4
    int nameAddress = kernel->machine->ReadRegister(4);
    // ● Transfer the “name” from the user space to system space
    char *name = stringUser2System(nameAddress);
    if (name == NULL)
    {
        ASSERT(false);
        kernel->machine->WriteRegister(2, -1);
        delete[] name;
        return increaseProgramCounter();
    }
    int res = kernel->semTab->Signal(name);
    kernel->machine->WriteRegister(2, res);
    delete[] name;
    return increaseProgramCounter();
}

// The syscall SC_Down uses the class Stable to perform the wait operation
// ● Declare the function int Down(char* name)
// ● Read the “name” address from the register r4
// ● Transfer the “name” from the user space to system space
// ● Check if this "name" Semaphore is in the sTab table, if not, given an error
// ● Call the method Wait() of the Stable class
// ● Save the result in the register r2

void handle_SC_Down()
{
    // ● Read the “name” address from the register r4
    int nameAddress = kernel->machine->ReadRegister(4);
    // ● Transfer the “name” from the user space to system space
    char *name = stringUser2System(nameAddress);
    if (name == NULL)
    {
        ASSERT(false);
        kernel->machine->WriteRegister(2, -1);
        delete[] name;
        return increaseProgramCounter();
    }
    int res = kernel->semTab->Wait(name);
    kernel->machine->WriteRegister(2, res);
    delete[] name;
    return increaseProgramCounter();
}


void ExceptionHandler(ExceptionType which)
{
    int type = kernel->machine->ReadRegister(2);

    DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

    switch (which)
    {
    case SyscallException:
        switch (type)
        {
        case SC_Halt:
            DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

            SysHalt();

            ASSERTNOTREACHED();
            break;

        case SC_Add:
            DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");

            /* Process SysAdd Systemcall*/
            int result;
            result = SysAdd(/* int op1 */ (int)kernel->machine->ReadRegister(4),
                            /* int op2 */ (int)kernel->machine->ReadRegister(5));

            DEBUG(dbgSys, "Add returning with " << result << "\n");
            /* Prepare Result */
            kernel->machine->WriteRegister(2, (int)result);

            /* Modify return point */
            increaseProgramCounter();

            return;

            // ASSERTNOTREACHED();

            break;
        case SC_PrintString:
            return handle_SC_PrintString();
        case SC_PrintNum:
            return handle_SC_PrintNum();
        case SC_Create:
            return handle_SC_CreateFile();
        case SC_Open:
            return handle_SC_Open();
        case SC_Close:
            return handle_SC_Close();
        case SC_Read:
            return handle_SC_Read();
        case SC_Write:
            return handle_SC_Write();
        case SC_Seek:
            return handle_SC_Seek();
        case SC_Exec:
            return handle_SC_Exec();
        case SC_Join:
            return handle_SC_Join();
        case SC_Exit:
            return handle_SC_Exit();
        case SC_CreateSemaphore:
            return handle_SC_CreateSemaphore();
        case SC_Up:
            return handle_SC_Up();
        case SC_Down:
            return handle_SC_Down();
        default:
            cerr << "Unexpected system call " << type << "\n";
            break;
        }
        break;
    default:
        cerr << "Unexpected user mode exception" << (int)which << "\n";
        break;
    }
    ASSERTNOTREACHED();
}
