#include "global.h"

#include "scene_actor_reset.h"
#include "engine.h"
#include "monster_particle_random.h"

// confidence=low: spider action tick; restores entry state, picks a route or delayed recovery each frame.
struct SpiderRecoveryContext {
    u8 pad[0x62A];
    u16 delay;
    u16 flags;
};

void Spider_SelectRandomEntry(void *);
void SceneActorMovement_ResolveRoutePosition(void *);
void SceneActorPlacement_TriggerTetherAction(void *);
s32 SceneActor_IsWithinTetherRange(void *, u32, u32);
void Spider_SelectRecoveryRoute();

static inline u8 TakeEntry(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}
static inline u8 TestFlags(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
static inline void SetControl(struct ChaseActor *a, void *callback, u32 kind)
{
    a->base.field2B2 = 1;
    a->base.field2B0 = 0;
    a->base.field2B5 = kind;
    a->base.target = callback;
    a->base.field2C8 = 0;
    {
        u32 *p = &a->base.flags114;
        u32 mask = ~1;
        *p &= mask;
    }
    a->base.word104 = 0;
}
static inline void SetState(struct ChaseActor *a, u8 state, u8 phase, u8 behavior)
{
    a->base.state2A8 = state;
    a->base.phase2A9 = phase;
    a->base.behavior2AA = behavior;
    a->base.counter2AC = 0;
    a->base.field2B1 = 1;
}

void sub_080A34BC(struct ChaseActor *a)
{
    u8 *group = (u8 *)&a->group.common;
    struct SpiderRecoveryContext *ctx = a->base.context;

    if (TakeEntry(&a->base.field2B1)) {
        SetControl(a, a->base.savedTarget, 0);
        a->base.field2C2 = 0x40;
    }
    if ((u8)Callback_InvokeActor(a, a->base.contactQuery.opaque) == 0) {
        u32 four = 4;
        if (TestFlags(ctx->flags, four)) {
            if ((u8)SceneActor_IsWithinTetherRange(a, 0x40, 0x40)) {
                a->base.field2C2 = 0x40;
                Spider_SelectRandomEntry(group + 0x74);
                SceneActorMovement_ResolveRoutePosition(a);
                ctx->delay = 0;
                SetState(a, 0, 0, 15);
                SetControl(a, Spider_SelectRecoveryRoute, 0x22);
            } else {
                SceneActorPlacement_TriggerTetherAction(a);
            }
        } else if ((s32)a->base.counter2AC > 60) {
            Spider_SelectRandomEntry(group + 0x74);
            SceneActorMovement_ResolveRoutePosition(a);
            if (a->base.paletteOffset == 0) {
                if ((ParticleRandom_ReadDirectSample(gRandomTable) & 3) != 0) {
                    SetState(a, 0, 0, 15);
                    SetControl(a, Spider_SelectRecoveryRoute, 0x22);
                    ctx->delay = (ParticleRandom_ReadSample(gRandomTable) & 0x7F) + 60;
                } else {
                    SetState(a, 0, 0, 7);
                    SetControl(a, a->base.entryTarget228, 1);
                }
            } else {
                if ((ParticleRandom_ReadDirectSample(gRandomTable) & 7) == 0) {
                    SetState(a, 0, 0, 15);
                    SetControl(a, Spider_SelectRecoveryRoute, 0x22);
                    ctx->delay = (ParticleRandom_ReadSample(gRandomTable) & 0x7F) + 60;
                } else {
                    SetState(a, 0, 0, 7);
                    SetControl(a, a->base.entryTarget228, 1);
                }
            }
        } else {
            a->base.counter2AC++;
        }
    }
}
