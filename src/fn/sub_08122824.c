#include "global.h"
struct S {
    u8 f[0x130];
    s16 a;
    u8 g[0x3c - 2 + 0x2];
    u16 c;
};
u8 sub_0812266C(void);
void Collider_SetPosition(void *, void *, u32);
s32 Collider_Refresh(void *);
void sub_08122824(u8 *s)
{
    if (sub_0812266C() == 0) {
        if (*(s16 *)(s + 0x130) == 1) {
            u8 *p = s + 0xdc;
            Collider_SetPosition(p, s + 0xd4, 0);
            Collider_Refresh(p);
        }
        (*(u16 *)(s + 0x13c))++;
    }
}
