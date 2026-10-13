#include "global.h"

u8 *Heap_Alloc(u32);
void Memory_Clear(u8 *, u32);
s32 sub_080DCC98(u8 *);
void sub_080DCC8C(u8 *);
void Heap_Free(u8 *);

void sub_080DDCF4(void)
{
    u8 *p = Heap_Alloc(0x720);
    if (p != 0) {
        Memory_Clear(p, 0x720);
        if (sub_080DCC98(p) < 0) {
            sub_080DCC8C(p);
            Heap_Free(p);
        }
    }
}
