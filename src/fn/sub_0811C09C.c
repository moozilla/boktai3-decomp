
#ifdef NONMATCHING
// fuzzy: 93.1%
// foundry draft (worker best 93.1%); what differs is mostly register allocation and instruction order
#define ActorParticles_EmitOffsetBurst sub_0811C09C
#define gRandomIndex gUnk_03005304
#define gRandomValues gUnk_086141BC
#include "global.h"
#include "scene_object.h"
extern u32 gUnk_03005304;
extern const u16 gUnk_086141BC[];
u32 ActorParticles_CreateAtOffset(void *, struct SceneExtent *, void *, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32);
static inline u16 NextRandom(void)
{
    gUnk_03005304 = (gUnk_03005304 + 1) & 0x3ff;
    return gUnk_086141BC[gUnk_03005304];
}
void sub_0811C09C(void *actor)
{
    s32 i = 3;
    struct SceneExtent offset;
    do {
        offset.x = (NextRandom() & 31) - 7;
        offset.y = -(NextRandom() & 31);
        offset.z = 0;
        ActorParticles_CreateAtOffset(actor, &offset, 0, 0x8639, 0x75f6, 0, 0, 0, 0, 0x153, 30, 0, 0, 1, 0);
    } while (--i >= 0);
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0811C09C);
#endif
