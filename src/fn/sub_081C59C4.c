#include "global.h"
struct E {
    u8 pad[0x60];
};
struct S {
    u8 f[0x78];
    struct E e[4];
};
void Anim_Remove(void *);
void Bg_ClearTilemap(u32);
void sub_081C59C4(struct S *p)
{
    s32 i;
    struct E *e = p->e;
    for (i = 3; i >= 0; i--) {
        Anim_Remove(e);
        e++;
    }
    Anim_Remove((u8 *)p + 0x1f8);
    Anim_Remove((u8 *)p + 0x258);
    Bg_ClearTilemap(0);
    Bg_ClearTilemap(1);
    Bg_ClearTilemap(2);
    Bg_ClearTilemap(3);
}
