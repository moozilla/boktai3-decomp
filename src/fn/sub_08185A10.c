#include "global.h"
struct Armor {
    u8 pad0[0x18];
    u8 navigation[0x30];
    s16 position[3];
    u8 pad4E[0xb39 - 0x4e];
    u8 direction;
    u8 padB3A[3];
    u8 state, timer;
};
struct ArmorPackedPosition {
    u32 x : 16, y : 16, z : 16, unused : 16;
};
extern u8 *gUnk_02000710;
s32 sub_0821F7A8(void *, void *, struct ArmorPackedPosition *);
u32 sub_0821FB8C(void *, void *, u32, u32);
s32 sub_08184FE8(struct Armor *, s32);
void Armor_UpdateTargetFacing(struct Armor *p)
{
    struct ArmorPackedPosition pos;
    if ((p->timer & 3) == 0) {
        u8 *target = gUnk_02000710;
        pos.x = *(u16 *)(target + 0x30);
        pos.y = *(u16 *)(target + 0x32);
        pos.z = *(u16 *)(target + 0x34);
        sub_0821F7A8(p->navigation, p->position, &pos);
    }
    sub_0821FB8C(p->navigation, p->position, p->direction, 0x40);
    if (p->state == 0) {
        if (sub_08184FE8(p, 0x1c))
            p->state = 255;
    }
}
