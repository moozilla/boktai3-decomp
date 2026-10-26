#include "global.h"
#include "scene_object.h"
#include "game_state.h"
#include "ram.h"

extern const u16 gUnk_086141BC[];
u32 ActorEffect_SpawnAtActorPos(void *, struct SceneExtent *, struct SceneExtent *, u32, u32, u32,
                                u32, u32, u32, u32, u32, u32, u32, u32, u32);
static inline u32 NextRandom(void)
{
    gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 1023;
    return gUnk_086141BC[gEffectRandomTableIndex];
}
static inline u8 HasModifier(u32 mask)
{
    if (gGameState->flags868 & mask)
        return 1;
    return 0;
}
/* Tentative: emit paired particles when the actor reports an impact. */
void ActorParticles_SpawnImpactBurst(void *actor)
{
    struct SceneExtent offset, velocity;
    s32 i;
    u8 modifier;
    Sound_PlaySong(0x90);
    Memory_Clear(&offset, 8);
    modifier = HasModifier(8);
    for (i = 0; i < 4; i++) {
        offset.x = (NextRandom() & 31) - 7;
        velocity.x = (NextRandom() & 3) - 2;
        velocity.y = -(NextRandom() & 1);
        velocity.z = 0;
        ActorEffect_SpawnAtActorPos(actor, &offset, &velocity, 0x409C, 0x1C22, 4, 0, modifier, 0,
                                    0xE3, 30, 0, 512, 1, 0);
        ActorEffect_SpawnAtActorPos(actor, &offset, &velocity, 0x409C, 0x1C22, 3, 0, modifier, 0,
                                    0xE3, 12, 0, 1024, 1, 0);
    }
}
