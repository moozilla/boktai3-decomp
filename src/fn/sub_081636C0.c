#include "global.h"
struct P {
    u8 f0[0x342];
    u8 e;
};
extern u32 gUnk_02000210;
void Sprite_Remove(u8 *);
void Anim_Remove(u8 *);
u32 sub_081636C0(u8 *p)
{
    u8 *q;
    s32 i;
    if (((struct P *)p)->e != 0) {
        q = p + 0x18;
        for (i = 7; i >= 0; i--) {
            Sprite_Remove(q);
            q += 0x50;
        }
        Anim_Remove(p + 0x2b8);
    }
    return gUnk_02000210 = 0;
}
