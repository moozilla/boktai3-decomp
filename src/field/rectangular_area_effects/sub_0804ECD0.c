#include "engine/shared/area_effect_link.h"
void Player_AddEnergy(struct Player *, s32);
static inline u32 AreaEffect_GetPlayerMode(struct Player *player)
{
    u32 mask = 32;
    u32 flags = player->statusFlags;
    u32 mode;
    if (flags & mask)
        mode = 2;
    else {
        u32 negative = 0 - (flags & 16);
        mode = negative >> 31;
    }
    return mode;
}
static inline u16 AreaEffect_GetLinkValue(s32 selected)
{
    struct AreaEffectLinkState *link = gLinkSession;
    if (!link || selected < 0)
        return 0;
    return link->playerValues[selected];
}
void AreaEffect_UpdateLinkPlayerResources(struct AreaEffectManager *manager)
{
    s32 i;
    for (i = 0; i < 4; i++) {
        struct Player *player = gPlayers[i];
        if (player && player->kind == 1) {
            u32 mode = AreaEffect_GetPlayerMode(player);
            s32 amount;
            if (player->actionMode == 6) {
                if (mode == 2)
                    manager->resourceFractions[i] += 40;
                else if (mode == 0)
                    manager->resourceFractions[i] += 5;
            } else {
                if (mode == 2)
                    manager->resourceFractions[i] += AreaEffect_GetLinkValue(i) * 8;
                else if (mode == 1)
                    manager->resourceFractions[i] += AreaEffect_GetLinkValue(i);
            }
            amount = 0;
            while (manager->resourceFractions[i] >= 120) {
                manager->resourceFractions[i] -= 120;
                amount++;
            }
            if (amount > 0)
                Player_AddEnergy(player, amount);
        }
    }
}
