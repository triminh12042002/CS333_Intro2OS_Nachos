/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__
#define __USERPROG_KSYSCALL_H__

#include "kernel.h"

#include "synchconsole.h"
// #include "ksyscallhelper.h"
// #include <stdlib.h>
#define MAX_NUM_LENGTH 11

char _numberBuffer[MAX_NUM_LENGTH + 2];

void SysHalt()
{
  kernel->interrupt->Halt();
}

int SysAdd(int op1, int op2)
{
  return op1 + op2;
}

void SysPrintString(char *buffer, int length)
{
  for (int i = 0; i < length; i++)
  {
    kernel->synchConsoleOut->PutChar(buffer[i]);
  }
}

void SysPrintNum(int num)
{
  if (num == 0)
    return kernel->synchConsoleOut->PutChar('0');

  // if (num == INT32_MIN) {
  //     kernel->synchConsoleOut->PutChar('-');
  //     for (int i = 0; i < 10; ++i)
  //         kernel->synchConsoleOut->PutChar("2147483648"[i]);
  //     return;
  // }

  if (num < 0)
  {
    kernel->synchConsoleOut->PutChar('-');
    num = -num;
  }
  int n = 0;
  while (num)
  {
    _numberBuffer[n++] = num % 10;
    num /= 10;
  }
  for (int i = n - 1; i >= 0; --i)
    kernel->synchConsoleOut->PutChar(_numberBuffer[i] + '0');
}

bool SysCreate(char *name)
{
  bool success;
  int fileNameLength = strlen(name);

  if (fileNameLength == 0)
  {
    DEBUG(dbgSys, "\nFile name can't be empty");
    success = false;
  }
  else if (name == NULL)
  {
    DEBUG(dbgSys, "\nNot enough memory in system");
    success = false;
  }
  else
  {
    DEBUG(dbgSys, "\nFile's name read successfully");
    if (!kernel->fileSystem->Create(name))
    {
      DEBUG(dbgSys, "\nError creating file");
      success = false;
    }
    else
    {
      success = true;
    }
  }

  return success;
}

int SysRead(char *buffer, int charCount, int fileId)
{
  if (fileId == 0)
  {
    return kernel->synchConsoleIn->GetString(buffer, charCount);
  }
  return kernel->fileSystem->Read(buffer, charCount, fileId);
}

int SysWrite(char *buffer, int charCount, int fileId)
{
  if (fileId == 1)
  {
    return kernel->synchConsoleOut->PutString(buffer, charCount);
  }
  return kernel->fileSystem->Write(buffer, charCount, fileId);
}

int SysOpen(char *fileName, int type)
{
  if (type != 0 && type != 1)
    return -1;

  int id = kernel->fileSystem->Open(fileName, type);
  if (id == -1)
    return -1;
  return id;
}

int SysClose(int id) { return kernel->fileSystem->Close(id); }

int SysSeek(int seekPos, int fileId)
{
  if (fileId <= 1)
  {
    DEBUG(dbgSys, "\nCan't seek in console");
    return -1;
  }
  return kernel->fileSystem->Seek(seekPos, fileId);
}

int SysExec(char *name)
{
  // cerr << "call: `" << name  << "`"<< endl;
  OpenFile *oFile = kernel->fileSystem->Open(name);
  if (oFile == NULL)
  {
    DEBUG(dbgSys, "\nExec:: Can't open this file.");
    return -1;
  }

  delete oFile;

  // Return child process id
  return kernel->pTab->ExecUpdate(name);
}

int SysJoin(int id)
{
  return kernel->pTab->JoinUpdate(id);
}

int SysExit(int id)
{
  return kernel->pTab->ExitUpdate(id);
}



#endif /* ! __USERPROG_KSYSCALL_H__ */
