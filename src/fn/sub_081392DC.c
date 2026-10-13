#include "global.h"
struct E {
    u8 f[0x4];
    u8 b;
    u8 pad[0xdb];
};
extern u32 gUnk_020004CC;
void Sprite_Remove(void *);
s32 sub_081392DC(u8 *s)
{
    s32 i;
    u32 z;
    struct E *p = (struct E *)(s + 0x38);
    i = 0x3;
    do {
        if (p->b != 0)
            Sprite_Remove(p);
        p++;
    } while (--i >= 0);
    z = 0;
    gUnk_020004CC = z;
    return 0;
}
