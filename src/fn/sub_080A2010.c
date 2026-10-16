#include "global.h"

void *Heap_Alloc(u32);
void Memory_Clear(void *, u32);
void Heap_Free(void *);
s32 Crow_Init(void *);
void sub_080A13AC(void *);

void Crow_Create(void)
{
    void *m = Heap_Alloc(0x43c);
    if (m != 0) {
        Memory_Clear(m, 0x43c);
        if (Crow_Init(m) < 0) {
            sub_080A13AC(m);
            Heap_Free(m);
        }
    }
}
