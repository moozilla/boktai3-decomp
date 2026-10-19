#define PlayerCue_ApplyContactPower sub_0815E034
#define PlayerCue_ApplyColliderScale sub_0815DEEC
#define PlayerCue_ApplyFacingPowerBonus sub_08220B24
#include "global.h"
#include "player.h"
void sub_0815DEEC(struct Player *, struct PlayerCueCollider *);
void sub_08220B24(struct PlayerCueCollider *, struct PlayerCueCollider *, u32);
void sub_08220B78(struct PlayerCueCollider *, struct PlayerCueCollider *);
void sub_0815E034(struct Player *p, struct PlayerCueCollider *source, struct PlayerCueCollider *target)
{
    s32 power;
    u32 primary, secondary, flags;
    if (target->appliedPower) return;
    flags = source->flags;
    /* Both observed flag paths clamp the incoming power identically. */
    if (flags & 0x1000) {
        power = source->power;
        if (power <= 0) power = 1;
    } else {
        power = source->power;
        if (power <= 0) power = 1;
    }
    if (flags & 0x2000) secondary = 0;
    else secondary = target->flags & 0x3f;
    primary = target->primaryMask & 0x3f;
    if (flags & 0x40) {
        target->appliedPower = 0;
        target->reaction = 1;
        sub_08220B24(source, target, target->appliedPower);
        return;
    }
    if (flags & 0x3f) {
        u32 offset;
        s32 reduction;
        if (primary & flags) target->reaction = 4;
        else if (secondary & flags) target->reaction = 2;
        else target->reaction = 1;
        switch ((flags & 0x7f) - 1) {
        case 0: offset = 0x450; break;
        case 1: offset = 0x451; break;
        case 3: offset = 0x452; break;
        case 7: offset = 0x453; break;
        case 15: offset = 0x454; break;
        case 31: offset = 0x455; break;
        default: goto apply;
        }
        reduction = 5 - p->cueDefenses[offset - 0x450];
        target->appliedPower = power + (((u32)(power * 5)) >> 5) * reduction;
apply:
        sub_08220B24(source, target, power);
        sub_0815DEEC(p, target);
        sub_08220B78(source, target);
    } else {
        target->appliedPower = power;
        target->reaction = 1;
        sub_08220B24(source, target, power);
        sub_0815DEEC(p, target);
        sub_08220B78(source, target);
    }
    if (!target->appliedPower) target->appliedPower = 1;
}
