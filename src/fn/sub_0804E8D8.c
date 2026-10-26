#include "area_effect.h"
s32 AreaEffect_FindContainingEntry(struct AreaEffectPosition *);
void AreaEffect_UpdateGraphicsId(struct AreaEffectManager *);
void EffectManager_SpawnParticles(u32, u32, u32, void *, void *, u32, u32);
struct PackedHalfword {
    u32 lo : 16;
    u32 hi : 16;
};
struct PackedPosition {
    struct PackedHalfword xy, zpad;
};
void AreaEffect_UpdateActionParticles(struct AreaEffectManager *manager)
{
    u32 step = 256;
    s32 active;
    if (!manager->visible) {
        s32 i;
        manager->visible = 1;
        i = 0;
        if (i < manager->count) {
            u32 mask = ~1;
            u8 *enabled = &manager->entries[0].enabled;
            u8 *base = (u8 *)manager;
            u8 *count = &manager->count;
            do {
                if (*enabled)
                    *(u32 *)(base + 0x34) &= mask;
                enabled += 0xA8;
                base += 0xA8;
                i++;
            } while (i < *count);
        }
    }
    active = 0;
    if (manager->player->stateTag == 1) {
        if (gGameState->flags868 & 2) {
            goto action;
        }
        if (AreaEffect_FindContainingEntry(&manager->player->position) > (0 - 1)) {
            active = 1;
        }
    }
    if (active) {
    action: {
        struct AreaEffectPlayer *player = manager->player;
        if (player->action == 7 && player->actionPhase <= 3) {
            if ((u8)(player->actionPhase - 2) <= 1 && manager->actionFrame <= 62) {
                if ((manager->actionFrame & 7) == 0) {
                    struct PackedPosition position, size;
                    *(struct AreaEffectPosition *)&position = player->position;
                    {
                        u32 x = (u16)(((struct AreaEffectPosition *)&position)->x - 100);
                        u32 highMask = 0xFFFF0000;
                        u32 xy = *(u32 *)&position.xy;
                        xy &= highMask;
                        xy |= x;
                        *(u32 *)&position.xy = xy;
                        {
                            u32 z = (u16)(((struct AreaEffectPosition *)&position)->z - 100);
                            *(u32 *)&position.zpad = (*(u32 *)&position.zpad & highMask) | z;
                        }
                        {
                            u32 y = (s32)xy >> 16;
                            u32 lowMask;
                            y += step;
                            y <<= 16;
                            lowMask = 0xFFFF;
                            {
                                u32 word = lowMask;
                                word &= xy;
                                word |= y;
                                *(u32 *)&position.xy = word;
                            }
                            *(u32 *)&size.xy = ((*(u32 *)&size.xy & highMask) | 256) & lowMask;
                            *(u32 *)&size.zpad = (*(u32 *)&size.zpad & highMask) | 256;
                        }
                    }
                    EffectManager_SpawnParticles(1, 1, 25, &position, &size, 180, 60);
                }
                manager->actionFrame++;
            }
        } else {
            manager->actionFrame = 0;
        }
    }

    } else {
        manager->actionFrame = active;
    }
    AreaEffect_UpdateGraphicsId(manager);
    {
        struct AreaEffectPlayer *player = manager->player;
        u32 mask = 32;
        u32 flags = player->flags;
        u32 mode;
        if (flags & mask) {
            mode = 2;
        } else {
            u32 negative = 0 - (flags & 16);
            mode = negative >> 31;
        }
        AreaEffect_AccumulatePlayerResource(manager, mode);
    }
}
