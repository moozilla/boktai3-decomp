#include "global.h"
#include "scene_actor_reaction.h"
#include "scene_actor_control_phase.h"
#include "engine.h"
#include "ram.h"

extern const s16 gUnk_086149C4[];
void Lun_OnSpecialAnimation(void);
void SceneActorControl_ConsumeLiftFlagPenalty(struct ChaseActor *);
static inline u32 Random(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + RANDOM_TABLE_ADDR);
}
static inline u32 *ClearFlags(u32 *field, u32 mask)
{
    *field &= mask;
    return field;
}
static inline s32 Scale12(s32 n)
{
    if (n >= 0)
        return n >> 12;
    return -((-n) >> 12);
}
static inline void ClearVector(u32 *zero, struct ContactPosition *v)
{
    *zero = 0;
    CpuSet(zero, v, 0x05000002);
}
static inline void SetControl(struct ChaseActor *actor, void *callback, u32 kind)
{
    actor->base.field2B2 = 1;
    actor->base.field2B0 = 0;
    actor->base.field2B5 = kind;
    actor->base.target = callback;
    actor->base.field2C8 = 0;
    {
        u32 *flags = &actor->base.flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->base.word104 = 0;
}
static inline void SetState(struct ChaseActor *actor, u8 state, u8 phase, u8 behavior)
{
    actor->base.state2A8 = state;
    actor->base.phase2A9 = phase;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}
// confidence=low: initializes a random launch vector and ends the action on motion flag 0x10.
void Lun_UpdateLaunchedReaction(struct ChaseActor *actor)
{
    u8 *state = actor->base.context;
    u32 zero;
    u8 entered = SceneActorControl_TakeFlag(&actor->base.field2B1);
    u32 *flags = &actor->base.flags2D0;
    if (entered) {
        u8 angle;
        SetControl(actor, Lun_OnSpecialAnimation, 0x24);
        SceneActorControl_ConsumeLiftFlagPenalty(actor);
        angle = Random() & 255;
        ClearVector(&zero, (struct ContactPosition *)((u8 *)state + 0x424));
        actor->base.secondaryMotion.x = Scale12(gUnk_086149C4[angle] * 12);
        actor->base.secondaryMotion.y = 10;
        actor->base.secondaryMotion.z = Scale12(gUnk_086149C4[(angle + 64) & 255] * 12);
        flags = ClearFlags(&actor->base.flags2D0, ~0x100000);
    }
    {
        u32 mask = 0x2000;
        actor->base.flags2CC |= mask;
    }
    {
        u32 mask = 0x10;
        if (*flags & mask) {
            u32 value = 0x100000;
            *flags |= value;
            ClearVector(&zero, &actor->base.secondaryMotion);
            SetState(actor, 0, 0, 4);
            SetControl(actor, SceneActorControl_OnIdle, 16);
        }
    }
    actor->base.counter2AC++;
}
