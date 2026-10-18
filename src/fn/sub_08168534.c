#define Hud_SetElementGroupFlags sub_08168534
#include "global.h"

/* HUD interpretation and descriptive names are tentative. */
struct HudElement {
    u8 pad0[8];
    u32 flags;
    u8 padC[0x54];
};
struct Hud {
    u8 pad0[0xc88];
    struct HudElement first[3];
    u8 padDa8[0x80];
    struct HudElement second[3];
    u8 padF48[0x28];
    struct HudElement third[3];
    struct HudElement fourth[3], fifth[3], sixth[3];
};
static inline void SetFlag(struct HudElement *e, u32 mask)
{
    e->flags |= mask;
}
void sub_08168534(struct Hud *hud)
{
    s32 i;
    u32 second, third;
    u32 mask = 1;
    u32 *flags = &hud->first[0].flags;
    struct Hud *cursor = hud;
    second = 0xe28;
    third = 0xf70;
    i = 2;
    do {
        *flags |= mask;
        SetFlag((struct HudElement *)((u8 *)cursor + second), mask);
        SetFlag((struct HudElement *)((u8 *)cursor + third), mask);
        flags = (u32 *)((u8 *)flags + 0x60);
        cursor = (struct Hud *)((u8 *)cursor + 0x60);
    } while (--i >= 0);
    {
        u32 fifth, sixth;
        mask = 1;
        flags = &hud->fourth[0].flags;
        cursor = hud;
        fifth = 0x11b0;
        sixth = 0x12d0;
        i = 2;
        do {
            *flags |= mask;
            SetFlag((struct HudElement *)((u8 *)cursor + fifth), mask);
            SetFlag((struct HudElement *)((u8 *)cursor + sixth), mask);
            flags = (u32 *)((u8 *)flags + 0x60);
            cursor = (struct Hud *)((u8 *)cursor + 0x60);
        } while (--i >= 0);
    }
}
