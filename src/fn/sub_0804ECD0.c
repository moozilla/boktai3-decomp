#include "area_effect_link.h"
// MATCHING_CONCESSION: register-pinning
void Player_AddEnergy(struct AreaEffectPlayer *, s32);
static inline u32 AreaEffect_GetPlayerMode(struct AreaEffectPlayer *player)
{
    u32 mask = 32;
    u32 flags = player->flags;
    u32 mode;
    if (flags & mask)
        mode = 2;
    else {
        u32 negative = 0 - (flags & 16);
        mode = negative >> 31;
    }
    return mode;
}
static inline u32 AreaEffect_GetLinkValue(s32 *index)
{
    s32 selected = *index;
    struct AreaEffectLinkState *link = gLinkSession;
    if (!link || selected < 0)
        return 0;
    return link->playerValues[selected];
}
void AreaEffect_UpdateLinkPlayerResources(struct AreaEffectManager *manager)
{
    struct AreaEffectManager *object = manager;
    s32 i = 0;
    u16 *fractions = object->resourceFractions;
    do {
        struct AreaEffectPlayer *player = gPlayers[i];
        s32 next = i + 1;
        if (player && player->stateTag == 1) {
            u32 mode = AreaEffect_GetPlayerMode(player);
            u32 offset;
            {
                u32 gain;
                u16 *destination;
                if (player->actionMode == 6) {
                    if (mode == 2) {
                        offset = i * 2;
                        destination = (u16 *)((u8 *)fractions + offset);
                        gain = *destination + 40;
                        goto storeFraction;
                    }
                    offset = i * 2;
                    if (mode != 0)
                        goto convertResources;
                    destination = (u16 *)((u8 *)fractions + offset);
                    gain = *destination + 5;
                    goto storeFraction;
                }
                if (mode == 2) {
                    gain = AreaEffect_GetLinkValue(&i);
                    offset = i * 2;
                    destination = (u16 *)((u8 *)fractions + offset);
                    gain <<= 3;
                    goto addLinkFraction;
                }
                offset = i * 2;
                if (mode != 1)
                    goto convertResources;
                gain = AreaEffect_GetLinkValue(&i);
                offset = i * 2;
                destination = (u16 *)((u8 *)fractions + offset);
            addLinkFraction: {
                register u32 previous asm("r4") = *destination;
                gain += previous;
            }
            storeFraction:
                *destination = gain;
            }
        convertResources: {
            s32 amount = 0;
            u16 *counter = (u16 *)((u8 *)fractions + offset);
            u16 value = *counter;

            next = i + 1;
            if (value > 119) {
                register u8 *base asm("r3") = (u8 *)object->resourceFractions;
                do {
                    u16 *counter = (u16 *)(base + offset);
                    u32 remaining = *counter - 120;
                    *counter = remaining;
                    amount++;
                    value = remaining;
                } while ((u16)value > 119);
            }
            if (amount > 0)
                Player_AddEnergy(player, amount);
        }
        }
        i = next;
    } while (i <= 3);
}
