#include "global.h"

void *Heap_Alloc(u32);
void Memory_Clear(void *, u32);
void Heap_Free(void *);
s32 Crow_Init(void *);
void Crow_Destroy(void *);

void Crow_Create(void)
{
    void *m = Heap_Alloc(0x43c);
    if (m != 0) {
        Memory_Clear(m, 0x43c);
        if (Crow_Init(m) < 0) {
            Crow_Destroy(m);
            Heap_Free(m);
        }
    }
}
