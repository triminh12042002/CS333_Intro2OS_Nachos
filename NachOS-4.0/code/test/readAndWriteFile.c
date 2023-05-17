#include "syscall.h"

#define MODE_READWRITE 0
#define MODE_READ 1

#define stdin 0
#define stdout 1

int main() {
    char contentOfFile[100];
    int write;

    int idOfFile = Open("abcxyz", MODE_READ);
    int read = Read(contentOfFile, 48, idOfFile);
    int lengthOfFile = 0;
    while (contentOfFile[lengthOfFile] != '\0') ++lengthOfFile;
    PrintString("file have ");
    PrintNum(lengthOfFile);
    PrintString(" char: ");
    PrintString(contentOfFile);
    PrintString("\n");
    Close(idOfFile);

    idOfFile = Open("abcxyz", MODE_READWRITE);
    // PrintNum(idOfFile);

    write = Write(contentOfFile, lengthOfFile, idOfFile);

    PrintString("Write to new file ");
    PrintNum(write);
    PrintString(" characters: ");
    PrintString(contentOfFile);
    PrintString("\n");
    Close(idOfFile);

    Halt();
}
