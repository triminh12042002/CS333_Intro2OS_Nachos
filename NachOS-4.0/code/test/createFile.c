#include "syscall.h"


int main()
{
    char *fileName = "abcxyz2";

    if( Create(fileName != 0) ){
        PrintString("create successfully file: ");
        PrintString(fileName);
        PrintString("\n");
        // PrintNum(123);
    }
    else
        PrintString("fail to create\n");

    Halt();
    /* not reached */
}