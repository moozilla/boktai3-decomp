// MATCHING_CONCESSION: register-pinning
#include "area_effect.h"
#include "input.h"
void Player_AddEnergy(struct AreaEffectPlayer *, s32);
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
    if (gEngineFlags & 0x100) {
        object->idleFrame = 0;
    } else if ((*(u16 *)gPadState)) {
        object->idleFrame = 0;
    } else if (object->idleFrame <= 899) {
        object->idleFrame++;
    }
    if (((gEngineFlags & 0x300) == 0x200) || any(4)) {
        return;
    }
    {
        struct AreaEffectPlayer *candidate = object->player;
        u32 kind = candidate->actionMode;
        player = candidate;
        if (kind == 6) {
            if (activeMode == 2) {
                object->resourceFraction += 40;
            } else if ((activeMode == 0) && (object->idleFrame <= 899)) {
                object->resourceFraction += 5;
            }
        } else if (activeMode == 2) {
            s32 value = gGameState->unk876;
            value <<= 3;
            {
                u32 fraction = object->resourceFraction;
                value += fraction;
                object->resourceFraction = value;
            }
        } else if (activeMode == 1 && object->idleFrame <= 899) {
            s32 value = gGameState->unk876;
            {
                u32 fraction = object->resourceFraction;
                value += fraction;
                object->resourceFraction = value;
            }
        }
    }
    {
        u32 kindOffset = 0x418;
        {
            u8 *kindAddress = (u8 *)player + kindOffset;
            if ((*kindAddress) == 2) {
                object->resourceFraction <<= 2;
            }
        }
    }
    {
        s32 amount = 0;
        u32 fraction = object->resourceFraction;
        if (fraction > 119) {
            u32 next;
            do {
                next = fraction - 120;
                DEBUG_NOP();
                amount++;
                fraction = next;
            } while ((u16)next > 119);
            object->resourceFraction = next;
        }
        if (amount > 0) {
            Player_AddEnergy(player, amount);
        }
    }
}
