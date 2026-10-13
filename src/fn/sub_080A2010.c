#include "global.h"

void *Heap_Alloc(u32);
void Memory_Clear(void *, u32);
void Heap_Free(void *);
s32 sub_080A1440(void *);
void sub_080A13AC(void *);

void sub_080A2010(void)
{
    void *m = Heap_Alloc(0x43c);
    if (m != 0) {
        Memory_Clear(m, 0x43c);
        if (sub_080A1440(m) < 0) {
            sub_080A13AC(m);
            Heap_Free(m);
        }
    }
}
