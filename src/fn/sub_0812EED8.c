#include "global.h"
#include "scene_actor_chase.h"

// confidence=low: preserves the existing tentative reaction-state interpretation.

static inline void SetState(struct SceneActorRegistryResetView *actor, u8 s, u8 p, u8 b)
{
    actor->state2A8 = s;
    actor->phase2A9 = p;
    actor->behavior2AA = b;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline void SetControl(struct SceneActorRegistryResetView *actor, void *cb, u8 kind)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = cb;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = 0;
}
// confidence=low: target contact flags enable a delayed return to route behavior.
static inline u8 HasTargetFlag(struct SceneActorReactionMotion **out,
                               struct SceneActorReactionMotion *motion, u32 mask)
{
    u32 value = motion->flags & mask;
    *out = motion;
    if (value)
        return 1;
    return 0;
}
static inline void SaveDelay(u8 *packet, void *control, u8 behavior)
{
    *(u32 *)(packet + 12) = 30;
    *(void **)(packet + 16) = control;
    packet[8] = 0;
    packet[9] = 0;
    packet[10] = behavior;
}
static inline u32 ActorFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    return actor->flags2CC & mask;
}
static inline void ClearTargetFlags(struct SceneActorReactionMotion *motion, u32 mask)
{
    motion->flags &= mask;
}
static inline void SetActorFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    actor->flags2CC |= mask;
}
static inline struct SceneActorReactionMotion *MakeMotionPointer(void *actor)
{
    u8 *bytes = actor;
    return (void *)(bytes + 0xdc);
}
u8 SceneActorReaction_BeginTargetRecovery(struct SceneActorRegistryResetView *actor)
{
    u8 *group = (u8 *)&actor->group.common;
    struct SceneActorReactionMotion *motion;
    if (!ActorFlags(actor, 0x400000) || !ActorFlags(actor, 0x200000) ||
        !HasTargetFlag(&motion, MakeMotionPointer(actor), 1))
        return 0;
    SetActorFlags(actor, 0x100);
    actor->field2C2 = 64;
    SceneActorAnimation_SelectDirectionalEntry(actor, 0);
    SetState(actor, 3, 1, 0);
    SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
    SetActorFlags(actor, 0x10);
    SaveDelay(group + 0x8c, actor->savedTarget, 9);
    Chase_SnapToRoute(*(struct ContactPosition **)(group + 0x200), group + 0x74);
    (*(struct ContactPosition **)(group + 0x200))->pad = 1;
    actor->field2C2 = 64;
    *(u16 *)((u8 *)actor + 0x414) = 0;
    *(u16 *)((u8 *)actor + 0x418) = 0;
    {
        u16 *field = &actor->flags2D4;
        u32 mask = ~0x380;
        *field &= mask;
    }
    ClearTargetFlags(motion, ~0x18);
    {
        u8 *control = (u8 *)actor + 0x420;
        if (ActorFlags(actor, 0x200000)) {
            {
                u32 *flags = &actor->flags2D0;
                u32 mask = ~0x1000000;
                *flags &= mask;
            }
            {
                u16 *flags = &actor->flags2D4;
                u32 mask = 0x200;
                *flags &= ~mask;
            }
            control[10] = 0;
            *(u32 *)(control + 24) = 0;
            *(u16 *)(control + 8) = 0;
        }
    }
    {
        u16 *field = &actor->flags2D4;
        u32 mask = ~0x400;
        *field &= mask;
    }
    actor->group.turn.turnsRemaining = 0xff;
    {
        u16 *field = &actor->flags2D4;
        u32 mask = ~0x1000;
        *field &= mask;
    }
    return 1;
}
