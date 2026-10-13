#include "global.h"

void *Heap_Alloc(u32);
void Memory_Clear(void *, u32);
void Heap_Free(void *);
s32 sub_080C3260(void *);
void sub_080C3240(void *);

void sub_080C44AC(void)
{
    void *m = Heap_Alloc(0x74c);
    if (m != 0) {
        Memory_Clear(m, 0x74c);
        if (sub_080C3260(m) < 0) {
            sub_080C3240(m);
            Heap_Free(m);
        }
    }
}
