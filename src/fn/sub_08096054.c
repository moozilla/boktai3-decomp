#include "global.h"
#include "scene_actor_control_phase.h"
#include "engine.h"
s32 Mod(s32, s32);
void Bee_ClearActionFlags(void *);
void Bee_ConsumeFlag80(void *);
void SceneActor_EndLinkedReaction(void *);
void Sound_PlaySong(u32);
void SparkParticle_Spawn(const void *, s32, const void *, s32, s32, s32, s32, s32);
static inline s32 Quarter(s32 v)
{
    return v >= 0 ? v >> 2 : -(-v >> 2);
}
static inline s32 Clamp(s32 v)
{
    if (v > 200)
        return 200;
    if (v < -200)
        return -200;
    return v;
}
static inline u8 AtRest(s32 x, s32 z)
{
    if (!x && !z)
        return 1;
    return 0;
}
static inline u32 Test(u16 *f, u32 maskInput)
{
    u32 mask = maskInput;
    return *f & mask;
}
static inline void SetPhase(struct SceneActorRegistryResetView *actor, u8 state, u8 phase,
                            u8 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline u32 Random4(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    u32 v;
    gRandomIndex = index;
    index *= 2;
    v = *(const u16 *)((u32)RANDOM_TABLE_ADDR + index);
    v &= 3;
    return v + 1;
}
// confidence=low: damage stagger; sparks while flagged, then converges saved motion and returns to idle.
void Ghost_UpdateHitStagger(struct SceneActorRegistryResetView *actor, s32 tick)
{
    u8 *ctx = actor->context;
    struct ContactPosition range;
    s32 x, z;
    s16 *priorX, *priorZ;
    u16 *f;
    u8 taken;
    taken = SceneActorControl_TakeFlag(&actor->field2B2);
    f = &actor->flags2D4;
    if (taken) {
        {
            u32 value = 0x20;
            value |= *f;
            *f = value;
        }
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 3);
        Bee_ClearActionFlags(actor);
        SceneActor_EndLinkedReaction(actor);
        if (Test(f, 8))
            Sound_PlaySong(0x12A);
    }
    if (Test(f, 8)) {
        if (Mod(tick, 12) == 0) {
            range.x = 60;
            range.y = 60;
            range.z = 40;
            SparkParticle_Spawn(&actor->body.position, Random4(), &range,
                                actor->body.direction + 128, 10, 6, 2, 2);
        }
    }
    {
        s32 current = *(s16 *)((u8 *)actor + 0x310);
        s16 *field = (void *)((u8 *)actor + 0x318);
        s32 difference = current - *field;
        priorX = field;
        x = Quarter(difference);
    }
    {
        s32 current = *(s16 *)((u8 *)actor + 0x314);
        s16 *field = (void *)((u8 *)actor + 0x31C);
        s32 difference = current - *field;
        priorZ = field;
        z = Quarter(difference);
    }
    x = Clamp(x);
    z = Clamp(z);
    actor->body.motion.x += x;
    actor->body.motion.z += z;
    *priorX += x;
    *priorZ += z;
    if (AtRest(x, z)) {
        actor->field2B0 = 1;
        Bee_ConsumeFlag80(actor);
        {
            struct SignedFlag {
                s32 bits : 16;
            };
            struct SignedFlag *flags = (void *)f;
            s32 m = ~0x28;
            m &= (u16)flags->bits;
            flags->bits = m;
        }
        {
            u32 *flags = &actor->flags2CC;
            s32 mask = ~0x80;
            *flags &= mask;
        }
        {
            u32 mask = 0x100000;
            actor->flags2D0 |= mask;
        }
        SceneActor_EndLinkedReaction(actor);
        {
            u32 mask = 0x10;
            u32 *flags = (u32 *)(ctx + 0x558);
            *flags |= mask;
        }
        {
            u16 v = *(u16 *)(ctx + 0x622);
            *(u16 *)(ctx + 0x624) = v;
        }
        SetPhase(actor, 0, 2, 2);
    }
}
