#include "scene/areas/area_effect.h"
s32 AreaEffect_FindContainingEntry(struct SceneVec *);
void AreaEffect_UpdateGraphicsId(struct AreaEffectManager *);
void EffectManager_SpawnParticles(u32, u32, u32, void *, void *, u32, u32);
void AreaEffect_UpdateActionParticles(struct AreaEffectManager *manager)
{
    s32 active;
    if (!manager->visible) {
        s32 i;
        manager->visible = 1;
        for (i = 0; i < manager->count; i++) {
            if (manager->entries[i].enabled)
                manager->entries[i].spriteFlags &= ~1;
        }
    }
    active = 0;
    if (manager->player->kind == 1) {
        if (gGameState->flags868 & 2) {
            goto action;
        }
        if (AreaEffect_FindContainingEntry(&manager->player->position) > (0 - 1)) {
            active = 1;
        }
    }
    if (active) {
    action: {
        struct Player *player = manager->player;
        if (player->tetherMode == 7 && player->tetherPhase <= 3) {
            if ((u8)(player->tetherPhase - 2) <= 1 && manager->actionFrame <= 62) {
                if ((manager->actionFrame & 7) == 0) {
                    struct SceneVec position, size;
                    position = player->position;
                    position.x -= 100;
                    position.z -= 100;
                    position.y += 256;
                    size.x = 256;
                    size.y = 0;
                    size.z = 256;
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
        struct Player *player = manager->player;
        u32 mask = 32;
        u32 flags = player->statusFlags;
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
