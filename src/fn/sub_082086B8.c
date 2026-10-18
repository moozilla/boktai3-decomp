#include "global.h"
struct V {
    u16 x;
    u16 y;
    u32 z;
};
extern u32 gUnk_03005308;
struct R {
    u8 a;
    u8 b;
} __attribute__((packed));
extern struct R gUnk_0203B400[];
void sub_081263E0(void *, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32);
void CannonTurret_HandleDefeat(void *);
s32 sub_082086B8(u8 *p)
{
    s16 h = *((s16 *)(p + 0x146));
    if (!h) {
        if ((*((s32 *)(p + 0x18))) <= 11) {
            struct V v;
            struct V *q = &v;
            u32 r;
            *q = *((struct V *)(p + 0x40));
            q->y += 128;
            gUnk_03005308 = (gUnk_03005308 + 1) & 0x3ff;
            r = (*(&gUnk_0203B400[gUnk_03005308])).a;
            sub_081263E0(&v, 128, 32, 12, 12, 24, r, 32, 32, 1, h, 512);
        }
        if ((*((s32 *)(p + 0x18))) == 12) {
            CannonTurret_HandleDefeat(p);
        }
    }
    return 0;
}
