#include "global.h"
#include "scene_object.h"
#include "ram.h"

#define gRandomValues gUnk_086141BC
extern const u16 gUnk_086141BC[];
u32 ActorParticles_CreateAtOffset(void *, struct SceneExtent *, void *, u32, u32, u32, u32, u32,
                                  u32, u32, u32, u32, u32, u32, u32);
static inline u16 NextRandom(void)
{
    gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 0x3ff;
    return gUnk_086141BC[gEffectRandomTableIndex];
}
void ActorParticles_EmitOffsetBurst(void *actor)
{
    s32 i = 0;
    struct SceneExtent offset;
    do {
        offset.x = (NextRandom() & 31) - 7;
        offset.y = -(NextRandom() & 31);
        offset.z = 0;
        ActorParticles_CreateAtOffset(actor, &offset, 0, 0x8639, 0x75f6, 0, 0, 0, 0, 0x153, 30, 0,
                                      0, 1, 0);
    } while (++i < 4);
}
