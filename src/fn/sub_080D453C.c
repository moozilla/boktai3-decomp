#include "global.h"
// confidence=low: consumes a nearby stimulus and attempts a tagged-position route.
#define Mummy_TryStimulusRoute sub_080D453C
#ifdef NONMATCHING
// fuzzy: 57.0%
// fix.py best (sweep after the inlined-insn fix)
// Full control flow recovered; guard, helper lifetimes and flag ordering still differ.
#include "scene_actor_chase.h"
u8 SceneStimulus_FindOverlap(s16, s16, u16, u8, u16);
void SceneActorPosition_ResolveTaggedPosition(void *, void *, void *, s8);
u8 SceneActor_TryTrackNearbyPlayer(void *, void *, u32);
void SceneGrid_SnapPositionToCellCenter(void *, u32);
void SceneActorAttachment_RetargetNearestPlayer(void *, void *);
static inline u8 Mummy_TryRoute(u16 *flags, u32 mask, void *route, struct ContactPosition *from,
                                struct ContactPosition *to)
{
    s32 result = Nav_IsPositionInMesh(route, from, to);
    if (!result) {
        *flags &= ~mask;
        return 0;
    }
    result = Terrain_ApplyBlocked(route, from, to);
    if (result) {
        if (result != 2)
            Memory_Clear((u8 *)from + 8, 8);
        return 1;
    }
    *flags &= ~mask;
    return 0;
}
static inline u8 Mummy_TestContactFlags(struct SceneActorReactionMotion *motion, u32 mask)
{
    if (motion->flags & mask)
        return 1;
    return 0;
}
static inline u8 Mummy_ShouldSkipStimulus(struct SceneActorRegistryResetView *a, u32 motion)
{
    if (!(a->flags2CC & 0x200000) || (motion & 4) || (a->flags2CC & 0x100) ||
        (a->flags2D4 & 0xE13F) || Mummy_TestContactFlags((void *)((u8 *)a + 0xDC), 16) ||
        *(u16 *)((u8 *)a + 0x2B8) || *(u16 *)((u8 *)a + 0x178))
        return 1;
    return 0;
}
u32 Mummy_TryStimulusRoute(struct SceneActorRegistryResetView *a)
{
    u8 *group;
    u8 *body;
    s8 tag;
    u32 started;
    struct ContactPosition *target;
    u32 motion;
    {
        u32 *flags = &a->flags2D0;
        u32 mask = 0xFEFFFFFF;
        motion = (*flags) & mask;
        do {
            *flags = motion;
        } while (0);
    }
    if (Mummy_ShouldSkipStimulus(a, motion))
        return 0;
    group = (u8 *)(&a->group);
    *((u8 *)(((u8 *)a) + 0x42A)) = 0;
    started = 0;
    body = a->context;
    tag = SceneStimulus_FindOverlap(a->position.x, a->position.z, *((u16 *)(((u8 *)a) + 0x422)),
                                    *((u8 *)(((u8 *)a) + 0x42B)), *((u16 *)(((u8 *)a) + 0x50)));
    if (tag < 0)
        return 0;
    if (!(*((u16 *)(((u8 *)a) + 0x428)))) {
        *((u16 *)(((u8 *)a) + 0x428)) = 180;
        Chase_SetCtl(a, *((void **)(group + 0x68)), 9);
        Chase_SetBehavior(a, 11);
        {
            u32 mask = 0x4000;
            *((u16 *)(body + 0x6F2)) = mask | (*((u16 *)(body + 0x6F2)));
        }
        started = 1;
    }
    if (a->flags2D4 & 0x80) {
        u16 *timer = (void *)(((u8 *)a) + 0x418);
        *timer += 300;
        if ((*timer) > (*((u16 *)(((u8 *)a) + 0x416))))
            *timer = *((u16 *)(((u8 *)a) + 0x416));
    }
    SceneActorPosition_ResolveTaggedPosition(group + 0x4C, ((u8 *)a) + 0x50,
                                             *((void **)(group + 0x1FC)), tag);
    target = (void *)(group + 0x58);
    if (SceneActor_TryTrackNearbyPlayer(a, (void *)(group + 0x58), *((u32 *)(group + 0x64))))
        return 1;
    Chase_SnapToRoute(*((struct ContactPosition **)(group + 0x200)), group + 0x74);
    (*((struct ContactPosition **)(group + 0x200)))->y = a->position.y;
    (*((struct ContactPosition **)(group + 0x200)))->pad = 0;
    SceneGrid_SnapPositionToCellCenter((void *)(group + 0x58), *((u32 *)(group + 0x64)));
    if (!Mummy_TryRoute(&a->flags2D4, 0x200, *((void **)(group + 0x1FC)), &a->position,
                        (void *)(group + 0x58))) {
        Callback_InvokeActor(a, *((void **)(group + 0x224)));
        return 1;
    }
    {
        u32 mask = 16;
        a->flags2CC |= mask;
    }
    a->field2C2 = Div(64, 3);
    a->flags2CC &= 0xFF7FFFFF;
    body[0x6EA] = 4;
    {
        u32 mask = 0x200;
        a->flags2D4 = mask | a->flags2D4;
    }
    {
        u32 mask = 0x1000000;
        a->flags2D0 |= mask;
    }
    SceneActorAttachment_RetargetNearestPlayer(a, (void *)(group + 0x58));
    return started != 0;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_080D453C);
#endif
