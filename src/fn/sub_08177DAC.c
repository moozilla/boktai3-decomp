#include "global.h"
#define Hud_HideAuxiliaryElements sub_08177DAC
struct HudElement {
    u8 pad0[8];
    u32 flags;
    u8 padC[0x54];
};
struct HudManager {
    u8 pad0[0x4988];
    struct HudElement pair[2];
    u8 pad4A48[0x28];
    struct HudElement group[6];
};
void sub_08177DAC(struct HudManager *p)
{
    struct HudElement *e;
    s32 i;
    e = p->pair;
    {
        u32 mask = 1;
        for (i = 1; i >= 0; e++, i--)
            e->flags |= mask;
    }
    e = p->group;
    {
        u32 mask = 1;
        for (i = 5; i >= 0; e++, i--)
            e->flags |= mask;
    }
}
