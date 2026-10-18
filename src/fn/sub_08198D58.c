#include "global.h"

struct SceneEffectPool;

struct SceneEffectActor {
    u8 unk000[0x150];
    struct SceneEffectPool *owner;
    u8 unk154[4];
};

struct SceneEffectPool {
    u8 unk00[0x1C];
    u8 occupied;
    u8 unk1D[0x17];
    struct SceneEffectActor actors[1];
};

void SceneEffect_InitActor(struct SceneEffectActor *, s32, s32);

/* Tentative: claims the pool's single actor slot and initializes its mode. */
struct SceneEffectActor *SceneEffect_AllocateActor(struct SceneEffectPool *pool, s32 mode, s32 flag)
{
    struct SceneEffectActor *actor = NULL;
    struct SceneEffectActor *result;
    s32 i;

    for (i = 0; i < 1; i++) {
        if (!((pool->occupied >> i) & 1)) {
            actor = &pool->actors[i];
            pool->occupied |= 1 << i;
            break;
        }
    }
    if (actor) {
        actor->owner = pool;
        SceneEffect_InitActor(actor, mode, flag);
        result = actor;
    } else {
        result = NULL;
    }
    return result;
}
