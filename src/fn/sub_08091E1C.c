#include "global.h"

void *Heap_Alloc(u32);
void Memory_Clear(void *, u32);
void Heap_Free(void *);
s32 Slime_Init(void *);
void sub_08090F58(void *);

void Slime_Create(void)
{
    void *m = Heap_Alloc(0x40c);
    if (m != 0) {
        Memory_Clear(m, 0x40c);
        if (Slime_Init(m) < 0) {
            sub_08090F58(m);
            Heap_Free(m);
        }
    }
}
