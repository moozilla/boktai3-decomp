#include "global.h"
struct E {
    u8 p[0x3ac];
};
struct A {
    u8 p0[0x18];
    u32 w18;
    u8 p1[0x38 - 0x1c];
    struct E e[4];
};
extern u32 gUnk_020005EC;
void sub_081FE244(struct A *, struct E *, s32);
void Sound_StopSong(u32);
u32 sub_081FE644(struct A *a)
{
    struct E *e = a->e;
    s32 i = 0;
    do {
        if (a->w18 & (1 << i))
            sub_081FE244(a, e, i);
        i++;
        e++;
    } while (i <= 3);
    Sound_StopSong(0x564);
    return gUnk_020005EC = 0;
}
