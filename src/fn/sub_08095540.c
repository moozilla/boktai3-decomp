#include "global.h"

void *Heap_Alloc(u32);
void Memory_Clear(void *, u32);
void Heap_Free(void *);
s32 Bee_Init(void *);
void sub_080946D0(void *);

void Bee_Create(void)
{
    void *m = Heap_Alloc(0x490);
    if (m != 0) {
        Memory_Clear(m, 0x490);
        if (Bee_Init(m) < 0) {
            sub_080946D0(m);
            Heap_Free(m);
        }
    }
}
