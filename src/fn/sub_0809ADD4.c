#include "global.h"

void *Heap_Alloc(u32);
void Memory_Clear(void *, u32);
void Heap_Free(void *);
s32 sub_08099B44(void *);
void sub_08099AAC(void *);

void sub_0809ADD4(void)
{
    void *m = Heap_Alloc(0x62c);
    if (m != 0) {
        Memory_Clear(m, 0x62c);
        if (sub_08099B44(m) < 0) {
            sub_08099AAC(m);
            Heap_Free(m);
        }
    }
}
