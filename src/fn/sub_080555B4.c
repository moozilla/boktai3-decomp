#include "global.h"
extern u32 gUnk_02000494;
void Sprite_Remove(u8 *);
void sub_08013684(void);
void Heap_Free(u32);
s32 sub_080555B4(u8 *p)
{
    s32 i;
    for (i = 0; i < p[0x19]; i++) {
        u32 off = i * 0xb8;
        u32 *q;
        u8 *b;
        Sprite_Remove((u8 *)(off + (u32)p) + 0x44);
        b = p + 0xd0;
        q = (u32 *)(b + off);
        if (*q != 0) {
            sub_08013684();
            Heap_Free(*q);
        }
    }
    return gUnk_02000494 = 0;
}
