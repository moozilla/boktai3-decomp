#include "global.h"
#include "player.h"

s32 Div(s32, s32);

static inline u32 MaskFlags(u32 *flags, u32 mask)
{
    return *flags & mask;
}
static inline u8 HasFixedDelayFlag(struct Player *p)
{
    u8 result;
    if (p) {
        u32 flags = MaskFlags(&p->actionFlags, 0x40);
        result = 1;
        if (flags)
            goto done;
    }
    result = 0;
done:
    return result;
}
static inline s32 IsFixedDelay(struct Player *p)
{
    if (!p)
        return 0;
    if (p->active && p->sceneMode == 1)
        return 1;
    return HasFixedDelayFlag(p);
}

// confidence=low: flags and active modifiers choose and shorten the linked action delay.
#define Player_ComputeLinkedActionDelay sub_0823C424
s32 Player_ComputeLinkedActionDelay(struct Player *p)
{
    s32 n;
    if (IsFixedDelay(p))
        goto full;
    n = Div(0, 100);
    if (n > 16) {
    full:
        n = 16;

    } else if (n <= 8) {
        n = 9;
    }
    if ((p->movementModifierKind == 2) && (p->movementModifierActive != 0)) {
        n -= 4;
    }
    if (p->movementSpeedReduction != 0) {
        n -= p->movementSpeedReduction;
    }
    if (n <= 1) {
        return 2;
    }
    return n;
}
