#include "global.h"
extern u8 *gUnk_02000488;
void sub_08049168(u8 *, u32);
struct P { u16 x, y, z, w; };
void sub_08049FFC(struct P *a)
{
    u8 *p = gUnk_02000488;
    if (p) {
        struct P **pp;
        *(struct P *)(p + 0x258) = *a;
        *(struct P *)(p + 0x50) = *a;
        pp = (struct P **)(p + 0x118);
        **pp = *a;
        (*pp)->x += *(u16 *)(p + 0x37E);
        (*pp)->z += *(u16 *)(p + 0x382);
        *(struct P *)(p + 0x3F0) = *a;
    }
}
