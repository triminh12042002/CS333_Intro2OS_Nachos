
#include "stable.h"
#include "synch.h"

int STable::Create(char *name,
                   int init)
{ // Check da ton tai semaphore nay chua?
    for (int i = 0; i < MAX_SEMAPHORE; i++)
    {
        if (bm->Test(i))
        {
            if (strcmp(name, semTab[i]->GetName()) == 0)
            {
                return -1;
            }
        }
    }
    // Tim slot tren bang semTab trong
    int id = this->FindFreeSlot();

    // Neu k tim thay thi tra ve -1
    if (id < 0)
    {
        return -1;
    }

    // Neu tim thay slot trong thi nap Semaphore vao semTab[id]
    this->semTab[id] = new Sem(name, init);
    return 0;
}

int STable::FindFreeSlot()
{
    return this->bm->FindAndSet();
}

int STable::Signal(char* name) {
    for (int i = 0; i < MAX_SEMAPHORE; i++) {
        // test sem
        if (bm->Test(i)) {
            if (strcmp(name, semTab[i]->GetName()) == 0) {
                semTab[i]->signal();
                return 0;
            }
        }
    }
    // printf("not found sem");
    return -1;
}

int STable::Wait(char* name) {
    for (int i = 0; i < MAX_SEMAPHORE; i++) {
        if (bm->Test(i)) {
            if (strcmp(name, semTab[i]->GetName()) == 0) {
                semTab[i]->wait();
                return 0;
            }
        }
    }
    printf("not found sem");
    return -1;
}