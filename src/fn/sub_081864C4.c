#include "global.h"

struct S {
    u8 f[0xB3D];
    u8 st;
    u8 sub;
    u8 g[0xB58 - 0xB3F];
    u8 obj[4];
};
s32 Armor_TryStartAction(void *, s32);
void Armor_QueueFallingParts(void *);
void Armor_UpdateAction10Sequence(struct S *p)
{
    u8 *q = &p->st;
    u8 s = *q;
    switch (s) {
        case 0:
            if (Armor_TryStartAction(p, 10) != 0) {
                *q = 1;
                p->sub = s;
            }
            break;
        case 1:
            if (*(u8 *)((u8 *)p + 0xB40) == 0xff) {
                {
                    Armor_QueueFallingParts(p->obj);
                }
                {
                    u32 zero = 0;
                    *q = 2;
                    p->sub = zero;
                }
            }
            break;
        case 2:
            if (p->sub > 0x13 && Armor_TryStartAction(p, 12) != 0)
                *q = 3;
            break;
        case 3:
            if (*(u8 *)((u8 *)p + 0xB40) == 0xff)
                *q = 0xff;
            break;
    }
}
