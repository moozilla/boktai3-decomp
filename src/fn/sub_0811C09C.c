#include "global.h"
#include "scene_object.h"

#define gRandomIndex  gUnk_03005304
#define gRandomValues gUnk_086141BC
extern u32 gUnk_03005304;
extern const u16 gUnk_086141BC[];
u32 ActorParticles_CreateAtOffset(void *, struct SceneExtent *, void *, u32, u32, u32, u32, u32,
                                  u32, u32, u32, u32, u32, u32, u32);
static inline u16 NextRandom(void)
{
    gUnk_03005304 = (gUnk_03005304 + 1) & 0x3ff;
    return gUnk_086141BC[gUnk_03005304];
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
