#include "global.h"
#include "player.h"

s32 Div(s32, s32);
static inline s32 HasFlag(struct Player *p, u32 mask)
{
    u32 offset;
    u32 *flags;
    if (!p)
        return 0;
    offset = (u32) & ((struct Player *)0)->actionFlags;
    flags = (void *)((u8 *)p + offset);
    {
        u32 bits = mask;
        return *flags & bits;
    }
}

#define PlayerStats_AddHundredths sub_0815977C
void PlayerStats_AddHundredths(struct Player *p, s32 amount)
{
    if (p->kind == 1 && p->actionMode == 0) {
        if (!(HasFlag(p, 0x2000) && HasFlag(p, 0x4000))) {
            if (HasFlag(p, 0x2000))
                amount *= 2;
            else if (HasFlag(p, 0x4000))
                amount = Div(amount, 2);
        }
        p->statHundredths += amount;
        if (p->statHundredths > 999)
            p->statHundredths = 1000;
    }
}