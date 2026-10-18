#include "global.h"
#include "player.h"
s32 Div(s32, s32);
static inline u32 MaskFlags(u32 *flags, u32 mask)
{
    return *flags & mask;
}
static inline u8 HasFastMovement(struct Player *p)
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
static inline s32 IsFastMovement(struct Player *p)
{
    if (!p)
        return 0;
    if (p->active && p->sceneMode == 1)
        return 1;
    return HasFastMovement(p);
}
s32 Player_GetMovementSpeed(struct Player *p)
{
    s32 speed;
    if (IsFastMovement(p))
        goto fast;
    if (p && MaskFlags(&p->actionFlags, 0x40))
        goto fast;
    speed = Div((100 - p->movementLoadPercent) * 12, 100);
    if (speed > 16) {
    fast:
        speed = 16;
    } else if (speed <= 8)
        speed = 9;
    if (p->movementModifierKind == 2 && p->movementModifierActive)
        speed -= 4;
    if (p->movementSpeedReduction)
        speed -= p->movementSpeedReduction;
    if (p && MaskFlags(&p->actionFlags, 0x80))
        speed -= 3;
    if (speed <= 1)
        return 2;
    return speed;
}
