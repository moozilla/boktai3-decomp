#include "global.h"
#define ResourceStation_Destroy sub_0804F418
extern u8 *gUnk_02000114;
void Sprite_Remove(u8 *);
void Anim_Remove(u8 *);
void Collider_Remove(u8 *);
u32 ResourceStation_Destroy(u8 *p)
{
    u8 *q;
    s32 i;
    Sprite_Remove(p + 0x18);
    q = p + 0x80;
    i = 3;
    do {
        Anim_Remove(q);
        q += 0x60;
        i--;
    } while (i >= 0);
    Collider_Remove(p + 0x200);
    return 0;
}
