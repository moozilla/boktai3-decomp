#include "global.h"

u8 *Heap_Alloc(u32);
void Memory_Clear(u8 *, u32);
s32 sub_080F0C20(u8 *);
void sub_080F0B8C(u8 *);
void Heap_Free(u8 *);

void Ivy_Create(void)
{
    u8 *p = Heap_Alloc(0x640);
    if (p != 0) {
        Memory_Clear(p, 0x640);
        if (sub_080F0C20(p) < 0) {
            sub_080F0B8C(p);
            Heap_Free(p);
        }
    }
}
