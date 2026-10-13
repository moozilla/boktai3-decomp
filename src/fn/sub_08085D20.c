#include "global.h"

void *Heap_Alloc(u32);
void Memory_Clear(void *, u32);
void Heap_Free(void *);
s32 sub_08084F44(void *);
void sub_08084EB0(void *);

void sub_08085D20(void)
{
    void *m = Heap_Alloc(0x43c);
    if (m != 0) {
        Memory_Clear(m, 0x43c);
        if (sub_08084F44(m) < 0) {
            sub_08084EB0(m);
            Heap_Free(m);
        }
    }
}
