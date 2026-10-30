#include "global.h"
#include "scene_actor_reaction.h"
#include "runtime.h"
/* Clears hit timers; at zero health hands control to the defeat target and records the defeat. */
void SceneActorRecord_RecordPair(u16, u16);
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
static inline u32 Flag2D0(struct SceneActorRegistryResetView *actor, u32 maskInput)
{
    u32 mask = maskInput;
    return actor->flags2D0 & mask;
}
static inline void Reaction_SetControl(struct SceneActorRegistryResetView *actor, u32 kind,
                                       void *control)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = control;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = 0;
}
static inline void Hide(struct SceneActorRegistryResetView *a)
{
    if (!a->drawableKind) {
        u32 *flags = a->drawable;
        u32 mask = ~1;
        *flags &= mask;
    } else {
        struct SceneActorAnimationTile *tile = (void *)((u8 *)a->drawable + 0x20);
        u32 mask = ~1;
        tile->flags &= mask;
    }
}
void SceneActorReaction_EndHitCheckDefeat(struct SceneActorRegistryResetView *actor)
{
    {
        u16 *timer = (u16 *)((u8 *)actor + 0x174);
        u32 zero = 0;
        *timer = zero;
        actor->field2B8 = zero;
    }
    {
        u16 *flags = &actor->flags2D4;
        u32 mask = ~0x2000;
        *flags = mask & *flags;
    }
    {
        u16 *flags = &actor->flags2D6;
        u32 mask = ~0x980;
        *flags = mask & *flags;
    }
    if (actor->health > 0)
        return;
    SceneActorTerrain_ClearCollisionTimers((void *)actor);
    *(u16 *)((u8 *)actor + 0x178) = 0;
    Hide(actor);
    {
        void *control = *(void **)((u8 *)actor + 0x234);
        Reaction_SetControl(actor, 17, control);
    }
    {
        u32 state = 3;
        actor->state2A8 = state;
        actor->phase2A9 = 0;
        actor->behavior2AA = 0;
        actor->counter2AC = 0;
        actor->field2B1 = 1;
    }
    actor->field2E0 = 32;
    if (!Test(gEngineFlags, 0x4800) && !Flag2D0(actor, 0x800))
        SceneActorRecord_RecordPair(gScriptEntryProcedure, actor->body.id);
    {
        u32 mask = 0x2000;
        actor->flags2CC |= mask;
    }
}
