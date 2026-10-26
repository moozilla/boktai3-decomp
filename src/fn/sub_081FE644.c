#include "global.h"
#include "ram.h"
struct E {
    u8 p[0x3ac];
};
struct A {
    u8 p0[0x18];
    u32 w18;
    u8 p1[0x38 - 0x1c];
    struct E e[4];
};
void OrbitEffect_RemoveSlot(struct A *, struct E *, s32);
void Sound_StopSong(u32);
u32 OrbitEffect_DestroyManager(struct A *a)
{
    struct E *e = a->e;
    s32 i = 0;
    do {
        if (a->w18 & (1 << i))
            OrbitEffect_RemoveSlot(a, e, i);
        i++;
        e++;
    } while (i <= 3);
    Sound_StopSong(0x564);
    return gSlotController = 0;
}
