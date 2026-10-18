#include "global.h"

extern void Anim_Remove(void *);
extern void Sprite_Remove(void *);

void GameStart_Destroy(u8 *p)
{
    s32 i;
    u8 *q;
    u8 *a, *b;
    vu16 *r = (vu16 *)0x03004BD8;
    s32 m = 4;
    *r = *r & -m;
    q = p + 0x3b4;
    for (i = 1; i >= 0; i--) {
        Anim_Remove(q);
        q += 0x60;
    }
    a = p + 0xa4;
    b = p + 0x1c;
    q = p + 0x1d4;
    for (i = 4; i >= 0; i--) {
        Anim_Remove(q);
        q += 0x60;
    }
    Sprite_Remove(p + 0x474);
    Anim_Remove(p + 0x12c);
    Anim_Remove(a);
    Anim_Remove(b);
}
