#include "area_effect.h"
// MATCHING_CONCESSION: register-pinning
void sub_081594F4(struct AreaEffectPlayer *, s32);
static inline u32 test(u32 *value, u32 mask)
{
    return *value & mask;
}
static inline u32 any(u32 mask)
{
    return gUnk_02000560 & mask;
}
void AreaEffect_AccumulatePlayerResource(struct AreaEffectManager *manager, s32 mode)
{
    struct AreaEffectManager *object = manager;
    s32 activeMode = mode;
    struct AreaEffectPlayer *player;
    if (gUnk_030053F4 & 0x100)
        object->idleFrame = 0;
    else if (gUnk_03005260)
        object->idleFrame = 0;
    else if (object->idleFrame <= 899)
        object->idleFrame++;
    if ((gUnk_030053F4 & 0x300) == 0x200 || any(4))
        return;
    {
        register struct AreaEffectPlayer *candidate asm("r0") = object->player;
        u32 kind = candidate->actionMode;
        player = candidate;
        if (kind == 6) {
            if (activeMode == 2)
                object->resourceFraction += 40;
            else if (activeMode == 0 && object->idleFrame <= 899)
                object->resourceFraction += 5;
        } else {
            if (activeMode == 2) {
                register s32 value asm("r0") = gUnk_02000710->unk876;
                value <<= 3;
                {
                    u32 fraction = object->resourceFraction;
                    value += fraction;
                    object->resourceFraction = value;
                }
            } else if (activeMode == 1 && object->idleFrame <= 899) {
                s32 value = gUnk_02000710->unk876;
                asm("" : "+r"(value));
                {
                    register u32 fraction asm("r2") = object->resourceFraction;
                    value += fraction;
                    object->resourceFraction = value;
                }
            }
        }
    }
    {
        register u32 kindOffset asm("r1") = 0x418;
        asm("" : "+r"(kindOffset));
        {
            register u8 *kindAddress asm("r0") = (u8 *)player + kindOffset;
            if (*kindAddress == 2)
                object->resourceFraction <<= 2;
        }
    }
    {
        s32 amount = 0;
        register u32 fraction asm("r3") = object->resourceFraction;
        if (fraction > 119) {
            u32 next;
            do {
                next = fraction - 120;
                amount++;
                fraction = next;
            } while ((u16)next > 119);
            object->resourceFraction = next;
        }
        if (amount > 0)
            sub_081594F4(player, amount);
    }
}
