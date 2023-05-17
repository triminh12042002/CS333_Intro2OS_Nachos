#include "sem.h"
#include "synch.h"

Sem::Sem(char *na, int i)
{
    strcpy(this->name, na);
    sem = new Semaphore(this->name, i);
}

char *Sem::GetName()
{
    return this->name;
}

// Down(sem)
void Sem::wait()
{
    sem->P();
} 

// Up(sem)
void Sem::signal()
{
    sem->V();
} 
