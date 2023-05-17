#include "syscall.h"

int main() {
    char fileName[] = "abcxyz";
    int fileId;
    if ((fileId = Open(fileName, 0)) != -1) {
        PrintString("open file: ");
        PrintString(fileName);
        PrintString(" sucessfully,fileId: ");
        PrintNum(fileId);
        PrintString("\n");
        Close(fileId);
    } else
        PrintString("fail to open file\n");

    Halt();
    /* not reached */
}
