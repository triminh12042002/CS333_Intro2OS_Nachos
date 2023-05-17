#include "syscall.h"

int main() {
    char fileName[] = "abcxyz";
    int fileId;
    int position = 2;
    int res = 0;
    if ((fileId = Open(fileName, 0)) != -1) {
        PrintString("open file: ");
        PrintString(fileName);
        PrintString(" sucessfully,fileId: ");
        PrintNum(fileId);
        PrintString("\n");
        res = Seek(position, fileId);
        PrintString("pos : ");
        PrintNum(res);
        PrintString("\n");

        Close(fileId);
    } else
        PrintString("fail to open file\n");

    Halt();
    /* not reached */
}
