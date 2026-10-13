#include "global.h"

u8 *Heap_Alloc(u32);
void Memory_Clear(u8 *, u32);
s32 sub_080CFC14(u8 *);
void sub_080CFC08(u8 *);
void Heap_Free(u8 *);

void sub_080D0BFC(void)
{
    u8 *p = Heap_Alloc(0x6b0);
    if (p != 0) {
        Memory_Clear(p, 0x6b0);
        if (sub_080CFC14(p) < 0) {
            sub_080CFC08(p);
            Heap_Free(p);
        }
    }
}
