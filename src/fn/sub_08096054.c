// MATCHING_CONCESSION: register-pinning
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
    register u16 *f asm("r6");
    u8 taken;
    taken = SceneActorControl_TakeFlag(&actor->field2B2);
    f = &actor->flags2D4;
    if (taken) {
        {
            register u32 mask asm("r0") = 0x20;
            *f = mask | *f;
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
            SparkParticle_Spawn(&actor->position, Random4(), &range, actor->direction + 128, 10, 6,
                                2, 2);
        }
    }
    {
        u32 offset = 0x310;
        s32 current = *(s16 *)((u8 *)actor + offset);
        u32 priorOffset;
        s16 *field;
        s32 difference;
        priorOffset = 0x318;
        field = (void *)((u8 *)actor + priorOffset);
        difference = current - *field;
        priorX = field;
        x = Quarter(difference);
    }
    {
        register u32 offset asm("r4") = 0x314;
        s32 current = *(s16 *)((u8 *)actor + offset);
        register s16 *field asm("r1");
        s32 difference;
        offset += 8;
        asm("" : "+r"(offset));
        field = (void *)((u8 *)actor + offset);
        difference = current - *field;
        priorZ = field;
        z = Quarter(difference);
    }
    x = Clamp(x);
    z = Clamp(z);
    actor->motion.x += x;
    actor->motion.z += z;
    *priorX += x;
    *priorZ += z;
    if (AtRest(x, z)) {
        u32 ready;
        register u32 zero asm("r9");
        u32 stateTest;
        {
            u32 offset = 0x2B0;
            u8 *field = (u8 *)actor + offset;
            zero = 0;
            ready = 1;
            *field = ready;
        }
        Bee_ConsumeFlag80(actor);
        {
            u32 mask = ~0x28;
            u32 loaded = *f;
            mask &= loaded;
            stateTest = 0;
            *f = mask;
        }
        {
            u32 *flags = &actor->flags2CC;
            u32 mask = ~0x80;
            *flags &= mask;
        }
        {
            u32 mask = 0x100000;
            u32 offset = 0x2D0;
            u32 *flags = (void *)((u8 *)actor + offset);
            *flags |= mask;
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
        {
            u32 phase = 2;
            u32 offset = 0x2A8;
            u8 *field = (u8 *)actor + offset;
            *field = stateTest;
            *((u8 *)actor + 0x2A9) = phase;
            *((u8 *)actor + 0x2AA) = phase;
            *((u32 *)((u8 *)actor + 0x2AC)) = zero;
            *((u8 *)actor + 0x2B1) = ready;
        }
    }
}
