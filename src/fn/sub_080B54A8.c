#include "global.h"

u8 *Heap_Alloc(u32);
void Memory_Clear(u8 *, u32);
s32 sub_080B43BC(u8 *);
void sub_080B43B0(u8 *);
void Heap_Free(u8 *);

void sub_080B54A8(void)
{
    u8 *p = Heap_Alloc(0x720);
    if (p != 0) {
        Memory_Clear(p, 0x720);
        if (sub_080B43BC(p) < 0) {
            sub_080B43B0(p);
            Heap_Free(p);
        }
    }
}
