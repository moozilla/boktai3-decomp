#include "engine/shared/global.h"
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
void Hud_HideAuxiliaryElements(struct HudManager *p)
{
    struct HudElement *e;
    s32 i;
    for (i = 0, e = p->pair; i < 2; i++)
        e++->flags |= 1;
    for (i = 0, e = p->group; i < 6; i++)
        e++->flags |= 1;
}
