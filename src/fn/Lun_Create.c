#include "global.h"

void *Heap_Alloc(u32);
void Memory_Clear(void *, u32);
void Heap_Free(void *);
s32 sub_0809DD78(void *);
void sub_0809DCE4(void *);

void Lun_Create(void)
{
    void *m = Heap_Alloc(0x44c);
    if (m != 0) {
        Memory_Clear(m, 0x44c);
        if (sub_0809DD78(m) < 0) {
            sub_0809DCE4(m);
            Heap_Free(m);
        }
    }
}
