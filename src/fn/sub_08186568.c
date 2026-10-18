#define Armor_UpdateRandomFlankAction42 sub_08186568
#define gRandomTableIndex gUnk_03005308
#define gRandomTable gUnk_0203B400
#include "global.h"

/* Tentative Armor-family behavior: aim a quarter turn to either side of the target. */
struct ArmorActor {
    u8 unk0[0x48];
    s16 x;
    s16 unk4a;
    s16 y;
    u8 unk4e[0xB3B - 0x4E];
    u8 direction;
    u8 unkB3C;
    u8 phase;
    u8 unkB3E[2];
    u8 actionStatus;
};
struct ArmorTarget {
    u8 unk0[0x30];
    s16 x;
    s16 unk32;
    s16 y;
};
extern struct ArmorTarget *gUnk_02000710;
extern u32 gUnk_03005308;
extern const u16 gUnk_0203B400[];
s32 Math_Atan2(s32, s32);
s32 sub_08184FE8(struct ArmorActor *, s32);

void sub_08186568(struct ArmorActor *p)
{
    u8 *q = &p->phase;
    u8 r;
    switch (*q) {
        case 0: {
            struct ArmorTarget *g = gUnk_02000710;
            s32 a = Math_Atan2(g->x - p->x, g->y - p->y);
            u32 random;
            gUnk_03005308 = (gUnk_03005308 + 1) & 0x3ff;
            random = *(const u16 *)((gUnk_03005308 * 2) + (u32)gUnk_0203B400);
            random &= 0x10;
            if (random)
                a += 0x40;
            else
                a -= 0x40;
            a += 0x10;
            a &= 0xe0;
            p->direction = a;
            if (sub_08184FE8(p, 0x2a)) {
                p->phase = 1;
            }
            break;
        }
        case 1:
            r = p->actionStatus;
            if (r == 0xff) {
                *q = r;
            }
            break;
    }
}
