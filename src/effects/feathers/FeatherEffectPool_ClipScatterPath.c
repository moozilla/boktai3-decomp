#include "effects/feathers/feather_effect_pool.h"
#include "engine/math/monster_particle_random.h"
#include "scene/motion/scene_actor_terrain.h"

void SpriteEffectController_Create(struct ContactPosition *, struct ContactPosition *, s32, u32,
                                   u32, u32, u32, u32, u32, u32, u32, u32, s32, s32, s32, s32, s32,
                                   s32);

/* Tentative feather identity; clips the scatter position at rising terrain. */
u8 FeatherEffectPool_ClipScatterPath(struct Feather *feather, struct ContactPosition *spot,
                                     s32 cell, s32 target, s32 direction)
{
    struct ContactPosition velocity;
    struct ContactPosition center;

    ZeroFill8(&velocity);
    while (cell != target) {
        s32 previous = cell;
        s32 height;

        switch (direction) {
            case 0:
                cell++;
                break;
            case 1:
                cell += SceneActorTerrain_Width();
                break;
            case 2:
                cell--;
                break;
            case 3:
                cell -= SceneActorTerrain_Width();
                break;
        }
        if (*SceneActorTerrain_GetCell(cell) >> 4)
            return 1;
        height = *SceneActorTerrain_GetCell(previous) & 15;
        if (height < (*SceneActorTerrain_GetCell(cell) & 15)) {
            SceneActorTerrain_GetCenter(cell, &center);
            switch (direction) {
                case 0:
                    spot->x = center.x - 128;
                    return 1;
                case 1:
                    spot->z = center.z - 128;
                    return 1;
                case 2:
                    spot->x = center.x + 128;
                    break;
                case 3:
                    spot->z = center.z + 128;
                    break;
            }
            /* Clipping is done: the cell index now holds the random table address. */
            cell = (s32)gRandomTable;
            spot->y = (height << 8) + (ParticleRandom_ReadDirectSample((const u16 *)cell) & 255);
            if (feather->variant == 0) {
                s32 life = (ParticleRandom_ReadDirectSample((const u16 *)cell) & 63) + 30;

                SpriteEffectController_Create(spot, &velocity, 0x1C1A, 0xD1B8, 3, 0, 0x31, 0, 1, 0,
                                              0, life, 4, 1, feather->effectFlags, 30, 8, 8);
            } else {
                SpriteEffectController_Create(spot, &velocity, 0x1C1A, 0xD1B8, 0x12, 0, 0x11, 0, 1,
                                              0, 0, 0, 4, 1, feather->effectFlags, 30, 8, 8);
            }
            return 1;
        }
    }
#ifdef UBFIX
    /* Nothing in the way; the original leaves r0 as it stands. */
    return 0;
#endif
}
