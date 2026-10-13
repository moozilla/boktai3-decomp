#include "global.h"

u8 *Heap_Alloc(u32);
void Memory_Clear(u8 *, u32);
s32 sub_080EC3F8(u8 *);
void sub_080EC364(u8 *);
void Heap_Free(u8 *);

void sub_080ED2F0(void)
{
    u8 *p = Heap_Alloc(0x63c);
    if (p != 0) {
        Memory_Clear(p, 0x63c);
        if (sub_080EC3F8(p) < 0) {
            sub_080EC364(p);
            Heap_Free(p);
        }
    }
}
